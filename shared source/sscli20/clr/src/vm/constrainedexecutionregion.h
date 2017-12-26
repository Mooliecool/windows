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

#ifndef __CONSTRAINED_EXECUTION_REGION_H
#define __CONSTRAINED_EXECUTION_REGION_H


#include <common.h>
#include <corhlpr.h>
#include <typestring.h>


// An enumeration that abstracts the interesting information (from our point of view) present in a reliability contract decorating a
// method.
enum ReliabilityContractLevel
{
    RCL_UNKNOWN             = -1,   // The contract attribute hasn't been read yet
    RCL_NO_CONTRACT         = 0,    // No contract (or a fairly useless one) was specified
    RCL_BASIC_CONTRACT      = 1,    // The contract promises enough for the method to be a legal member of a CER call graph
    RCL_PREPARE_CONTRACT    = 2,    // The contract promises enough to be worth preparing the method as part of a CER call graph
};


// Look for reliability contracts at the method, class and assembly level and parse them to extract the information we're interested
// in from a runtime preparation viewpoint. This information is abstracted in the form of the ReliabilityContractLevel enumeration.
ReliabilityContractLevel CheckForReliabilityContract(MethodDesc *pMD);


// Structure used to track enough information to identify a method (possibly generic or belonging to a generic class). Includes a
// MethodDesc pointer and a SigTypeContext (values of class and method type parameters to narrow down the exact method being refered
// to). Similar to MethodContext (see ConstrainedExecutionRegion.cpp), but without the unneeded list link field (we expect to embed
// these in arrays, hence the name).
struct MethodContextElement
{
    MethodDesc     *m_pMethodDesc;      // Pointer to a MethodDesc
    SigTypeContext  m_sTypeContext;     // Additional type parameter information to qualify the exact method targetted
};


// Base structure used to track which CERs have been prepared so far.
// These structures are looked up via a per-assembly hash table using the root method desc as a key.
// Used to avoid extra work in both the jit and PrepareMethod calls. The latter case is more involved because we support preparing a
// CER with generic type parameters (the instantiation is passed in along with the method in the PrepareMethod call). In that case
// we need to track exactly which instantiations we've prepared for a given method.
struct CerPrepInfo
{
    CerPrepInfo() :
        m_fFullyPrepared(false),
        m_fRequiresInstantiation(false),
        m_fMethodHasCallsWithinExplicitCer(false)
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        } CONTRACTL_END;

        if (!m_sIsInitAtInstHash.Init(17, NULL, NULL, FALSE))
            COMPlusThrowOM();
    }

    bool                        m_fFullyPrepared;           // True implies we've prep'd this once and there are no shared instantiations
    bool                        m_fRequiresInstantiation;   // True implies that this method is shared amongst multiple instantiations
    bool                        m_fMethodHasCallsWithinExplicitCer; // True if method contains calls out from within an explicit PCER range
    EEInstantiationHashTable    m_sIsInitAtInstHash;        // Hash of instantiations we've prepared this CER for 
};




// Default initial size for hash table used to track CerPrepInfo structures on a per-module basis.
#define CER_DEFAULT_HASH_SIZE   17


// Structure used to track a single exception handling range (catch, finally etc.). We build an array of these and then track which
// ones have become 'activated' by virtue of having their try clause immediately preceded by a call to our preparation marker
// method. This allows us to determine which call sites in the method body should be followed during method preparation.
struct EHClauseRange
{
    DWORD   m_dwTryOffset;
    DWORD   m_dwHandlerOffset;
    DWORD   m_dwHandlerLength;
    bool    m_fActive;
};


// Structure used to track enough information to identify a method (possibly generic or belonging to a generic class). Includes a
// MethodDesc pointer and a SigTypeContext (values of class and method type parameters to narrow down the exact method being refered
// to). The structure also contains a next pointer so that it can be placed in a singly linked list (see MethodContextStack below).
struct MethodContext
{
    MethodContext  *m_pNext;            // Next MethodContext in a MethodContextStack list
    MethodDesc     *m_pMethodDesc;      // Pointer to a MethodDesc
    SigTypeContext  m_sTypeContext;     // Additional type parameter information to qualify the exact method targetted
    bool            m_fRoot;            // Does this method contain a CER root of its own?

