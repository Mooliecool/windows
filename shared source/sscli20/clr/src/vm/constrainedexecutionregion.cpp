// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
//
// Methods to support the implementation of Constrained Execution Regions (CERs). This includes logic to walk the IL of methods to
// determine the statically determinable call graph and prepare each submethod (jit, prepopulate generic dictionaries etc.,
// everything needed to ensure that the runtime won't generate implicit failure points during the execution of said call graph).
//

#include <common.h>
#include <openum.h>
#include <mdaassistantsptr.h>
#include <constrainedexecutionregion.h>
#include <ecmakey.h>
#include <typestring.h>
#include <jitinterface.h>



// Internal debugging support. Would be nice to use the common logging code but we've run out of unique facility codes and the debug
// info we spew out is of interest to a limited audience anyhow.
#ifdef _DEBUG

#define CER_NOISY_PREPARE       0x00000001
#define CER_NOISY_RESTORE       0x00000002
#define CER_NOISY_CONTRACTS     0x00000004
#define CER_NOISY_WARNINGS      0x00000008
#define CER_NOISY_NGEN_STATS    0x00000010

DWORD g_dwCerLogActions = 0xffffffff;
DWORD GetCerLoggingOptions()
{
    if (g_dwCerLogActions != 0xffffffff)
        return g_dwCerLogActions;
    return g_dwCerLogActions = EEConfig::GetConfigDWORD(L"CerLogging", 0);
}

#define CER_LOG(_reason, _msg) do { if (GetCerLoggingOptions() & CER_NOISY_##_reason) printf _msg; } while (false)
#else
#define CER_LOG(_reason, _msg)
#endif


// Enumeration used to determine the number of inline data bytes included inside a given IL instruction (except for the case of a
// SWITCH instruction, where a dynamic calculation is required).
enum
{
	ArgBytes_InlineNone             = 0,	// no inline args       
	ArgBytes_InlineVar              = 2,	// local variable       (U2 (U1 if Short on))
	ArgBytes_InlineI                = 4,	// an signed integer    (I4 (I1 if Short on))
	ArgBytes_InlineR                = 8,	// a real number        (R8 (R4 if Short on))
	ArgBytes_InlineBrTarget         = 4,    // branch target        (I4 (I1 if Short on))
	ArgBytes_InlineI8               = 8,
	ArgBytes_InlineMethod           = 4,    // method token (U4)
	ArgBytes_InlineField            = 4,    // field token  (U4)
	ArgBytes_InlineType             = 4,    // type token   (U4)
	ArgBytes_InlineString           = 4,    // string TOKEN (U4)
	ArgBytes_InlineSig              = 4,    // signature tok (U4)
	ArgBytes_InlineRVA              = 4,    // ldptr token  (U4)
	ArgBytes_InlineTok              = 4,    // a meta-data token of unknown type (U4)
	ArgBytes_InlineSwitch           = 4,    // count (U4), pcrel1 (U4) .... pcrelN (U4)
	ArgBytes_ShortInlineVar         = 1,
	ArgBytes_ShortInlineI           = 1,
	ArgBytes_ShortInlineR           = 4,
	ArgBytes_ShortInlineBrTarget    = 1
};

// Build an array of argument byte counts as described above by extracting the 'args' field of each entry in opcode.def.
#define OPDEF(c, s, pop, push, args, type, l, s1, s2, ctrl) ArgBytes_##args,
DWORD g_rOpArgs[] = {
#include <opcode.def>
};
#undef OPDEF


// Various definitions used to parse reliability contracts. These must be kept synchronized with the managed version in
// BCL\System\Runtime\Reliability\ReliabilityContractAttribute.cs

#define RELIABILITY_CONTRACT_NAME       "System.Runtime.ConstrainedExecution.ReliabilityContractAttribute"
#define RC_CONSISTENCY_PROP_NAME        "ConsistencyGuarantee"
#define RC_CER_PROP_NAME                "Cer"

enum {
    RC_CONSISTENCY_CORRUPT_PROCESS      = 0,
    RC_CONSISTENCY_CORRUPT_APPDOMAIN    = 1,
    RC_CONSISTENCY_CORRUPT_INSTANCE     = 2,
    RC_CONSISTENCY_CORRUPT_NOTHING      = 3,
    RC_CONSISTENCY_UNDEFINED            = 4,
    RC_CER_NONE                         = 0,
    RC_CER_MAYFAIL                      = 1,
    RC_CER_SUCCESS                      = 2,
    RC_CER_UNDEFINED                    = 3
};


// We compact the reliability contract states above into a single DWORD format easy to cache at the assembly and class level
// opaquely. We also encode in this DWORD whether a given part of the state has been defined yet (an assembly might set a
// consistency level without specifying a cer level, for instance, and this information is vital when merging states between
// assembly, class and method levels).
// The macros below handle the encoding so nobody else needs to know the details.

// The base state for an encoded DWORD: neither consistency or cer defined.
#define RC_NULL RC_ENCODE(RC_CONSISTENCY_UNDEFINED, RC_CER_UNDEFINED)

// Extract the raw consistency value from an encoded DWORD.
#define RC_CONSISTENCY(_encoded) ((_encoded) >> 2)

// Extract the raw cer value from an encoded DWORD.
#define RC_CER(_encoded) ((_encoded) & 3)

// Produce an encoded DWORD from a pair of raw consistency and cer values. Values must have been range validated first.
#define RC_ENCODE(_consistency, _cer) (DWORD)(((_consistency) << 2) | (_cer))

// Produce an abstracted ReliabilityContractLevel from an encoded DWORD, see CheckForReliabilityContract for details of the rules.
#define RC_ENCODED_TO_LEVEL(_encoded)                                                   \
    ((RC_CONSISTENCY(_encoded) == RC_CONSISTENCY_UNDEFINED ||                           \
      RC_CONSISTENCY(_encoded) < RC_CONSISTENCY_CORRUPT_INSTANCE) ? RCL_NO_CONTRACT :   \
     (RC_CER(_encoded) == RC_CER_UNDEFINED ||                                           \
      RC_CER(_encoded) == RC_CER_NONE) ? RCL_BASIC_CONTRACT :                           \
     RCL_PREPARE_CONTRACT)

// Given two DWORD encodings presumed to come from different scopes (e.g. method and class) merge them to find the effective
// contract state. It's presumed the first encoding is the most tightly scoped (i.e. method would go first in the example above) and
// therefore takes precedence.
#define RC_MERGE(_old, _new) RC_ENCODE((RC_CONSISTENCY(_old) == RC_CONSISTENCY_UNDEFINED) ? \
                                       RC_CONSISTENCY(_new) : RC_CONSISTENCY(_old),         \
                                       (RC_CER(_old) == RC_CER_UNDEFINED) ?                 \
                                       RC_CER(_new) : RC_CER(_old))                         \

// Return true if either consistency or cer has not been specified in the encoded DWORD given.
#define RC_INCOMPLETE(_encoded) (RC_CONSISTENCY(_encoded) == RC_CONSISTENCY_UNDEFINED || RC_CER(_encoded) == RC_CER_UNDEFINED)


// Global cache of methods and their reliability contract state.
PtrHashCache *g_pMethodContractCache = NULL;


// Private method forward references.
bool IsPcrReference(Module *pModule, mdToken tkMethod);
MethodContext *TokenToMethodDesc(Module *pModule, mdToken tokMethod, SigTypeContext *pTypeContext);
TypeHandle GetTypeFromMemberDefOrRefOrSpecThrowing(Module         *pModule,
                                                   mdToken         tokMethod,
                                                   SigTypeContext *pTypeContext);

bool MethodCallGraphPreparer::ShouldGatherExplicitCERCallInfo()
{
    // If we're partially processing a method body (at the top of the call graph), we need to fetch exception handling
    // information to determine possible ranges of interesting IL (potentially each finally and catch clause).
    // 
    // And if we are probing for stack overflow, we need to know if the explicit CER region contains any calls out, in 
    // which case we want to probe in the call to PrepareConstrainedExecutionRegions.  This will ensure that we don't
    // take an SO in boundary code and not be able to call the CER.  When stack probing is disabled, we rip the process
    // if we take an SO anywhere but managed, or if we take an SO with a CER on the stack.  For NGEN images, we need 
    // to always probe because stack probing may be enabled in the runtime, but if we haven't probed in the NGEN image
    // then we could take an SO in boundary code and not be able to crawl the stack to know that we've skipped a CER and
    // need to tear the process.
    //
    // Additionally, if the MDA for illegal PrepareConstrainedRegions call positioning is enabled we gather this information for
    // all methods in the graph.
    return !m_fEntireMethod 
        || g_pConfig->ProbeForStackOverflow();
}

MethodCallGraphPreparer::MethodCallGraphPreparer(MethodDesc *pRootMD, SigTypeContext *pRootTypeContext, bool fEntireMethod, bool fExactTypeContext)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pRootMD));
        PRECONDITION(CheckPointer(pRootTypeContext));
    } CONTRACTL_END;

    // Canonicalize value type unboxing stubs into their underlying method desc.
    if (pRootMD->IsUnboxingStub())
        pRootMD = pRootMD->GetWrappedMethodDesc();

    m_pRootMD = pRootMD;
    m_pRootTypeContext = pRootTypeContext;
    m_fEntireMethod = fEntireMethod;
    m_fExactTypeContext = fExactTypeContext;
    
    m_pEHClauses = NULL;
    m_cEHClauses = 0;
    m_pCerPrepInfo = NULL;  
    m_pMethodDecoder = NULL;


    m_pThread = GetThread(); 
    m_fPartialPreparation = false;
    m_fMethodHasCallsWithinExplicitCer = false;
}

// Walk the call graph of the method given by pRootMD (and type context in pRootTypeContext which provides instantiation information
// for generic methods/classes).
//
// If fEntireMethod is true then the entire body of pRootMD is scanned for callsites, otherwise we assume that one or more CER
// exception handlers exist in the method and only the finally and catch blocks of such handlers are scanned for graph roots.
//
// Each method we come across in the call graph (excluding late bound invocation destinations precipitated by virtual or interface
// calls) is jitted and has any generic dictionary information we can determine at jit time prepopulated. This includes implicit
// cctor invocations. If this method is called at ngen time we will attach extra fixup information to the affected method to ensure
// that fixing up the root method of the graph will cause all methods in the graph to be fixed up at that point also.
//
// Some generic dictionary entries may not be prepopulated if unbound type variables exist at the root of the call tree. Such cases
// will be ignored (as for the virtual/interface dispatch case we assume the caller will use an out-of-band mechanism to pre-prepare
// these entries explicitly).
bool MethodCallGraphPreparer::Run()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Avoid recursion while jitting methods for another preparation.
    if (m_pThread->IsPreparingCer())
        return TRUE;   // Assume the worst

    {
        // The non-ngen case (normal jit, call to PrepareMethod etc).
        m_pCerPrepInfo = m_pRootMD->GetModule()->GetCerPrepInfo(m_pRootMD);
        if (m_pCerPrepInfo) {

            // Check for the "everything's done" case.
            if (m_pCerPrepInfo->m_fFullyPrepared)
                return m_pCerPrepInfo->m_fMethodHasCallsWithinExplicitCer;

            // Check for the "we can't do anything" case (see below for descriptions of that).
            if (m_pCerPrepInfo->m_fRequiresInstantiation && !m_fExactTypeContext)
                return m_pCerPrepInfo->m_fMethodHasCallsWithinExplicitCer;

            // Check for the "need to prepare per-instantiation, but we've already done this one" case.
            if (m_pCerPrepInfo->m_fRequiresInstantiation) {
                HashDatum sDatum;
                if (m_pCerPrepInfo->m_sIsInitAtInstHash.GetValue(m_pRootTypeContext, &sDatum))
                    return m_pCerPrepInfo->m_fMethodHasCallsWithinExplicitCer;
            }
        }
    }

    // We can't deal with generic methods or methods on generic types that may have some representative type parameters in their
    // instantiation (i.e. some reference types indicated by Object rather than the exact type). The jit will tend to pass us these
    // since it shares code between instantiations over reference types. We can't prepare methods like these completely -- even
    // though we can jit all the method bodies the code might require generic dictionary information at the class or method level
    // that is populated at runtime and can introduce failure points. So we reject such methods immediately (they will need to be
    // prepared at non-jit time by an explicit call to PrepareMethod with a fully instantiated method).
    //
    // In the case where the type context is marked as suspect (m_fExactTypeContext == false) there are a number of possibilites for
    // bad methods the jit will pass us:
    //  1) We're passed a MethodDesc that shared between instantiations (bogus because exact method descs are never shared).
    //  2) We're passed a MethodDesc that's an instantiating stub (bogus because non-shared methods don't need this).
    //  3) We're passed a MethodDesc that has generic variables in its instantiations (I've seen this during ngen).
    //
    // Technically we could do a little better than this -- we could determine whether any of the representative type parameters are
    // actually used within the CER call graph itself. But this would require us to understand the IL at a much deeper level (i.e.
    // parse every instruction that could take a type or member spec and pull apart those specs to see if a type var is used). Plus
    // we couldn't make this determination until we've prepared the entire region and the result is rather brittle from the code
    // author's point of view (i.e. we might prepare a CER automatically one day but stop doing after some relatively subtle changes
    // in the source code).
    m_fPartialPreparation = m_pRootMD->IsSharedByGenericInstantiations() || m_pRootMD->IsInstantiatingStub() || m_pRootMD->ContainsGenericVariables();
    if (!m_fExactTypeContext && m_fPartialPreparation) {
        CER_LOG(WARNINGS, ("CER: %s has open type parameters and can't be pre-prepared\n", m_pRootMD->m_pszDebugMethodName));


        {
            // Set up a prep info structure for this method if it's not there already (the create method takes care of races).
            if (m_pCerPrepInfo == NULL)
                m_pCerPrepInfo = m_pRootMD->GetModule()->CreateCerPrepInfo(m_pRootMD);

            // We may be racing to update the structure at this point but that's OK since the flag we're setting is never cleared once
            // it's set and is always guaranteed to be set before we rely on its value (setting it here is just a performance thing,
            // letting us early-out on multiple attempts to prepare this CER from the jit).
            m_pCerPrepInfo->m_fRequiresInstantiation = true;
        }

        if (! g_pConfig->ProbeForStackOverflow())
        {
            return FALSE;
        }
        m_pCerPrepInfo = m_pRootMD->GetModule()->GetCerPrepInfo(m_pRootMD);
        return (!m_pCerPrepInfo || m_pCerPrepInfo->m_fMethodHasCallsWithinExplicitCer);
        
    }


    // Prevent inlining of the root method (otherwise it's hard to tell where ThreadAbort exceptions should be delayed). Note that
    // MethodDesc::SetNotInline is thread safe.
    m_pRootMD->SetNotInline(true);

    // Remember the checkpoint for all of our allocations. Keep it in a holder so they'll be unwound if we throw an exception past
    // here.
    CheckPointHolder sCheckpoint(m_pThread->m_MarshalAlloc.GetCheckpoint());

    // Push the current method as the one and only method to process so far.
    m_sLeftToProcess.Push(MethodContext::PerThreadAllocate(m_pRootMD, m_pRootTypeContext));

    MethodContext      *pContext = NULL;               // The current MethodContext we're processing
    
    // Iterate until we run out of methods to process.
    while ((pContext = m_sLeftToProcess.Pop()) != NULL) {

        // Restore the MD if necessary. In particular, if this is an instantiating stub and the wrapped MethodDesc could
        // not be hard bound, then we'll need to restore that pointer before getting it.
        pContext->m_pMethodDesc->CheckRestore();

        // Transfer the method to the already seen stack immediately (we don't want to loop infinitely in the case of method
        // recursion).
        m_sAlreadySeen.Push(pContext);

        // Check if the enclosing class requires a static class constructor to be run. If so, we need to prepare that method as
        // though it were any other call.
        if (pContext->m_pMethodDesc->GetMethodTable()->HasClassConstructor()) {

            // Decode target method into MethodDesc and new SigTypeContext.
            // The type context is easy to derive here : .cctors never have any method type parameters and the class instantiations
            // are those of the method we're currently parsing, so can be simply copied down.
            MethodDesc *pCctor = pContext->m_pMethodDesc->GetCanonicalMethodTable()->GetClassConstructor();
            SigTypeContext sCctorTypeContext(pCctor, pContext->m_sTypeContext.m_classInst, NULL);
            MethodContext *pCctorContext = MethodContext::PerThreadAllocate(pCctor, &sCctorTypeContext);

            // Only process this cctor the first time we find it in this call graph.
            if (!m_sAlreadySeen.IsInStack(pCctorContext) && !m_sLeftToProcess.IsInStack(pCctorContext))
                m_sLeftToProcess.Push(pCctorContext);
        }

        // Skip further processing if this method doesn't have an IL body (note that we assume the method we entered with was IL, so
        // we don't need to bother with partial method processing).
        if (!pContext->m_pMethodDesc->IsIL()) {
            _ASSERTE(m_fEntireMethod);
            continue;
        }

        // Locate the IL body of the current method. May have to account for the fact that the current method desc is an
        // instantiating stub and burrow down for the real method desc.
        MethodDesc *pRealMethod = pContext->m_pMethodDesc;
        if (pRealMethod->IsInstantiatingStub()) {
            _ASSERTE(!pRealMethod->ContainsGenericVariables());
            pRealMethod = ((InstantiatedMethodDesc*)pRealMethod)->GetWrappedMethodDesc();
        }

        COR_ILMETHOD_DECODER method(pRealMethod->GetILHeader());
        m_pMethodDecoder = &method;

        // We want to reget the EH clauses for the current method so that we can scan its handlers
        GetEHClauses();

        LookForInterestingCallsites(pContext);

        // Whatever we've done, we're definitely not processing the top-level method at this point (so we'll be processing full
        // method bodies from now on).
        m_fEntireMethod = true;
    }

    {
        // Set up a prep info structure for this method if it's not there already (the create method takes care of races).
        // This needs to happen before we start JITing the methods as part of preparation. The JIT needs to know
        // about the CER root in CEEInfo::canTailCall.
        if (m_pCerPrepInfo == NULL)
            m_pCerPrepInfo = m_pRootMD->GetModule()->CreateCerPrepInfo(m_pRootMD);
    }

    // Once we get here we've run out of methods to process and have recorded each method we visited in the m_sAlreadySeen stack. Now
    // it's time to prepare each of these methods (jit, prepopulate generic dictionaries etc.).
    PrepareMethods();

    return RecordResults();
}