    // Allocate and initialize a MethodContext from the per-thread stacking allocator (we assume the checkpoint has already been
    // taken).
    static MethodContext* PerThreadAllocate(MethodDesc *pMD, SigTypeContext *pTypeContext)
    {
        CONTRACTL {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        } CONTRACTL_END;

        MethodContext *pContext = (MethodContext*)GetThread()->m_MarshalAlloc.Alloc(sizeof(MethodContext));
        pContext->m_pMethodDesc = pMD;
        pContext->m_sTypeContext = *pTypeContext;
        pContext->m_fRoot = false;

        return pContext;
    }

    // Determine if two MethodContexts are equivalent (same MethodDesc pointer and identical arrays of TypeHandles in the
    // TypeContext).
    bool Equals(MethodContext *pOther)
    {
        WRAPPER_CONTRACT;

        if (pOther->m_pMethodDesc != m_pMethodDesc)
            return false;

        if (pOther->m_sTypeContext.m_classInstCount != m_sTypeContext.m_classInstCount)
            return false;

        if (pOther->m_sTypeContext.m_methInstCount != m_sTypeContext.m_methInstCount)
            return false;

        DWORD i;

        for (i = 0; i < m_sTypeContext.m_classInstCount; i++)
            if (pOther->m_sTypeContext.m_classInst[i] != m_sTypeContext.m_classInst[i])
                return false;

        for (i = 0; i < m_sTypeContext.m_methInstCount; i++)
            if (pOther->m_sTypeContext.m_methInst[i] != m_sTypeContext.m_methInst[i])
                return false;

        return true;
    }

#ifdef _DEBUG
#define CER_DBG_MAX_OUT 4096
    char *ToString()
    {
        WRAPPER_CONTRACT;

        // Support up to two ToString calls before we re-use a buffer and overwrite previous output.
        static char szOut1[CER_DBG_MAX_OUT];
        static char szOut2[CER_DBG_MAX_OUT];
        static char *pszOut = szOut1;

        StackSString ssBuffer;
        StackScratchBuffer ssScratch;

        TypeString::AppendMethod(ssBuffer, m_pMethodDesc, m_sTypeContext.m_classInst, TypeString::FormatNamespace | TypeString::FormatAngleBrackets);
        sprintf_s(&pszOut[0], CER_DBG_MAX_OUT, "%s", ssBuffer.GetUTF8(ssScratch));

        char *pszReturn = pszOut;
        pszOut = pszOut == szOut1 ? szOut2 : szOut1;
        return pszReturn;
    }
#endif
};

// Maintains a stack of MethodContexts (implemented as a singly linked list with insert and remove operations only at the head).
class MethodContextStack
{
    MethodContext  *m_pFirst;       // The head of the linked list
    DWORD           m_cElements;    // Count of elements in the stack

public:

    // Initialize to an empty list.
    MethodContextStack()
    {
        LEAF_CONTRACT;

        m_pFirst = NULL;
        m_cElements = 0;
    }

    // Push a MethodContext pointer on the head of the list.
    void Push(MethodContext *pContext)
    {
        LEAF_CONTRACT;

        pContext->m_pNext = m_pFirst;
        m_pFirst = pContext;
        m_cElements++;
    }

    // Remove and retrieve the most recently pushed MethodContext. Return NULL if no more entries exist.
    MethodContext *Pop()
    {
        LEAF_CONTRACT;

        MethodContext* pContext = m_pFirst;
        if (pContext == NULL)
            return NULL;

        m_pFirst = pContext->m_pNext;
        m_cElements--;

        return pContext;
    }

    // Return true if an MethodContext equivalent to the argument exists in the stack.
    bool IsInStack(MethodContext *pMatchContext)
    {
        WRAPPER_CONTRACT;

        MethodContext* pContext = m_pFirst;
        while (pContext) {
            if (pContext->Equals(pMatchContext))
                return true;
            pContext = pContext->m_pNext;
        }

        return false;
    }

    // Get count of elements in the stack.
    DWORD GetCount()
    {
        LEAF_CONTRACT;

        return m_cElements;
    }
};


class MethodCallGraphPreparer
{
    MethodDesc *m_pRootMD;
    SigTypeContext *m_pRootTypeContext;