void MethodCallGraphPreparer::GetEHClauses()
{
     CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (! ShouldGatherExplicitCERCallInfo())
    {
        return;
    }

    m_cEHClauses = 0;
    m_pEHClauses = NULL;    // we use the StackingAllocator, so don't have to delete the previous storage
    
    COR_ILMETHOD_SECT_EH const * pEH = m_pMethodDecoder->EH;
    if (pEH == NULL ||pEH->EHCount() == 0) 
    {
        return;
    }

    m_cEHClauses = pEH->EHCount();
    m_pEHClauses = (EHClauseRange*)m_pThread->m_MarshalAlloc.Alloc(sizeof(EHClauseRange) * m_cEHClauses);

    for (DWORD i = 0; i < m_cEHClauses; i++) 
    {
        COR_ILMETHOD_SECT_EH_CLAUSE_FAT         sEHClauseBuffer;
        const COR_ILMETHOD_SECT_EH_CLAUSE_FAT   *pEHClause;

        pEHClause = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)pEH->EHClause(i, &sEHClauseBuffer);

        // The algorithm below assumes handlers are located after their associated try blocks. If this turns out to be a
        // false assumption we need to move to a two pass technique (or defer callsite handling in some other fashion until
        // we've scanned the IL for all calls to our preparation marker method).
        if (!(pEHClause->GetTryOffset() < pEHClause->GetHandlerOffset()))
        {
            COMPlusThrowHR(COR_E_NOTSUPPORTED, IDS_EE_NOTSUPPORTED_CATCHBEFORETRY);
        }

        m_pEHClauses[i].m_dwTryOffset = pEHClause->GetTryOffset();
        m_pEHClauses[i].m_dwHandlerOffset = pEHClause->GetHandlerOffset();
        m_pEHClauses[i].m_dwHandlerLength = pEHClause->GetHandlerLength();
        m_pEHClauses[i].m_fActive = false;

        //printf("Try: %u Handler: %u -> %u\n", pEHClause->GetTryOffset(), pEHClause->GetHandlerOffset(), pEHClause->GetHandlerOffset() + pEHClause->GetHandlerLength() - 1);
    }
 
}

void MethodCallGraphPreparer::MarkEHClauseActivatedByCERCall(MethodContext *pContext, BYTE *pbIL, DWORD cbIL)
{
     CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    DWORD   dwOffset = (DWORD)(SIZE_T)((pbIL + ArgBytes_InlineTok) - (BYTE*)m_pMethodDecoder->Code);

    // Additionally we need to cope with the fact that VB and C# (for debug builds) can generate NOP instructions
    // between the PCR call and the beginning of the try block. So we're potentially looking for the
    // intersection of the try with a range of instructions. Count the number of consecutive NOP instructions
    // which follow the call.
    DWORD   dwLength = 0;
    BYTE   *pbTmpIL = pbIL + ArgBytes_InlineTok;
    while (pbTmpIL < (pbIL + cbIL) && *pbTmpIL++ == CEE_NOP)
    {
        dwLength++;
    }

    bool    fMatched = false;
    for (DWORD i = 0; i < m_cEHClauses; i++)
    {
        if (m_pEHClauses[i].m_dwTryOffset >= dwOffset &&
            m_pEHClauses[i].m_dwTryOffset <= (dwOffset + dwLength)) 
        {
            fMatched = true;
            m_pEHClauses[i].m_fActive = true;
        }
    }
    if (!fMatched) 
    {
#if defined(_DEBUG) || defined(MDA_SUPPORTED)
        DWORD dwPCROffset = (DWORD)(SIZE_T)((pbIL - 1) - (BYTE*)m_pMethodDecoder->Code);
#endif // defined(_DEBUG) || defined(MDA_SUPPORTED)
        CER_LOG(WARNINGS, ("CER: %s: Invalid call to PrepareConstrainedRegions() at IL +%04X\n",
                           pContext->m_pMethodDesc->m_pszDebugMethodName, dwPCROffset));
    }
}

bool MethodCallGraphPreparer::CheckIfCallsiteWithinCER(DWORD dwOffset)
{
     CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    //printf("Found: %s at %u\n", pCallTarget->m_pMethodDesc->m_pszDebugMethodName, dwOffset);

    // Search all the EH regions we know about.
    for (DWORD i = 0; i < m_cEHClauses; i++) 
    {
        bool fCallsiteWithinCER = false;
        if (! m_pEHClauses[i].m_fActive) 
        {
            // clause not CER-active so skip it
            continue;
        }
        if (dwOffset >= (m_pEHClauses[i].m_dwHandlerOffset + m_pEHClauses[i].m_dwHandlerLength)) 
        {
            // offset beyond clause, so skip it
            continue;
        }
        if (dwOffset >= m_pEHClauses[i].m_dwTryOffset)
        {   
            // For stack probing optimization, we care if either the try or the handler has a call.  If neither
            // does, then we can optimize the probe out.
            m_fMethodHasCallsWithinExplicitCer = true;
            if (dwOffset >= m_pEHClauses[i].m_dwHandlerOffset)
            {
                fCallsiteWithinCER = true;
            }
        }
        // Only terminate if we got a positive result (i.e. the calliste is within a hardened clause).
        // We can't terminate early in the negative case because the callsite could be nested
        // in another EH region which may be hardened.
        if (fCallsiteWithinCER == true)
        {
            return true;
        }
    }
    
    return false;
}


// Iterate through the body of the method looking for interesting call sites.
void MethodCallGraphPreparer::LookForInterestingCallsites(MethodContext *pContext)
{
     CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    BYTE *pbIL = (BYTE*)m_pMethodDecoder->Code;
    DWORD cbIL = m_pMethodDecoder->GetCodeSize();

    while (cbIL) {

        // Read the IL op.
        DWORD dwOp = *pbIL++; cbIL--;

        // Handle prefix codes (only CEE_PREFIX1 is legal so far).
        if (dwOp == CEE_PREFIX1) {
            dwOp = 256 + *pbIL++; cbIL--;
        } else if (dwOp >= CEE_PREFIX7)
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

        // We're interested in NEWOBJ, JMP, CALL and CALLVIRT (can't trace through CALLI). We include CALLVIRT becase C#
        // routinely calls non-virtual instance methods this way in order to get this pointer null checking. We prepare NEWOBJ
        // because that covers the corner case of value types which can be constructed with no failure path.
        if (dwOp == CEE_CALL || dwOp == CEE_CALLVIRT || dwOp == CEE_NEWOBJ || dwOp == CEE_JMP) {

            // Decode target method into MethodDesc and new SigTypeContext.
            mdToken tkCallTarget = (mdToken)GET_UNALIGNED_VAL32(pbIL);
            MethodContext *pCallTarget = TokenToMethodDesc(pContext->m_pMethodDesc->GetModule(), tkCallTarget, &pContext->m_sTypeContext);

            // Check whether we've found a call to our own preparation marker method.
            if (pCallTarget->m_pMethodDesc == g_pPrepareConstrainedRegionsMethod) {

                if (ShouldGatherExplicitCERCallInfo()) {
                    // If we're preparing a partial method these callsites are significant (we mark which EH clauses are now
                    // 'activated' by proximity to this marker method call). Look for EH regions that are 'activated' by the call to
                    // PrepareConstrainedRegions by comparing the IL offset of the start of the try to the offset immediately after
                    // the callsite (remember to account for the rest of the CALLVIRT instruction we haven't skipped yet).
                    MarkEHClauseActivatedByCERCall(pContext, pbIL, cbIL);
                }

                // Record the fact that we found a method in the CER which is the root of a sub-CER. This is important for the
                // ngen case as we'll see below.
                pContext->m_fRoot = true;
            }

            // Determine if this was really a virtual call (we discard those since we can't reliably determine the call target).
            bool fNonVirtualCall = dwOp == CEE_CALL || !pCallTarget->m_pMethodDesc->IsVirtual() || pCallTarget->m_pMethodDesc->IsFinal();

            // When we're only processing qualified catch / finally handlers then we need to compute whether this call site
            // lands in one of them.  The callsite is always within a cer if we are processing the full method.
            // If we have stackoverflow probing on, also call to determine if the CER try or finally makes any calls
            bool fCallsiteWithinCerInThisFunction = false;
            if (!m_fEntireMethod || g_pConfig->ProbeForStackOverflow()) {
                DWORD dwOffset = (DWORD)(SIZE_T)((pbIL - 1) - (BYTE*)m_pMethodDecoder->Code);
                fCallsiteWithinCerInThisFunction = CheckIfCallsiteWithinCER(dwOffset);
            }
            bool fCallsiteWithinCer = m_fEntireMethod || fCallsiteWithinCerInThisFunction;

            // Check for the presence of some sort of reliability contract (on the method, class or assembly). This will
            // determine whether we log an error, ignore the method or treat it as part of the prepared call graph.
            ReliabilityContractLevel eLevel = RCL_UNKNOWN;
            if (fNonVirtualCall &&                          // Ignore virtual calls
                fCallsiteWithinCer &&                       // And calls outside CERs
                !m_sAlreadySeen.IsInStack(pCallTarget) &&     // And methods we've seen before
                !m_sLeftToProcess.IsInStack(pCallTarget) &&   // And methods we've already queued for processing
                (eLevel = CheckForReliabilityContract(pCallTarget->m_pMethodDesc)) >= RCL_PREPARE_CONTRACT) // And unreliable methods
                m_sLeftToProcess.Push(pCallTarget);           // Otherwise add this method to the list to process
            else if (fCallsiteWithinCer) { 
#if defined(_DEBUG) || defined(MDA_SUPPORTED)
                DWORD dwOffset = (DWORD)(SIZE_T)((pbIL - 1) - (BYTE*)m_pMethodDecoder->Code);
#endif // defined(_DEBUG) || defined(MDA_SUPPORTED)
                if (eLevel == RCL_NO_CONTRACT) {
                    // Method was sufficiently unreliable for us to warn interested users that something may be amiss. Do this
                    // through MDA logging.
                    CER_LOG(WARNINGS, ("CER: %s +%04X -> %s: weak contract\n", pContext->ToString(), dwOffset, pCallTarget->ToString()));
                } else if (!fNonVirtualCall) {
                    CER_LOG(WARNINGS, ("CER: %s +%04X -> %s: virtual call\n", pContext->ToString(), dwOffset, pCallTarget->ToString()));
                }
            }
        }

        // Skip the rest of the current IL instruction. Look up the table built statically at the top of this module for most
        // instructions, but CEE_SWITCH requires special processing (the length of that instruction depends on a count DWORD
        // embedded right after the opcode).
        if (dwOp == CEE_SWITCH) {
            DWORD dwTargets = GET_UNALIGNED_VAL32(pbIL);
            pbIL += 4 + (dwTargets * 4);
            cbIL -= 4 + (dwTargets * 4);
        } else {
            PREFIX_ASSUME(dwOp < _countof(g_rOpArgs));
            pbIL += g_rOpArgs[dwOp];
            cbIL -= g_rOpArgs[dwOp];
        }

    } // End of IL parsing loop
}

void MethodCallGraphPreparer::PrepareMethods()
{
     CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

#ifdef _DEBUG
    DWORD dwCount = 0;
    CER_LOG(PREPARE, ("---------------------------------------------------------------\n"));
    SString ssMethod;
    TypeString::AppendMethodInternal(ssMethod, m_pRootMD, TypeString::FormatNamespace | TypeString::FormatStubInfo);
    CER_LOG(PREPARE, ("Preparing from %S\n", ssMethod.GetUnicode()));
#endif

    ThreadPreparingCerHolder sCerHolder;
    MethodContext      *pContext;               // The current MethodContext we're processing

    while ((pContext = m_sAlreadySeen.Pop()) != NULL) {
        MethodDesc *pMD = pContext->m_pMethodDesc;

        // Jitting. Don't need to do this for the ngen case.
        {
            // Also skip the jit for the root method in the activated from jit case (where this would result in a recursive
            // jit). We'd cope with this just fine, the main reason for this logic is to avoid unbalancing some profiler event
            // counts that upset some of our test cases. This is safe in the face of multiple instantiations of the same method
            // because in the jit activated case (where we're told the root type context is not exact) we early exit if the root
            // method desc isn't 'unique' (i.e. independent of the type context).
            if (m_fExactTypeContext || pMD != m_pRootMD) {

                // Jit the method we traced.
                if (!pMD->IsCodeReady())
                {
                    pMD->EnsureActive();
                    pMD->DoPrestub(NULL);
                }

                // If we traced an instantiating stub we need to jit the wrapped (real) method as well.
                if (pMD->IsInstantiatingStub()) {
                    _ASSERTE(!pMD->ContainsGenericVariables());
                    MethodDesc *pRealMD = ((InstantiatedMethodDesc*)pMD)->GetWrappedMethodDesc();
                    if (!pRealMD->IsCodeReady())
                    {
                        pMD->EnsureActive();                        
                        pRealMD->DoPrestub(NULL);
                    }
                }
            }
        }

        // Prepare generic dictionaries (both class and method as needed). We do this even in the ngen scenario, trying to get
        // as many slots filled as possible. By the looks of it, it's possible that not all of these entries will make it across
        // to runtime (the fixup code seems to give up on some of the more complex entries, not sure of the details). But we'll
        // do as best we can here to hopefully minimize any real work on the other side.

        // Don't use the direct PrepopulateDictionary method on MethodTable here, it takes binding considerations into account
        // (which we don't care about).
        DictionaryLayout *pClassDictLayout = pMD->GetClass()->GetDictionaryLayout();
        if (pClassDictLayout) {
            // Translate the representative method table we can find from our method desc into an exact instantiation using the
            // type context we have.
            MethodTable *pMT = TypeHandle(pMD->GetMethodTable()).Instantiate(pContext->m_sTypeContext.m_classInst,
                                                                             pContext->m_sTypeContext.m_classInstCount).AsMethodTable();

            pMT->GetDictionary()->PrepopulateDictionary(pMT->GetModule(),
                                                        &pContext->m_sTypeContext,
                                                        pMT->GetNumGenericArgs(),
                                                        pClassDictLayout,
                                                        pMT->GetDomain(),
                                                        false);

            // The dictionary may have overflowed in which case we need to prepopulate the jit's lookup cache as well.
            PrepopulateGenericHandleCache(pClassDictLayout, pMD, pMT);
        }

        // Don't use the direct PrepopulateDictionary method on MethodDesc here, it appears to use a representative class
        // instantiation (and we have the exact one handy).
        DictionaryLayout *pMethDictLayout = pMD->GetDictionaryLayout();
        if (pMethDictLayout) {
            pMD->GetMethodDictionary()->PrepopulateDictionary(pMD->GetModule(),
                                                              &pContext->m_sTypeContext,
                                                              pMD->GetNumGenericMethodArgs(),
                                                              pMethDictLayout,
                                                              pMD->GetDomain(),
                                                              false);

            // The dictionary may have overflowed in which case we need to prepopulate the jit's lookup cache as well.
            PrepopulateGenericHandleCache(pMethDictLayout, pMD, NULL);
        }


#ifdef _DEBUG
        CER_LOG(PREPARE, ("  %s\n", pContext->ToString()));
        dwCount++;
#endif
    }

#ifdef _DEBUG
    CER_LOG(PREPARE, ("Prepared a total of %u methods\n", dwCount));
    CER_LOG(PREPARE, ("---------------------------------------------------------------\n"));
#endif
}