    COR_ILMETHOD_DECODER *m_pMethodDecoder; 
    
    MethodContextStack  m_sLeftToProcess;         // MethodContexts we have yet to look at in this call graph
    MethodContextStack  m_sAlreadySeen;           // MethodContexts we've already processed at least once

    EHClauseRange      *m_pEHClauses;             // Array of exception handling clauses in current method (only if !fEntireMethod)
    DWORD               m_cEHClauses;             // Number of elements in above array
    CerPrepInfo        *m_pCerPrepInfo;    // Context recording how much preparation this region has had
    Thread             *m_pThread;  // Cached managed thread pointer (for allocations and the like)
    bool        m_fPartialPreparation;    // True if we have unbound type vars at the CER root and can only prep one instantiation at a time

    bool        m_fEntireMethod;    // True if are preparing for the entire method
    bool        m_fExactTypeContext;
    bool        m_fMethodHasCallsWithinExplicitCer;  // True if method contains calls out from within an explicit PCER range

  public:
    MethodCallGraphPreparer(MethodDesc *pRootMD, SigTypeContext *pRootTypeContext, bool fEntireMethod, bool fExactTypeContext);

    // Walk the call graph of the method given by m_pRootMD (and type context in m_pRootTypeContext which provides instantiation information
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
    //
    // Returns true if the m_pRootMD contains a CER that calls outside the method. 
    //
    bool Run();
    
  private:
    void GetEHClauses();
    void MarkEHClauseActivatedByCERCall(MethodContext *pContext, BYTE *pbIL, DWORD cbIL);
    bool MethodCallGraphPreparer::CheckIfCallsiteWithinCER(DWORD dwOffset);
    bool ShouldGatherExplicitCERCallInfo();
    void LookForInterestingCallsites(MethodContext *pContext);
    void PrepareMethods();
    bool RecordResults();
};

// Determines whether the given method contains a CER root that can be pre-prepared (i.e. prepared at jit time).
bool ContainsPrePreparableCerRoot(MethodDesc *pMD);

// Prepares the critical finalizer call graph for the given object type (which must derive from CriticalFinalizerObject). This
// involves preparing at least the finalizer method and possibly some others (for SafeHandle and CriticalHandle derivations). If a
// module pointer is supplied then only the critical methods introduced in that module are prepared (this is used at ngen time to
// ensure that we're only generating ngen preparation info for the targetted module).
void PrepareCriticalFinalizerObject(MethodTable *pMT, Module *pModule = NULL);

bool IsCerRootMethod(MethodDesc *pMD);

// Fill the cache of overflowed generic dictionary entries that the jit maintains with all the overflow slots stored so far in the
// dictionary layout.
void PrepopulateGenericHandleCache(DictionaryLayout  *pDictionaryLayout,
                                   MethodDesc        *pMD,
                                   MethodTable       *pMT);
    
DWORD GetReliabilityContract(IMDInternalImport *pImport, mdToken tkParent);



// A fixed sized hash table keyed by pointers and storing two bits worth of value for every entry. The value is stored in the low
// order bits of the keys, so the pointers must be at least DWORD aligned. No hash table expansion occurs so new entries will sooner
// or later overwrite old. The implementation uses no locks (all accesses are single aligned pointer sized operations and therefore
// inherently atomic).
// The purpose of this table is to store a smallish number of reliability contract levels for the most recently queried methods,
// mainly for the purpose of speeding up thread abort processing (where we will walk the stack probing methods for contracts,
// sometimes repeatedly). So we use a small fixed sized hash to speed up lookups on average but avoid impacting working set.
#define PHC_BUCKETS     29
#define PHC_CHAIN       5
#define PHC_DATA_MASK   3

class PtrHashCache
{
public:
    PtrHashCache();
    bool Lookup(void *pKey, DWORD *pdwValue);
    void Add(void *pKey, DWORD dwValue);

#ifdef _DEBUG
    void DbgDumpStats();
#endif

private:
    DWORD GetHash(void *pKey);

    UINT_PTR    m_rEntries[PHC_BUCKETS * PHC_CHAIN];

#ifdef _DEBUG
    DWORD       m_dwHits;
    DWORD       m_dwMisses;
#endif
};


#endif