bool  MethodCallGraphPreparer::RecordResults()
{
     CONTRACTL {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Preparation has been successful, record what we've done in a manner consistent with whether we're ngen'ing or running for
    // real.

    // This is the runtime (non-ngen case). Record our progress in an info structure placed in a hash table hung off the module
    // which owns the root method desc in the CER. The methods which create this info structure handle race conditions (to
    // ensure we don't leak memory or data), but the updates to the info structure itself might not require any serialization
    // (the values are 'latched' -- recomputation should yield the same result). The exception is any update to a more complex
    // data fields (lists and hash tables) that require serialization to prevent corruption of the basic data structure.

    if (m_pCerPrepInfo == NULL)
        m_pCerPrepInfo = m_pRootMD->GetModule()->CreateCerPrepInfo(m_pRootMD);

    m_pCerPrepInfo->m_fMethodHasCallsWithinExplicitCer = m_fMethodHasCallsWithinExplicitCer;

    // Simple case first: if this isn't a partial preparation (no pesky unbound type vars to worry about), then the method is
    // now fully prepared.
    if (!m_fPartialPreparation) {
        m_pCerPrepInfo->m_fFullyPrepared = true;
        return m_fMethodHasCallsWithinExplicitCer;
    }

    // Else we know we require per-instantiation initialization. We need to update a hash table to record the preparation we did
    // in this case, and that requires taking a mutex. We could check that nobody beat us to it first, but that will hardly ever
    // happen, so it's not really worth it. So just acquire the mutex right away.
    CrstHolder sHolder(m_pRootMD->GetModule()->GetCerCrst());

    m_pCerPrepInfo->m_fRequiresInstantiation = true;

    // Add an entry to a hash that records which instantiations we've prep'd for (again, only if someone hasn't beaten us).
    HashDatum sDatum;
    if (!m_pCerPrepInfo->m_sIsInitAtInstHash.GetValue(m_pRootTypeContext, &sDatum))
    {
        m_pCerPrepInfo->m_sIsInitAtInstHash.InsertKeyAsValue(m_pRootTypeContext);
    }

    return m_fMethodHasCallsWithinExplicitCer;
}


// Determines whether the given method contains a CER root that can be pre-prepared (i.e. prepared at jit time).
bool ContainsPrePreparableCerRoot(MethodDesc *pMD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMD));
    } CONTRACTL_END;

    // Deal with exotic cases (non-IL methods and the like).
    if (!pMD->IsIL() || pMD->IsAbstract())
        return false;

    // And cases where we can't jit prepare (because the code is shared between instantiations).
    if (pMD->IsSharedByGenericInstantiations() || pMD->IsInstantiatingStub() || pMD->ContainsGenericVariables())
        return false;

    // If we've already jitted the method we recorded enough information to determine the answer already.
    if (pMD->IsPointingToNativeCode() && pMD->IsCodeReady())
        return IsCerRootMethod(pMD);

    // Otherwise we have a trickier calculation. We don't want to force the jit of the method at this point (may cause infinite
    // recursion problems when we're called from the jit in the presence of call cycles). Instead we walk the top-level of the
    // method IL using the same algorithm as PrepareMethodCallGraph.

    // Locate the IL body of the current method. May have to account for the fact that the current method desc is an
    // instantiating stub and burrow down for the real method desc.
    MethodDesc *pRealMethod = pMD;
    if (pRealMethod->IsInstantiatingStub()) {
        _ASSERTE(!pRealMethod->ContainsGenericVariables());
        pRealMethod = ((InstantiatedMethodDesc*)pRealMethod)->GetWrappedMethodDesc();
    }
    COR_ILMETHOD_DECODER method(pRealMethod->GetILHeader());
    BYTE *pbIL = (BYTE*)method.Code;
    DWORD cbIL = method.GetCodeSize();

    // Look for exception handling information for the method. If there isn't any then we know there can't be a CER rooted here.
    COR_ILMETHOD_SECT_EH const * pEH = method.EH;
    if (pEH == NULL || pEH->EHCount() == 0)
        return false;

    // Iterate through the body of the method looking for interesting call sites.
    while (cbIL) {

        // Read the IL op.
        DWORD dwOp = *pbIL++; cbIL--;

        // Handle prefix codes (only CEE_PREFIX1 is legal so far).
        if (dwOp == CEE_PREFIX1) {
            dwOp = 256 + *pbIL++; cbIL--;
        } else if (dwOp >= CEE_PREFIX7)
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

        // We'll only ever see CALL instructions targetting PrepareConstrainedRegions (well those are the ones we're interested in
        // anyway).
        if (dwOp == CEE_CALL && IsPcrReference(pMD->GetModule(), (mdToken)GET_UNALIGNED_VAL32(pbIL)))
            return true;

        // Skip the rest of the current IL instruction. Look up the table built statically at the top of this module for most
        // instructions, but CEE_SWITCH requires special processing (the length of that instruction depends on a count DWORD
        // embedded right after the opcode).
        if (dwOp == CEE_SWITCH) {
            DWORD dwTargets = GET_UNALIGNED_VAL32(pbIL);
            pbIL += 4 + (dwTargets * 4);
            cbIL -= 4 + (dwTargets * 4);
        } else {
            PREFIX_ASSUME(dwOp < _countof(g_rOpArgs));
            pbIL += g_rOpArgs[dwOp];
            cbIL -= g_rOpArgs[dwOp];
        }

    } // End of IL parsing loop

    // If we get here then there was no CER-root.
    return false;
}

#define PCR_METHOD      "PrepareConstrainedRegions"
#define PCR_TYPE        "RuntimeHelpers"
#define PCR_NAMESPACE   "System.Runtime.CompilerServices"

// Given a token and a module scoping it, determine if that token is a reference to PrepareConstrainedRegions. We want to do this
// without loading any random types since we're called in a context where type loading is prohibited.
bool IsPcrReference(Module *pModule, mdToken tkMethod)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pModule));
    } CONTRACTL_END;

    IMDInternalImport *pImport = pModule->GetMDImport();

    // Validate that the token is one that we can handle.
    if (!pImport->IsValidToken(tkMethod) || (TypeFromToken(tkMethod) != mdtMethodDef &&
                                             TypeFromToken(tkMethod) != mdtMethodSpec &&
                                             TypeFromToken(tkMethod) != mdtMemberRef))
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_METHOD_TOKEN);

    // No reason to see a method spec for a call to something as simple as PrepareConstrainedRegions.
    if (TypeFromToken(tkMethod) == mdtMethodSpec)
        return false;

    // If it's a method def then the module had better be mscorlib.
    if (TypeFromToken(tkMethod) == mdtMethodDef) {
        if (pModule->GetAssembly()->GetManifestModule() == SystemDomain::SystemAssembly()->GetManifestModule())
            return tkMethod == g_pPrepareConstrainedRegionsMethod->GetMemberDef();
        else
            return false;
    }

    // That leaves the cross module reference case.
    _ASSERTE(TypeFromToken(tkMethod) == mdtMemberRef);

    // First get the method name and signature and validate it.
    PCCOR_SIGNATURE pSig;
    DWORD cbSig;
    LPCSTR szMethod = pImport->GetNameAndSigOfMemberRef(tkMethod, &pSig, &cbSig);

    // Signature is easy: void PCR().
    // Must be a static method signature.
    if (CorSigUncompressCallingConv(pSig) != IMAGE_CEE_CS_CALLCONV_DEFAULT)
        return false;
    // With no arguments.
    if (CorSigUncompressData(pSig) != 0)
        return false;
    // And a void return type.
    if (*pSig != (BYTE)ELEMENT_TYPE_VOID)
        return false;
    _ASSERTE(cbSig == 3);

    // Validate the name.
    if (strcmp(szMethod, PCR_METHOD) != 0)
        return false;

    // The method looks OK, move up to the type and validate that.
    mdToken tkType = pImport->GetParentOfMemberRef(tkMethod);
    if (!pImport->IsValidToken(tkType))
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN);

    // If the parent is not a type ref then this isn't our target (we assume that mscorlib never uses a member ref to target
    // PrepareConstrainedRegions, check that with the assert below, if it ever fails we need to add some additional logic below).
    _ASSERTE(TypeFromToken(tkType) != mdtTypeDef ||
             pModule->GetAssembly()->GetManifestModule() != SystemDomain::SystemAssembly()->GetManifestModule());
    if (TypeFromToken(tkType) != mdtTypeRef)
        return false;

    // Get the type name and validate it.
    LPCSTR szNamespace;
    LPCSTR szType;
    pImport->GetNameOfTypeRef(tkType, &szNamespace, &szType);
    if (strcmp(szType, PCR_TYPE) != 0)
        return false;
    if (strcmp(szNamespace, PCR_NAMESPACE) != 0)
        return false;

    // Type is OK as well. Check the assembly reference.
    mdToken tkScope = pImport->GetResolutionScopeOfTypeRef(tkType);
    if (TypeFromToken(tkScope) != mdtAssemblyRef)
        return false;
    if (!pImport->IsValidToken(tkScope))
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN);

    // Fetch the name and public key or public key token.
    BYTE *pbPublicKeyOrToken;
    DWORD cbPublicKeyOrToken;
    LPCSTR szAssembly;
    DWORD dwAssemblyFlags;
    pImport->GetAssemblyRefProps(tkScope,
                                 (const void**)&pbPublicKeyOrToken,
                                 &cbPublicKeyOrToken,
                                 &szAssembly,
                                 NULL, // AssemblyMetaDataInternal: we don't care about version, culture etc.
                                 NULL, // Hash value pointer, obsolete information
                                 NULL, // Byte count for above
                                 &dwAssemblyFlags);

    // Validate the name.
    if (stricmpUTF8(szAssembly, g_psBaseLibraryName) != 0)
        return false;

    // And the public key or token, which ever was burned into the reference by the compiler. For mscorlib this is the ECMA key or
    // token.
    if (IsAfPublicKeyToken(dwAssemblyFlags)) {
        if (cbPublicKeyOrToken != sizeof(g_rbNeutralPublicKeyToken) ||
            memcmp(pbPublicKeyOrToken, g_rbNeutralPublicKeyToken, cbPublicKeyOrToken) != 0)
            return false;
    } else {
        if (cbPublicKeyOrToken != sizeof(g_rbNeutralPublicKey) ||
            memcmp(pbPublicKeyOrToken, g_rbNeutralPublicKey, cbPublicKeyOrToken) != 0)
            return false;
    }

    // If we get here we've finally proved the call target was indeed PrepareConstrainedRegions. Whew.
    return true;
}

// Macro used to prepare a (possibly virtual) method on an instance. The method is identified via a binder ID, so the initial
// declaration of the method must reside within mscorlib. We might have ngen fixup information for the method and can avoid direct
// preparation as well.

#define CER_NGEN_PREP(__pPrepMethod)

#define CER_PREPARE_METHOD(_pMT, _methodId, _pModule) do                                                        \
{                                                                                                               \
    MethodDesc *_pPrepMethod = _pMT->GetMethodDescForSlot(g_Mscorlib.GetMethod(_methodId)->GetSlot());          \
    CER_NGEN_PREP(_pPrepMethod)                                                                                 \
    {                                                                                                           \
        if (_pModule == NULL || _pPrepMethod->GetModule() == _pModule) {                                        \
            SigTypeContext _sTypeContext(_pPrepMethod, TypeHandle(_pMT));                                       \
            MethodCallGraphPreparer mcgp(_pPrepMethod, &_sTypeContext, true, true);                                   \
            mcgp.Run(); \
        }                                                                                                       \
    }                                                                                                           \
} while (false)

// Prepares the critical finalizer call graph for the given object type (which must derive from CriticalFinalizerObject). This
// involves preparing at least the finalizer method and possibly some others (for SafeHandle and CriticalHandle derivations). If a
// module pointer is supplied then only the critical methods introduced in that module are prepared (this is used at ngen time to
// ensure that we're only generating ngen preparation info for the targetted module).
void PrepareCriticalFinalizerObject(MethodTable *pMT, Module *pModule)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMT));
    } CONTRACTL_END;

    // Have we prepared this type before?
    if (pMT->CriticalTypeHasBeenPrepared())
        return;

    // Restore the method table if necessary.
    pMT->CheckRestore();

    // Determine the interesting parent class (either SafeHandle, CriticalHandle or CriticalFinalizerObject).
    MethodTable *pSafeHandleClass = g_Mscorlib.FetchClass(CLASS__SAFE_HANDLE);
    MethodTable *pCriticalHandleClass = g_Mscorlib.FetchClass(CLASS__CRITICAL_HANDLE);
    MethodTable *pParent = pMT->GetParentMethodTable();
    while (pParent) {
        if (pParent == g_pCriticalFinalizerObjectClass ||
            pParent == pSafeHandleClass ||
            pParent == pCriticalHandleClass) {
            break;
        }
        pParent = pParent->GetParentMethodTable();
    }
    _ASSERTE(pParent != NULL);

    // Prepare the method or methods based on the parent class.
    if (pParent == pSafeHandleClass) {
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_FINALIZER_OBJECT__FINALIZE, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__SAFE_HANDLE__RELEASE_HANDLE, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__SAFE_HANDLE__GET_IS_INVALID, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__SAFE_HANDLE__DISPOSE, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__SAFE_HANDLE__DISPOSE_BOOL, pModule);
    } else if (pParent == pCriticalHandleClass) {
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_FINALIZER_OBJECT__FINALIZE, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_HANDLE__RELEASE_HANDLE, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_HANDLE__GET_IS_INVALID, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_HANDLE__DISPOSE, pModule);
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_HANDLE__DISPOSE_BOOL, pModule);
    } else {
        _ASSERTE(pParent == g_pCriticalFinalizerObjectClass);
        CER_PREPARE_METHOD(pMT, METHOD__CRITICAL_FINALIZER_OBJECT__FINALIZE, pModule);
    }

    // Note the fact that we've prepared this type before to prevent repetition of the work above. (Though repetition is harmless in
    // all other respects, so there's no need to worry about the race setting this flag).
    pMT->SetCriticalTypeHasBeenPrepared();
}

#ifdef _DEBUG

static char *g_rszContractNames[] = { "RCL_NO_CONTRACT", "RCL_BASIC_CONTRACT", "RCL_PREPARE_CONTRACT" };
static DWORD g_dwContractChecks = 0;

#define ReturnContractLevel(_level) do {                                                        \
    g_dwContractChecks++;                                                                       \
    if ((g_dwContractChecks % 100) == 0 && g_pMethodContractCache)                              \
        g_pMethodContractCache->DbgDumpStats();                                                 \
    ReliabilityContractLevel __level = (_level);                                                \
    CER_LOG(CONTRACTS, ("%s -- %s\n", pMD->m_pszDebugMethodName, g_rszContractNames[__level])); \
    return __level;                                                                             \
} while (false)
#else
#define ReturnContractLevel(_level) return (_level)
#endif

// Look for reliability contracts at the method, class and assembly level and parse them to extract the information we're interested
// in from a runtime preparation viewpoint. This information is abstracted in the form of the ReliabilityContractLevel enumeration.
ReliabilityContractLevel CheckForReliabilityContract(MethodDesc *pMD)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
        SO_TOLERANT;
    } CONTRACTL_END;

    // We are attempting to abstract reliability contracts for the given method into three different buckets: those methods that
    // will cause an error (or a MDA report at least) during preparation (RCL_NO_CONTRACT), those we allow but don't prepare
    // (RCL_BASIC_CONTRACT) and those we allow and prepare (RCL_PREPARE_CONTRACT).
    //
    // We place methods into the first bucket below that matches:
    //  RCL_NO_CONTRACT      --  Methods with no consistency or whose consistency states they may corrupt the appdomain or process.
    //  RCL_BASIC_CONTRACT   --  Methods that state CER.None (or don't specify a CER attribute)
    //  RCL_PREPARE_CONTRACT --  Methods that state CER.MayFail or CER.Success
    //
    // We look for reliability contracts at three levels: method, class and assembly. Definitions found at the method level override
    // those at the class and assembly level and those at the class level override assembly settings.
    //
    // In the interests of efficiency we cache contract information in a number of ways. Firstly we look at a hash of recently
    // queried MethodDescs. This contains authoritative answers (assembly/class/method information has already been composed so on a
    // hit we don't need to look anywhere else). This cache is allocated lazily, never grows (newer items eventually displace older
    // ones), is global, requires no locks and is never freed. The idea is to limit the amount of working set we ever occupy while
    // keeping the CPU usage as low as possible. Typical usages of this method involve querying a small number of methods in a stack
    // walk, possibly multiple times, so a small hash cache should work reasonably well here.
    //
    // On a miss we're going to have to bite the bullet and look at the assembly, class and method. The assembly and class cache
    // this information at load (ngen) time though, so they're not so expensive (class level data is cached on the EEClass, so it's
    // cold data, but the most performance sensitive scenario in which we're called here, ThreadAbort, isn't all that hot).

    // Check the cache first, it contains a raw contract level.
    ReliabilityContractLevel eLevel;
    if (g_pMethodContractCache && g_pMethodContractCache->Lookup(pMD, (DWORD*)&eLevel))
        ReturnContractLevel(eLevel);

    // Start at the method level and work up until we've found enough information to make a decision. The contract level is composed
    // in an encoded DWORD form that lets us track both parts of the state (consistency and cer) and whether each has been supplied
    // yet. See the RC_* macros for encoding details.
    DWORD dwMethodContractInfo = GetReliabilityContract(pMD->GetMDImport(), pMD->GetMemberDef());
    if (RC_INCOMPLETE(dwMethodContractInfo)) {
        DWORD dwClassContractInfo = pMD->GetClass()->GetReliabilityContract();
        dwMethodContractInfo = RC_MERGE(dwMethodContractInfo, dwClassContractInfo);
        if (RC_INCOMPLETE(dwMethodContractInfo)) {
            DWORD dwAssemblyContractInfo = pMD->GetAssembly()->GetReliabilityContract();
            dwMethodContractInfo = RC_MERGE(dwMethodContractInfo, dwAssemblyContractInfo);
        }
    }

    // We've got an answer, so attempt to cache it for the next time.

    // First check we have a cache (we allocate it lazily).
    if (g_pMethodContractCache == NULL) {
        PtrHashCache *pCache = new (nothrow) PtrHashCache();
        if (pCache)
            if (FastInterlockCompareExchangePointer((void**)&g_pMethodContractCache, pCache, NULL) != NULL)
                delete pCache;
    }

    // We still might not have a cache in low memory situations. That's OK.
    if (g_pMethodContractCache)
        g_pMethodContractCache->Add(pMD, RC_ENCODED_TO_LEVEL(dwMethodContractInfo));

    ReturnContractLevel(RC_ENCODED_TO_LEVEL(dwMethodContractInfo));
}


// Macro used to handle failures in the routine below.
#define IfFailRetRcNull(_hr) do { if (FAILED(_hr)) return RC_NULL; } while (false)

// Look for a reliability contract attached to the given metadata token in the given scope. Return the result as an encoded DWORD
// (see the RC_ENCODE macro).
DWORD GetReliabilityContract(IMDInternalImport *pImport, mdToken tkParent)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pImport));
    } CONTRACTL_END;

    HRESULT hr;
    DWORD   dwResult = RC_NULL;

    // Sadly we only have two unmanaged APIs available to us for looking at custom attributes. One looks up attributes by name but
    // only returns the byte blob, not the attribute ctor information (which we need to parse the blob) while the other returns
    // everything but requires us to enumerate all attributes on a given token looking for the one we're interested in. To keep the
    // cost down we probe for the existence of the attribute using the first API and then use the enumeration method if we get a
    // hit.
    hr = pImport->GetCustomAttributeByName(tkParent, RELIABILITY_CONTRACT_NAME, NULL, NULL);
    if (hr == S_FALSE)
        return RC_NULL;

    IfFailRetRcNull(hr);

    // Got at least one contract against this parent. Enumerate them all (filtering by name).
    MDEnumHolder hEnum(pImport);
    hr = pImport->SafeAndSlowEnumCustomAttributeByNameInit(tkParent, RELIABILITY_CONTRACT_NAME, &hEnum);
    _ASSERTE(hr != S_FALSE);
    IfFailRetRcNull(hr);

    // Enumerate over all the contracts.
    mdToken tkContract;
    while (S_OK == pImport->SafeAndSlowEnumCustomAttributeByNameNext(tkParent, RELIABILITY_CONTRACT_NAME, &hEnum, &tkContract)) {

        // Get the attribute type (token of the ctor used) since we need this information in order to parse the blob we'll find
        // next.
        mdToken tkAttrType;
        pImport->GetCustomAttributeProps(tkContract, &tkAttrType);
        if (!pImport->IsValidToken(tkAttrType))
            continue;

        // The token should be a member ref or method def.
        // Get the signature of the ctor so we know which version has been called.
        PCCOR_SIGNATURE pSig;
        DWORD           cbSig;
        if (TypeFromToken(tkAttrType) == mdtMemberRef) {
            pImport->GetNameAndSigOfMemberRef(tkAttrType, &pSig, &cbSig);
        } else {
            if (TypeFromToken(tkAttrType) != mdtMethodDef)
                continue;
            pImport->GetNameAndSigOfMethodDef(tkAttrType, &pSig, &cbSig);
        }

        // Only two signatures are supported: the null sig '()' and the full sig '(Consistency, CER)'.
        // Set a boolean based on which one was provided.
        bool                    fNullCtor;
        CorCallingConvention    eCallConv;

        // Check the calling convention is what we expect (default convention on an instance method).
        eCallConv = (CorCallingConvention)CorSigUncompressCallingConv(pSig);
        _ASSERTE(eCallConv == (IMAGE_CEE_CS_CALLCONV_DEFAULT | IMAGE_CEE_CS_CALLCONV_HASTHIS));
        if (eCallConv != (IMAGE_CEE_CS_CALLCONV_DEFAULT | IMAGE_CEE_CS_CALLCONV_HASTHIS))
            IfFailRetRcNull(COR_E_BADIMAGEFORMAT);

        // If so, the next datum is the count of arguments, and this is all we need to determine which ctor has been used.
        DWORD dwArgs = CorSigUncompressData(pSig);
        _ASSERTE(dwArgs == 0 || dwArgs == 2);
        if (dwArgs != 0 && dwArgs != 2)
            IfFailRetRcNull(COR_E_BADIMAGEFORMAT);

        fNullCtor = dwArgs == 0;

        // Now we know how to parse the blob, let's fetch a pointer to it.
        BYTE   *pbData;
        DWORD   cbData;
        pImport->GetCustomAttributeAsBlob(tkContract, (const void **)&pbData, &cbData);

        // Check serialization format (we support version 1 only).
        if (cbData < sizeof(WORD) || GET_UNALIGNED_VAL16(pbData) != 1)
            IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
        pbData += sizeof(WORD);
        cbData -= sizeof(WORD);

        // Parse ctor arguments if we have any.
        if (!fNullCtor) {

            // We assume the enums are based on DWORDS.
            if (cbData < (sizeof(DWORD) * 2))
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);

            // Consistency first.
            DWORD dwConsistency = GET_UNALIGNED_VAL32(pbData);
            pbData += sizeof(DWORD);
            cbData -= sizeof(DWORD);
            if (dwConsistency > RC_CONSISTENCY_CORRUPT_NOTHING)
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);

            // Followed by Cer.
            DWORD dwCer = GET_UNALIGNED_VAL32(pbData);
            pbData += sizeof(DWORD);
            cbData -= sizeof(DWORD);
            if (dwCer > RC_CER_SUCCESS)
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);

            dwResult = RC_MERGE(dwResult, RC_ENCODE(dwConsistency, dwCer));
        }

        // Get the count of field/property, value pairs.
        if (cbData < sizeof(WORD))
            IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
        WORD cPairs = GET_UNALIGNED_VAL16(pbData);
        pbData += sizeof(WORD);
        cbData -= sizeof(WORD);

        // Iterate over any such pairs, looking for values we haven't picked up yet.
        for (DWORD i = 0 ; i < cPairs; i++) {

            // First is a field vs property selector. We expect only properties.
            if (cbData < sizeof(BYTE) || *(BYTE*)pbData != SERIALIZATION_TYPE_PROPERTY)
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
            pbData += sizeof(BYTE);
            cbData -= sizeof(BYTE);

            // Next is the type of the property. It had better be an enum.
            if (cbData < sizeof(BYTE) || *(BYTE*)pbData != SERIALIZATION_TYPE_ENUM)
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
            pbData += sizeof(BYTE);
            cbData -= sizeof(BYTE);

            // Next we have the assembly qualified enum type name. This is preceded by a metadata style packed byte length (the
            // string itself is 8-bit and not null terminated). Ignore it (just skip across) and we'll key off the property name
            // (coming up) instead.
            DWORD cbName;
            CPackedLen::GetData((const void *)pbData, &cbName);
            if (cbData < (CPackedLen::Size(cbName) + cbName))
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
            pbData += CPackedLen::Size(cbName) + cbName;
            cbData -= CPackedLen::Size(cbName) + cbName;

            // Now we have the name of the property (in a similar format to above).
            CPackedLen::GetData((const void *)pbData, &cbName);
            if (cbData < (CPackedLen::Size(cbName) + cbName))
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
            pbData += CPackedLen::Size(cbName);
            cbData -= CPackedLen::Size(cbName);

            bool fConsistencyProp = false;
            if (cbName == strlen(RC_CONSISTENCY_PROP_NAME) && strncmp((const char*)pbData, RC_CONSISTENCY_PROP_NAME, cbName) == 0)
                fConsistencyProp = true;
            else if (cbName == strlen(RC_CER_PROP_NAME) && strncmp((const char*)pbData, RC_CER_PROP_NAME, cbName) == 0)
                fConsistencyProp = false;
            else
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
            pbData += cbName;
            cbData -= cbName;

            // And finally the actual enum value (again, we assume the underlying type is a DWORD).
            if (cbData < sizeof(DWORD))
                IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
            DWORD dwValue = GET_UNALIGNED_VAL32(pbData);
            pbData += sizeof(DWORD);
            cbData -= sizeof(DWORD);

            if (fConsistencyProp) {
                if (dwValue > RC_CONSISTENCY_CORRUPT_NOTHING)
                    IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
                dwResult = RC_MERGE(dwResult, RC_ENCODE(dwValue, RC_CER_UNDEFINED));
            } else {
                if (dwValue > RC_CER_SUCCESS)
                    IfFailRetRcNull(COR_E_BADIMAGEFORMAT);
                dwResult = RC_MERGE(dwResult, RC_ENCODE(RC_CONSISTENCY_UNDEFINED, dwValue));
            }
        }

        // Shouldn't have any bytes left in the blob at this stage.
        _ASSERTE(cbData == 0);
    }

    // Return the result encoded and packed into the 2 low order bits of a DWORD.
    return dwResult;
}

// Given a metadata token, a scoping module and a type context, look up the appropriate MethodDesc (and recomputed accompanying type
// context).
MethodContext *TokenToMethodDesc(Module *pModule, mdToken tokMethod, SigTypeContext *pTypeContext)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pTypeContext));
    } CONTRACTL_END;

    // Validate that the token is one that we can handle.
    if (!pModule->GetMDImport()->IsValidToken(tokMethod) || (TypeFromToken(tokMethod) != mdtMethodDef &&
                                                             TypeFromToken(tokMethod) != mdtMethodSpec &&
                                                             TypeFromToken(tokMethod) != mdtMemberRef)) {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_METHOD_TOKEN);
    }

    // Look up the MethodDesc based on the token and type context.
    MethodDesc *pMD = MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecThrowing(pModule,
                                                                                  tokMethod,
                                                                                  pTypeContext,
                                                                                  TRUE,
                                                                                  FALSE);

    // The MethodDesc we get might be shared between several types. If so we'll need to do extra work to locate the exact
    // class instantiation instead of the default representative one.
    SigTypeContext sNewTypeContext;
    if (pMD->IsSharedByGenericInstantiations()) {
        TypeHandle th = GetTypeFromMemberDefOrRefOrSpecThrowing(pModule,
                                                                tokMethod,
                                                                pTypeContext);
        SigTypeContext::InitTypeContext(pMD, th,&sNewTypeContext);
    } else
        SigTypeContext::InitTypeContext(pMD, pMD->GetClassInstantiation(), pMD->GetMethodInstantiation(),&sNewTypeContext);

    return MethodContext::PerThreadAllocate(pMD, &sNewTypeContext);
}

// Locate an exact type definition given a method token and the type context in which it can be resolved.
TypeHandle GetTypeFromMemberDefOrRefOrSpecThrowing(Module         *pModule,
                                                   mdToken         tokMethod,
                                                   SigTypeContext *pTypeContext)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pTypeContext));
    } CONTRACTL_END;

    IMDInternalImport *pImport = pModule->GetMDImport();

    // Convert method specs into the underlying member ref.
    if (TypeFromToken(tokMethod) == mdtMethodSpec) {
      PCCOR_SIGNATURE   pSig;
      ULONG             cSig;
      mdMemberRef       tkGenericMemberRef;

      pImport->GetMethodSpecProps(tokMethod, &tkGenericMemberRef, &pSig, &cSig);
      if (!pImport->IsValidToken(tkGenericMemberRef) || (TypeFromToken(tkGenericMemberRef) != mdtMethodDef &&
                                                         TypeFromToken(tkGenericMemberRef) != mdtMemberRef))
          COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN_TYPE);

      tokMethod = tkGenericMemberRef;
    }

    // Follow the member ref/def back up to the type def/ref/spec or module (for global methods).
    if (TypeFromToken(tokMethod) == mdtMemberRef) {
      tokMethod = pImport->GetParentOfMemberRef(tokMethod);

      // For varargs, a memberref can point to a methodDef
      if(TypeFromToken(tokMethod) == mdtMethodDef) {
          if (!pImport->IsValidToken(tokMethod))
              COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN);

          pImport->GetParentToken(tokMethod, &tokMethod);
      }
    } else if (TypeFromToken(tokMethod) == mdtMethodDef)
        pImport->GetParentToken(tokMethod, &tokMethod);

    if (!pImport->IsValidToken(tokMethod) || (TypeFromToken(tokMethod) != mdtTypeDef  &&
                                              TypeFromToken(tokMethod) != mdtTypeRef  &&
                                              TypeFromToken(tokMethod) != mdtTypeSpec &&
                                              TypeFromToken(tokMethod) != mdtModuleRef))
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN);

    // Load the type in question, using a type context if necessary to get an exact representation.
    TypeHandle th;
    if (TypeFromToken(tokMethod) == mdtModuleRef) {
        DomainFile *pNewModule = pModule->LoadModule(GetAppDomain(), tokMethod, FALSE);
        if (pNewModule != NULL)
            th = TypeHandle(pNewModule->GetModule()->GetGlobalMethodTable());
    } else {
        th = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule,
                                                         tokMethod,
                                                         pTypeContext);
    }

    if (th.IsNull())
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

    return th;
}

bool IsCerRootMethod(MethodDesc *pMD)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
        SO_TOLERANT;
    } CONTRACTL_END;

    // Treat IL stubs as CER roots (marshaling code needs to string together operations without being interruped by thread aborts).
    if (pMD->IsILStub())
        return true;

    // There are some well defined root methods defined by the system.
    // Use GetExistingMethod below to avoid GC (we can only do this because we know the method will already have been looked up via
    // the binder prior to it being called).
    if (pMD == g_Mscorlib.GetExistingMethod(METHOD__RUNTIME_HELPERS__EXECUTE_BACKOUT_CODE_HELPER))
        return true;

    // For now we just look to see whether there is some prep or fixup info stored for this method.
    Module *pModule = pMD->GetModule();

    if (pModule->GetCerPrepInfo(pMD) != NULL)
        return true;


    return false;
}

// Fill the cache of overflowed generic dictionary entries that the jit maintains with all the overflow slots stored so far in the
// dictionary layout.
void PrepopulateGenericHandleCache(DictionaryLayout  *pDictionaryLayout,
                                   MethodDesc        *pMD,
                                   MethodTable       *pMT)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Dictionary overflow entries are recorded starting in the second bucket of the dictionary layout.
    DictionaryLayout *pOverflows = pDictionaryLayout->GetNextLayout();

    // Ignore class passed in unless it's required to provide exact instantiation information.
    TypeHandle thDeclaringClass;
    if (!pMD->HasMethodInstantiation())
        thDeclaringClass = TypeHandle(pMT);

    while (pOverflows) {
        for (DWORD i = 0; i < pOverflows->GetMaxSlots(); i++) {
            DictionaryEntryLayout *pEntry = pOverflows->GetEntryLayout(i);

            // We've finished as soon as we find the first unused slot.
            if (!pEntry->m_token1)
                return;

            // We have a valid overflow entry. Determine the handle value given the type context we have and push it into the JIT's
            // cache.
            JIT_GenericHandleWorker(pMD, thDeclaringClass, pEntry->m_token1, pEntry->m_token2, NULL);
        }
        pOverflows = pOverflows->GetNextLayout();
    }
}


PtrHashCache::PtrHashCache()
{
    LEAF_CONTRACT;
    ZeroMemory(this, sizeof(*this));

    // First entry in each bucket is a chain index used to evenly distribute inserts within a bucket.
    _ASSERTE(PHC_CHAIN > 1);
}

bool PtrHashCache::Lookup(void *pKey, DWORD *pdwValue)
{
    LEAF_CONTRACT;
    _ASSERTE(((UINT_PTR)pKey & PHC_DATA_MASK) == 0);

    DWORD dwBucket = GetHash(pKey);

    // Skip first entry in bucket, it's a sequence number used for insertions.
    for (DWORD i = 1; i < PHC_CHAIN; i++) {
        UINT_PTR uipEntry = *(volatile UINT_PTR*)&m_rEntries[(dwBucket * PHC_CHAIN) + i];
        if ((uipEntry & ~PHC_DATA_MASK) == (UINT_PTR)pKey) {
#ifdef _DEBUG
            FastInterlockIncrement((LONG*)&m_dwHits);
#endif
            *pdwValue = uipEntry & PHC_DATA_MASK;
            return true;
        }
    }

#ifdef _DEBUG
    FastInterlockIncrement((LONG*)&m_dwMisses);
#endif
    return false;
}

void PtrHashCache::Add(void *pKey, DWORD dwValue)
{
    LEAF_CONTRACT;
    _ASSERTE(((UINT_PTR)pKey & PHC_DATA_MASK) == 0);
    _ASSERTE((dwValue & ~PHC_DATA_MASK) == 0);

    DWORD dwBucket = GetHash(pKey);

    // We keep a sequence number in the first entry of the bucket so that we distribute insertions within the bucket evenly. We're
    // racing when we update this value, but it doesn't matter if we lose an update (we're a cache after all). We don't bother being
    // careful to avoid overflowing the value here (we just keep incrementing); we'll do the modulo logic when we insert our value
    // instead.
    DWORD dwIndex = m_rEntries[dwBucket * PHC_CHAIN]++;
    dwIndex = (dwIndex % (PHC_CHAIN - 1)) + 1;
    m_rEntries[(dwBucket * PHC_CHAIN) + dwIndex] = ((UINT_PTR)pKey & ~PHC_DATA_MASK) | dwValue;
}

DWORD PtrHashCache::GetHash(void *pKey)
{
    LEAF_CONTRACT;

    return (DWORD)(((UINT_PTR)pKey >> 4) % PHC_BUCKETS);
}

#ifdef _DEBUG
void PtrHashCache::DbgDumpStats()
{
}
#endif
