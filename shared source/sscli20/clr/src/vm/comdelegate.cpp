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
////////////////////////////////////////////////////////////////////////////////
// This module contains the implementation of the native methods for the
//  Delegate class.
//
// Date: June 1998
////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "comdelegate.h"
#include "invokeutil.h"
#include "excep.h"
#include "class.h"
#include "field.h"
#include "dllimportcallback.h"
#include "dllimport.h"
#include "remoting.h"
#include "eeconfig.h"
#include "mdaassistantsptr.h"
#include "cgensys.h"
#include "asmconstants.h"
#include "security.h"
#include "securitydescriptor.h"
#include "virtualcallstub.h"


#define DELEGATE_MARKER_UNMANAGEDFPTR -1

Stub* COMDelegate::s_pInvokeStub = NULL;
Stub* COMDelegate::s_pHasRetBuffInvokeStub = NULL;


#ifdef _X86_
// do not call this - it is pointer to instruction within DllImportForDelegateGenericStubWorker
extern "C" void __stdcall DllImportForDelegateGenericStubReturnFromCall(void);
#else // !_X86_
extern "C" PVOID DllImportForDelegateGenericStubReturnFromCall;
#endif // !_X86_

static const int DllImportForDelegateGenericWorkerStackSize = NDirectGenericWorkerFrameSize;





#ifndef DACCESS_COMPILE




static BYTE* GetVirtualCallStub(MethodDesc *method, TypeHandle scopeType)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM()); // from MetaSig::SizeOfActualFixedArgStack
    }
    CONTRACTL_END;

    //TODO: depending on what we decide for generics method we may want to move this check to better places
    if (method->IsGenericMethodDefinition() || method->HasMethodInstantiation())
    {
        COMPlusThrow(kNotSupportedException);
    }

    // need to grab a virtual stub 
    AppDomain* currentDomain = GetAppDomain();
    VirtualCallStubManager *pVirtualStubManager = currentDomain->GetVirtualCallStubManager();
    BYTE* pTargetCall = pVirtualStubManager->GetCallStub(scopeType, method);
    _ASSERTE(pTargetCall);
    return pTargetCall;
}

VOID GenerateShuffleArray(MethodDesc* pInvoke, MethodDesc *pTargetMeth, ShuffleEntry* pShuffleEntryArray, size_t nEntries)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM()); // from MetaSig::SizeOfActualFixedArgStack
    }
    CONTRACTL_END;

    //
    //

#if defined(_X86_)

    INDEBUG(ShuffleEntry *dbg_pShuffleEntryArrayEnd = pShuffleEntryArray + nEntries;)

    // Must create independent msigs to prevent the argiterators from
    // interfering with other.
    MetaSig msig1(pInvoke);
    _ASSERTE(msig1.HasThis());

    MetaSig msig2((pTargetMeth != NULL) ? pTargetMeth : pInvoke);
    if (pTargetMeth == NULL) 
    {
        msig2.DangerousRemoveHasThis();
        _ASSERTE(!msig2.HasThis());
    }

    ArgIterator    aisrc(NULL, &msig1, FALSE);
    ArgIterator    aidst(NULL, &msig2, (pTargetMeth) ? pTargetMeth->IsStatic() : TRUE);

    UINT stacksizedelta;

    if (pTargetMeth == NULL) 
    {
        PCCOR_SIGNATURE pSig;
        DWORD cbSigSize;
        pInvoke->GetSig(&pSig, &cbSigSize);
        PREFIX_ASSUME(pSig != NULL);
        Module *pModule = pInvoke->GetModule();
        SigTypeContext typeContext(pInvoke);

        stacksizedelta = MetaSig::SizeOfActualFixedArgStack(pModule, pSig, cbSigSize, FALSE, &typeContext) -
                         MetaSig::SizeOfActualFixedArgStack(pModule, pSig, cbSigSize, TRUE, &typeContext);
    }
    else
    {
        stacksizedelta = pInvoke->SizeOfActualFixedArgStack() - pTargetMeth->SizeOfActualFixedArgStack();
    }

    UINT srcregofs = (UINT)(-1), dstregofs = (UINT)(-1);
    INT  srcofs,   dstofs;
    UINT cbSize;
    BYTE typ;

    // if the function is non static we need to place the 'this' first
    if (pTargetMeth != NULL && !pTargetMeth->IsStatic())
    {
        srcofs = aisrc.GetNextOffset(&typ, &cbSize, &srcregofs);
        _ASSERTE(srcofs); // cannot be null
        srcofs -= FramedMethodFrame::GetOffsetOfReturnAddress();

        pShuffleEntryArray->srcofs = (srcregofs == (UINT)(-1)) ? srcofs : (ShuffleEntry::REGMASK | srcregofs);
        pShuffleEntryArray->dstofs = ShuffleEntry::REGMASK | 4;

        pShuffleEntryArray++;
    }
    else if (msig1.HasRetBuffArg())
    {
        // the first register is used for 'this'
        srcofs = aisrc.GetRetBuffArgOffset(&srcregofs);
        dstofs = aidst.GetRetBuffArgOffset(&dstregofs);

        // ret buff arg has to be in a register
        _ASSERTE(srcregofs != (UINT)(-1));
        _ASSERTE(dstregofs != (UINT)(-1));

        pShuffleEntryArray->srcofs = ShuffleEntry::REGMASK | srcregofs;
        pShuffleEntryArray->dstofs = ShuffleEntry::REGMASK | dstregofs;

        pShuffleEntryArray++;
    }

    while (0 != (srcofs = aisrc.GetNextOffset(&typ, &cbSize, &srcregofs)))
    {
        dstofs = aidst.GetNextOffset(&typ, &cbSize, &dstregofs) + stacksizedelta;

        cbSize = StackElemSize(cbSize);

        srcofs -= FramedMethodFrame::GetOffsetOfReturnAddress();
        dstofs -= FramedMethodFrame::GetOffsetOfReturnAddress();

        while (cbSize)
        {
            pShuffleEntryArray->srcofs =
                (srcregofs == (UINT)(-1)) ? srcofs : (ShuffleEntry::REGMASK | srcregofs);

            pShuffleEntryArray->dstofs =
                (dstregofs == (UINT)(-1)) ? dstofs : (ShuffleEntry::REGMASK | dstregofs);

            srcofs += STACK_ELEM_SIZE;
            dstofs += STACK_ELEM_SIZE;

            if (pShuffleEntryArray->srcofs != pShuffleEntryArray->dstofs)
            {
                pShuffleEntryArray++;
            }

            cbSize -= STACK_ELEM_SIZE;
        }
    }

    if (stacksizedelta != 0)
    {
        // Emit code to move the return address
        pShuffleEntryArray->srcofs = 0;     // retaddress is assumed to be at esp
        pShuffleEntryArray->dstofs = stacksizedelta;
        pShuffleEntryArray++;
    }

    pShuffleEntryArray->srcofs = ShuffleEntry::SENTINEL;
    pShuffleEntryArray->dstofs = (UINT16)stacksizedelta;

    // Make sure we didn't overstep our bounds.
    CONSISTENCY_CHECK(pShuffleEntryArray <= dbg_pShuffleEntryArrayEnd);

#elif defined(_PPC_)

    UINT cbSize;
    INT  ofs;
    BYTE typ;

    MetaSig msig(pInvoke);
    ArgIterator ai(NULL, &msig, FALSE);

    int numRegisterUsed = 0; // numRegisterUsed in destination
#ifdef _PPC_
    int numFloatRegisterUsed = 0;
#endif

#if THISPTR_LOCATION > 0
    if (msig.HasRetBuffArg())
        numRegisterUsed++;
#endif // THISPTR_LOCATION > 0

    // this pointer is special
    pShuffleEntryArray->srcofs = ShuffleEntry::REGMASK | (numRegisterUsed * STACK_ELEM_SIZE);
    pShuffleEntryArray->dstofs = (UINT16)-1;
    pShuffleEntryArray++;

    while (0 != (ofs = ai.GetNextOffset(&typ, &cbSize, NULL)))
    {
        ofs -= sizeof(FramedMethodFrame);

#ifdef _PPC_
        // skip the enregistered float arguments
        if ((typ == ELEMENT_TYPE_R4) || (typ == ELEMENT_TYPE_R8))
        {
            if (numFloatRegisterUsed  < NUM_FLOAT_ARGUMENT_REGISTERS)
            {
                numRegisterUsed += cbSize / STACK_ELEM_SIZE;
                numFloatRegisterUsed++;
                continue;
            }
        }
#endif // _PPC_


        cbSize = StackElemSize(cbSize);

        while (cbSize > 0)
        {
            pShuffleEntryArray->srcofs = ((numRegisterUsed+1) * STACK_ELEM_SIZE) |
                ((numRegisterUsed+1 < NUM_ARGUMENT_REGISTERS) ? ShuffleEntry::REGMASK : 0);

            pShuffleEntryArray->dstofs = (numRegisterUsed * STACK_ELEM_SIZE) |
                ((numRegisterUsed < NUM_ARGUMENT_REGISTERS) ? ShuffleEntry::REGMASK : 0);

            numRegisterUsed++;

            cbSize -= STACK_ELEM_SIZE;
            pShuffleEntryArray++;
        }
    }

    pShuffleEntryArray->srcofs = ShuffleEntry::SENTINEL;
    pShuffleEntryArray->dstofs = 0;

#else

    PORTABILITY_ASSERT("GenerateShuffleArray not implemented on this platform");

#endif
}


class ShuffleThunkCache : public MLStubCache
{
private:
    //---------------------------------------------------------
    // Compile a static delegate shufflethunk. Always returns
    // STANDALONE since we don't interpret these things.
    //---------------------------------------------------------
    virtual MLStubCompilationMode CompileMLStub(const BYTE *pRawMLStub,
                                                StubLinker *pstublinker,
                                                void *callerContext)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        MLStubCompilationMode ret = INTERPRETED;
        EX_TRY
        {
            ((CPUSTUBLINKER*)pstublinker)->EmitShuffleThunk((ShuffleEntry*)pRawMLStub, FALSE);
            ret = STANDALONE;
        }
        EX_CATCH
        {
            // In case of an error, we'll just leave the mode as "INTERPRETED."
            // and let the caller of Canonicalize() treat that as an error.
        }
        EX_END_CATCH(RethrowTransientExceptions)
        return ret;
    }

    //---------------------------------------------------------
    // Tells the MLStubCache the length of a ShuffleEntryArray.
    //---------------------------------------------------------
    virtual UINT Length(const BYTE *pRawMLStub)
    {
        LEAF_CONTRACT;
        ShuffleEntry *pse = (ShuffleEntry*)pRawMLStub;
        while (pse->srcofs != ShuffleEntry::SENTINEL)
        {
            pse++;
        }
        return sizeof(ShuffleEntry) * (UINT)(1 + (pse - (ShuffleEntry*)pRawMLStub));
    }

    virtual void AddStub(const BYTE* pRawMLStub, Stub* pNewStub)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;


        DelegateInvokeStubManager::g_pManager->AddStub(pNewStub);
    }
};

ShuffleThunkCache *COMDelegate::m_pShuffleThunkCache = NULL;
MulticastStubCache *COMDelegate::m_pSecureDelegateStubCache = NULL;
MulticastStubCache *COMDelegate::m_pMulticastStubCache = NULL;

MethodTable* COMDelegate::s_pIAsyncResult = 0;
MethodTable* COMDelegate::s_pAsyncCallback = 0;
CrstStatic   COMDelegate::s_DelegateToFPtrHashCrst;
PtrHashMap*  COMDelegate::s_pDelegateToFPtrHash = NULL;


// One time init.
void COMDelegate::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    s_DelegateToFPtrHashCrst.Init("DelegateToFPtrHash", CrstSyncHashLock, CRST_UNSAFE_ANYMODE);

    s_pDelegateToFPtrHash = ::new PtrHashMap();

    LockOwner lock = {&COMDelegate::s_DelegateToFPtrHashCrst, IsOwnerOfCrst};
    s_pDelegateToFPtrHash->Init(TRUE, &lock);

    m_pShuffleThunkCache = new ShuffleThunkCache();
    m_pMulticastStubCache = new MulticastStubCache();
    m_pSecureDelegateStubCache = new MulticastStubCache();
}

size_t COMDelegate::GetShuffleArrayEntryCount(MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    size_t nEntries;
    

    g_IBCLogger.LogMethodDescAccess(pMD);
    nEntries = (3 + pMD->SizeOfActualFixedArgStack() / STACK_ELEM_SIZE); 

#ifndef _DEBUG
    // This allocsize prediction is easy to break, so in retail, add
    // some fudge to be safe.
    nEntries += 3;
#endif // !_DEBUG

    return nEntries;
}


// We need a LoaderHeap that lives at least as long as the DelegateEEClass, but ideally no longer
LoaderHeap *DelegateEEClass::GetStubHeap()
{
    return m_pInvokeMethod->GetDomain()->GetStubHeap();
}


Stub* COMDelegate::SetupShuffleThunk(DelegateEEClass *pClass, MethodDesc *pTargetMeth)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    Stub* pShuffleThunk;
    MethodDesc *pMD = pClass->m_pInvokeMethod;
    size_t          nEntries            = GetShuffleArrayEntryCount(pMD);
    size_t          allocsize;
    if (!ClrSafeInt<size_t>::multiply(nEntries, sizeof(ShuffleEntry), allocsize))
        COMPlusThrowOM();
    CQuickBytes         rShuffleEntryArray;
    rShuffleEntryArray.AllocThrows(allocsize);

    FillMemory(rShuffleEntryArray.Ptr(), allocsize, 0);

    GenerateShuffleArray(pMD, pTargetMeth, (ShuffleEntry*)rShuffleEntryArray.Ptr(), nEntries);

    MLStubCache::MLStubCompilationMode mode;
    pShuffleThunk = m_pShuffleThunkCache->Canonicalize((const BYTE *)rShuffleEntryArray.Ptr(), &mode);
    if (!pShuffleThunk || mode != MLStubCache::STANDALONE)
    {
        COMPlusThrowOM();
    }

    if (!pTargetMeth->IsStatic() && pTargetMeth->HasRetBuffArg()) 
    {
        if (FastInterlockCompareExchangePointer((PVOID*)&pClass->m_pInstRetBuffCallStub, pShuffleThunk, NULL ) != NULL)
        {
            pShuffleThunk->DecRef();
            pShuffleThunk = pClass->m_pInstRetBuffCallStub;
        }
    }
    else
    {
        if (FastInterlockCompareExchangePointer((PVOID*)&pClass->m_pStaticCallStub, pShuffleThunk, NULL ) != NULL)
        {
            pShuffleThunk->DecRef();
            pShuffleThunk = pClass->m_pStaticCallStub;
        }
    }

    g_IBCLogger.LogEEClassCOWTableAccess(pClass);

    return pShuffleThunk;
}

FCIMPL5(FC_BOOL_RET, COMDelegate::BindToMethodName, 
                        Object *refThisUNSAFE, 
                        Object *targetUNSAFE, 
                        void *methodTH, 
                        StringObject* methodNameUNSAFE, 
                        int flags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc *pMatchingMethod = NULL;
    struct _gc
    {
        DELEGATEREF refThis;
        OBJECTREF target;
        STRINGREF methodName;
    } gc;

    gc.refThis    = (DELEGATEREF) ObjectToOBJECTREF(refThisUNSAFE);
    gc.target     = (OBJECTREF) targetUNSAFE;
    gc.methodName = (STRINGREF) methodNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc);

    // Caching of MethodDescs (impl and decl) for MethodTable slots provided significant
    // performance gain in some reflection emit scenarios.
    MethodTable::AllowMethodDataCaching();

    
    TypeHandle methodType = TypeHandle::FromPtr(methodTH);

    TypeHandle targetType((gc.target != NULL) ? gc.target->GetTrueMethodTable() : NULL);
    // get the invoke of the delegate
    TypeHandle delegateType = gc.refThis->GetTypeHandle();
    MethodDesc* pInvokeMeth = COMDelegate::FindDelegateInvokeMethod(delegateType.GetMethodTable());
    _ASSERTE(pInvokeMeth);

    //
    // now loop through the methods looking for a match
    //

    // get the name in UTF8 format
    SString wszName(SString::Literal, gc.methodName->GetBuffer());
    StackScratchBuffer utf8Name;
    LPCUTF8 szNameStr = wszName.GetUTF8(utf8Name);

    // pick a proper compare function
    typedef int (__cdecl *UTF8StringCompareFuncPtr)(const char *, const char *);
    UTF8StringCompareFuncPtr StrCompFunc = (flags & DBF_CaselessMatching) ? stricmpUTF8 : strcmp;
    
    // search the type hierarchy
    MethodTable *pMTOrig = methodType.GetMethodTable()->GetCanonicalMethodTable();
    for (MethodTable *pMT = pMTOrig; pMT != NULL; pMT = pMT->GetParentMethodTable())
    {
        Module * curModule = NULL;
        MethodTable * curMethodMT = NULL;
        
        MethodTable::MethodIterator it(pMT);
        it.MoveToEnd();
        for (; it.IsValid() && (pMT == pMTOrig || !it.IsVirtual()); it.Prev())
        {
            MethodDesc *pCurMethod = it.GetDeclMethodDesc();
            
            // We can't match generic methods (since no instantiation information has been provided).
            if (pCurMethod->IsGenericMethodDefinition())
                continue;

            //  update the current module if needed, so we won't fetch it over and over again
            MethodTable * methodMT = pCurMethod->GetMethodDescChunk()->GetMethodTable();
            if ( curMethodMT != methodMT )
            {
                curMethodMT = methodMT;
                curModule = curMethodMT->GetModule();
            }

            // faster equivalent of pCurMethod->GetName((USHORT) it.GetSlotNumber())
            PREFIX_ASSUME(curModule != NULL);
            LPCUTF8 curMethodName = curModule->GetMDImport()->GetNameOfMethodDef( pCurMethod->GetMemberDef() );
            CONSISTENCY_CHECK( curMethodName == pCurMethod->GetName((USHORT) it.GetSlotNumber()) );
                
            if ((pCurMethod != NULL) && (StrCompFunc(szNameStr, curMethodName) == 0))
            {
                // found a matching string, get an associated if needed
                pCurMethod =
                    MethodDesc::FindOrCreateAssociatedMethodDesc(pCurMethod,
                                                                 methodType.GetMethodTable(),
                                                                 (!pCurMethod->IsStatic() && pCurMethod->GetMethodTable()->IsValueType() && !pCurMethod->IsUnboxingStub()), 
                                                                 pCurMethod->GetNumGenericMethodArgs(),
                                                                 pCurMethod->GetMethodInstantiation(),
                                                                 false /* do not allow code with a shared-code calling convention to be returned */ );
                BOOL fIsOpenDelegate;
                if (COMDelegate::IsMethodDescCompatible((gc.target == NULL) ? TypeHandle() : gc.target->GetTrueTypeHandle(), 
                                                        methodType, 
                                                        pCurMethod, 
                                                        gc.refThis->GetTypeHandle(), 
                                                        pInvokeMeth,
                                                        flags,
                                                        &fIsOpenDelegate))
                {
                    // Initialize the delegate to point to the target method.
                    BindToMethod(&gc.refThis,
                                 &gc.target,
                                 pCurMethod,
                                 (MethodTable*)methodTH,
                                 fIsOpenDelegate,
                                 TRUE);

                    pMatchingMethod = pCurMethod;
                    goto done;
                }
            }
        }
    }
    done:
        ;
    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(pMatchingMethod != NULL);
}
FCIMPLEND


FCIMPL5(FC_BOOL_RET, COMDelegate::BindToMethodInfo, Object* refThisUNSAFE, Object* targetUNSAFE, MethodDesc *method, void *methodType, int flags)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    BOOL result = TRUE;

    struct _gc
    {
        DELEGATEREF refThis;
        OBJECTREF refFirstArg;
    } gc;

    gc.refThis          = (DELEGATEREF) ObjectToOBJECTREF(refThisUNSAFE);
    gc.refFirstArg      = ObjectToOBJECTREF(targetUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc);
    MethodTable *pMethMT = TypeHandle::FromPtr(methodType).GetMethodTable();

    _ASSERTE(gc.refThis != gc.refFirstArg);

    // A generic method had better be instantiated (we can't dispatch to an uninstantiated one).
    if (method->IsGenericMethodDefinition())
        COMPlusThrow(kArgumentException, L"Arg_DlgtTargMeth");

    // get the invoke of the delegate
    TypeHandle delegateType = gc.refThis->GetTypeHandle();
    MethodDesc* pInvokeMeth = COMDelegate::FindDelegateInvokeMethod(delegateType.GetMethodTable());
    _ASSERTE(pInvokeMeth);

    method =
        MethodDesc::FindOrCreateAssociatedMethodDesc(method,
                                                     pMethMT,
                                                     (!method->IsStatic() && pMethMT->IsValueType() && !method->IsUnboxingStub()), 
                                                     method->GetNumGenericMethodArgs(),
                                                     method->GetMethodInstantiation(),
                                                     false /* do not allow code with a shared-code calling convention to be returned */ );

    BOOL fIsOpenDelegate;
    if (COMDelegate::IsMethodDescCompatible((gc.refFirstArg == NULL) ? TypeHandle() : gc.refFirstArg->GetTrueTypeHandle(), 
                                            TypeHandle(pMethMT), 
                                            method, 
                                            gc.refThis->GetTypeHandle(), 
                                            pInvokeMeth,
                                            flags,
                                            &fIsOpenDelegate))
    {
        // Initialize the delegate to point to the target method.
        BindToMethod(&gc.refThis,
                     &gc.refFirstArg,
                     method,
                     pMethMT,
                     fIsOpenDelegate,
                     !(flags & DBF_SkipSecurityChecks));
    }
    else
        result = FALSE;

    HELPER_METHOD_FRAME_END();

    FC_RETURN_BOOL(result);
}
FCIMPLEND

// This method is called (in the late bound case only) once a target method has been decided on. All the consistency checks
// (signature matching etc.) have been done at this point and the only major reason we could fail now is on security grounds
// (someone trying to create a delegate over a method that's not visible to them for instance). This method will initialize the
// delegate (wrapping it in a secure delegate if necessary). Upon return the delegate should be ready for invocation.
void COMDelegate::BindToMethod(DELEGATEREF   *pRefThis,
                               OBJECTREF     *pRefFirstArg,
                               MethodDesc    *pTargetMethod,
                               MethodTable   *pExactMethodType,
                               BOOL           fIsOpenDelegate,
                               BOOL           fCheckSecurity)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pRefThis));
        PRECONDITION(CheckPointer(pRefFirstArg, NULL_OK));
        PRECONDITION(CheckPointer(pTargetMethod));
        PRECONDITION(CheckPointer(pExactMethodType));
    }
    CONTRACTL_END;
    
    // We might have to wrap the delegate in a secure delegate depending on the location of the target method. The following local
    // keeps track of the real (i.e. non-secure) delegate whether or not this is required.
    DELEGATEREF refRealDelegate = NULL;
    GCPROTECT_BEGIN(refRealDelegate);

    // Security checks (i.e. whether the creator of the delegate is allowed to access the target method) are the norm. They are only
    // disabled when we are used by deserialization to recreate an existing delegate instance, where such checks are unwarranted.
    if (fCheckSecurity)
    {
        RefSecContext sCtx;

        // Check visibility of the target method.
        InvokeUtil::CheckAccess(&sCtx,
                                pTargetMethod->GetAttrs(),
                                pExactMethodType,
                                NULL,
                                pTargetMethod,
                                REFSEC_CHECK_MEMBERACCESS|REFSEC_THROW_MEMBERACCESS);

        // Trip any link demands the target method requires.
        InvokeUtil::CheckLinktimeDemand(&sCtx,
                                        pTargetMethod,
                                        true);

        // Ask for skip verification if a delegate over a .ctor or .cctor is requested.
        if (pTargetMethod->IsCtor() || pTargetMethod->IsStaticInitMethod()) 
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);

        // Check if it's a COM object and if so, demand unmanaged code permission.
        if (pTargetMethod && pTargetMethod->GetMethodTable()->IsComObjectType())
            Security::SpecialDemand(SSWT_DEMAND_FROM_NATIVE, SECURITY_UNMANAGED_CODE);

        // Check whether the creator of the delegate lives in the same assembly as the target method. If not, and they aren't fully
        // trusted, we have to make this delegate a secure wrapper and allocate a new inner delegate to represent the real target.
        MethodTable *pCreatorMT = sCtx.GetCallerMT();
        Assembly *pCreatorAssembly = (pCreatorMT != NULL) ? pCreatorMT->GetAssembly() : NULL;
        if (NeedsSecureDelegate(pCreatorAssembly, sCtx.GetCallerDomain(), pTargetMethod))
            refRealDelegate = CreateSecureDelegate(*pRefThis, pCreatorAssembly, pTargetMethod);
    }

    // If we didn't wrap the real delegate in a secure delegate then the real delegate is the one passed in.
    if (refRealDelegate == NULL)
        refRealDelegate = *pRefThis;

    pTargetMethod->EnsureActive();

    if (fIsOpenDelegate) 
    {
        _ASSERTE(pRefFirstArg == NULL || *pRefFirstArg == NULL);

        // Open delegates use themselves as the target (which handily allows their shuffle thunks to locate additional data at
        // invocation time).
        refRealDelegate->SetTarget(refRealDelegate);

        // We need to shuffle arguments for open delegates since the first argument on the calling side is not meaningful to the
        // callee.
        DelegateEEClass *pDelegateClass = (DelegateEEClass*)(*pRefThis)->GetMethodTable()->GetClass();
        Stub *pShuffleThunk = NULL;

        // Look for a thunk cached on the delegate class first. Note we need a different thunk for instance methods with a
        // hidden return buffer argument because the extra argument switches place with the target when coming from the caller.
        if (!pTargetMethod->IsStatic() && pTargetMethod->HasRetBuffArg()) 
            pShuffleThunk = pDelegateClass->m_pInstRetBuffCallStub;
        else
            pShuffleThunk = pDelegateClass->m_pStaticCallStub;

        // If we haven't already setup a shuffle thunk go do it now (which will cache the result automatically).
        if (!pShuffleThunk)
            pShuffleThunk = SetupShuffleThunk(pDelegateClass, pTargetMethod);

        // Indicate that the delegate will jump to the shuffle thunk rather than directly to the target method.
        refRealDelegate->SetMethodPtr((void*)(pShuffleThunk->GetEntryPoint()));
            
        // Use stub dispatch for all virtuals.
        if (pTargetMethod->IsVirtual()) 
        {
            // Since this is an open delegate over a virtual method we cannot virtualize the call target now. So the shuffle thunk
            // needs to jump to another stub (this time provided by the VirtualStubManager) that will virtualize the call at
            // runtime.
            BYTE *pTargetCall = GetVirtualCallStub(pTargetMethod, TypeHandle(pExactMethodType));
            refRealDelegate->SetMethodPtrAux((void *)pTargetCall);
            refRealDelegate->SetInvocationCount((INT_PTR)(void *)pTargetMethod);
        }
        else
        {
            // Reflection or the code in BindToMethodName will pass us the unboxing stub for non-static methods on value types. But
            // for open invocation on value type methods the actual reference will be passed so we need the unboxed method desc
            // instead.
            if (pTargetMethod->IsUnboxingStub())
                pTargetMethod = pTargetMethod->GetWrappedMethodDesc();
            refRealDelegate->SetMethodPtrAux((void *)pTargetMethod->GetMultiCallableAddrOfCode());
        }
    }
    else
    {
        void *pTargetCode = NULL;

        // For virtual methods we can (and should) virtualize the call now (so we don't have to insert a thunk to do so at runtime).
        if (pTargetMethod->IsVirtual() &&
            *pRefFirstArg != NULL &&
            pTargetMethod->GetMethodTable() != (*pRefFirstArg)->GetMethodTable())
            pTargetCode = (void*)pTargetMethod->GetMultiCallableAddrOfVirtualizedCode(pRefFirstArg, pTargetMethod->GetMethodTable());
        else
#ifdef HAS_THISPTR_RETBUF_PRECODE
        if (pTargetMethod->IsStatic() && pTargetMethod->HasRetBuffArg())
            pTargetCode = pTargetMethod->GetDomainForAllocation()->GetFuncPtrStubs()->GetFuncPtrStub(pTargetMethod, PRECODE_THISPTR_RETBUF);
        else
#endif // HAS_THISPTR_RETBUF_PRECODE
            pTargetCode = (void*)pTargetMethod->GetMultiCallableAddrOfCode();
        _ASSERTE(pTargetCode);

        refRealDelegate->SetTarget(*pRefFirstArg);
        refRealDelegate->SetMethodPtr(pTargetCode);
    }

    GCPROTECT_END();
}

// Marshals a delegate to a unmanaged callback.
LPVOID COMDelegate::ConvertToCallback(OBJECTREF pDelegateObj)
{
    CONTRACTL
    { 
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    if (!pDelegateObj) 
        return NULL;

    DELEGATEREF pDelegate = (DELEGATEREF) pDelegateObj;

    LPVOID pCode;
    GCPROTECT_BEGIN(pDelegate);

    MethodTable* pMT = pDelegate->GetMethodTable();
    DelegateEEClass* pClass = (DelegateEEClass*)(pMT->GetClass());

    if (pMT->HasInstantiation())
        COMPlusThrowArgumentException(L"delegate", L"Argument_NeedNonGenericType");

    // If we are a delegate originally created from an unmanaged function pointer, we will simply return 
    // that function pointer.
    if (DELEGATE_MARKER_UNMANAGEDFPTR == pDelegate->GetInvocationCount())
    {
        pCode = pDelegate->GetMethodPtrAux();
    }
    else
    {
        UMEntryThunk*   pUMEntryThunk   = NULL;
        SyncBlock*      pSyncBlock      = pDelegate->GetSyncBlockSpecial();
        if (!pSyncBlock)
            ThrowOutOfMemory();
            
        InteropSyncBlockInfo* pInteropInfo = pSyncBlock->GetInteropInfo();
        
        pUMEntryThunk = (UMEntryThunk*)pInteropInfo->GetUMEntryThunk();

        if (!pUMEntryThunk) 
        {

            UMThunkMarshInfo *pUMThunkMarshInfo = pClass->m_pUMThunkMarshInfo;
            MethodDesc *pInvokeMeth = FindDelegateInvokeMethod(pMT);

            if (!pUMThunkMarshInfo) 
            {
                pUMThunkMarshInfo = new UMThunkMarshInfo();
                pUMThunkMarshInfo->LoadTimeInit(pInvokeMeth);
                pUMThunkMarshInfo->RunTimeInit();

                g_IBCLogger.LogEEClassCOWTableAccess(pClass);
                if (FastInterlockCompareExchangePointer((PVOID*)&(pClass->m_pUMThunkMarshInfo),
                                                    pUMThunkMarshInfo,
                                                    NULL ) != NULL)
                {
                    delete pUMThunkMarshInfo;
                    pUMThunkMarshInfo = pClass->m_pUMThunkMarshInfo;
                }
            }

            _ASSERTE(pUMThunkMarshInfo != NULL);
            _ASSERTE(pUMThunkMarshInfo == pClass->m_pUMThunkMarshInfo);

            pUMEntryThunk = UMEntryThunk::CreateUMEntryThunk();
            Holder<UMEntryThunk *, DoNothing, UMEntryThunk::FreeUMEntryThunk> umHolder;
            umHolder.Assign(pUMEntryThunk);

            // multicast. go thru Invoke
            OBJECTHANDLE objhnd = GetAppDomain()->CreateLongWeakHandle(pDelegate);
            _ASSERTE(objhnd != NULL);

            // MethodDesc is passed in for profiling to know the method desc of target
            pUMEntryThunk->CompleteInit(
                NULL,
                objhnd,
                pUMThunkMarshInfo, pInvokeMeth,
                GetAppDomain()->GetId());
            
           
            if (!pInteropInfo->SetUMEntryThunk(pUMEntryThunk)) 
            {
                pUMEntryThunk = (UMEntryThunk*)pInteropInfo->GetUMEntryThunk();
            }
            else
            {
                umHolder.SuppressRelease();
                // Insert the delegate handle / UMEntryThunk* into the hash
                LPVOID key = (LPVOID)pUMEntryThunk;

                // Assert that the entry isn't already in the hash.
                _ASSERTE((LPVOID)INVALIDENTRY == COMDelegate::s_pDelegateToFPtrHash->LookupValue((UPTR)key, 0));
                
                {
                    CrstHolder ch(&COMDelegate::s_DelegateToFPtrHashCrst);               
                    COMDelegate::s_pDelegateToFPtrHash->InsertValue((UPTR)key, pUMEntryThunk->GetObjectHandle());
                }
            }
            
            _ASSERTE(pUMEntryThunk != NULL);
            _ASSERTE(pUMEntryThunk == (UMEntryThunk*)pInteropInfo->GetUMEntryThunk()); 

        }
        pCode = (LPVOID)pUMEntryThunk->GetCode();
    }
    
    GCPROTECT_END();
    return pCode;
}

// Marshals an unmanaged callback to Delegate
//static
OBJECTREF COMDelegate::ConvertToDelegate(LPVOID pCallback, MethodTable* pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (!pCallback)
    {
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Check if this callback was originally a managed method passed out to unmanaged code.
    //

    UMEntryThunk* pUMEntryThunk = NULL;

    {
        pUMEntryThunk = UMEntryThunk::Decode(pCallback);
    }

    // Lookup the callsite in the hash, if found, we can map this call back to its managed function.
    // Otherwise, we'll treat this as an unmanaged callsite.
    LPVOID DelegateHnd = (pUMEntryThunk != NULL)
        ? COMDelegate::s_pDelegateToFPtrHash->LookupValue((UPTR)pUMEntryThunk, 0)
        : (LPVOID)INVALIDENTRY;

    if (DelegateHnd != (LPVOID)INVALIDENTRY)
    {
        // Found a managed callsite
        OBJECTREF pDelegate = NULL;
        GCPROTECT_BEGIN(pDelegate);

        pDelegate = ObjectFromHandle((OBJECTHANDLE)DelegateHnd);

        // Make sure we're not trying to sneak into another domain.
        SyncBlock* pSyncBlock = pDelegate->GetSyncBlockSpecial();
        _ASSERTE(pSyncBlock);
            
        InteropSyncBlockInfo* pInteropInfo = pSyncBlock->GetInteropInfo();
        _ASSERTE(pInteropInfo);
        
        pUMEntryThunk = (UMEntryThunk*)pInteropInfo->GetUMEntryThunk();
        _ASSERTE(pUMEntryThunk);

        if (pUMEntryThunk->GetDomainId() != GetAppDomain()->GetId())
            COMPlusThrow(kNotSupportedException, L"NotSupported_DelegateMarshalToWrongDomain");

        GCPROTECT_END();
        return pDelegate;
    }
    

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This is an unmanaged callsite. We need to create a new delegate.
    //
    // The delegate's invoke method will point to a call thunk.
    // The call thunk will internally shuffle the args, set up a DelegateTransitionFrame, marshal the args,
    //  call the UM Function located at m_pAuxField, unmarshal the args, and return.
    // Invoke -> CallThunk -> ShuffleThunk -> Frame -> Marshal -> Call AuxField -> UnMarshal
    
    DelegateEEClass*    pClass      = (DelegateEEClass*)pMT->GetClass();
    MethodDesc*         pMD         = FindDelegateInvokeMethod(pMT);
    LPVOID              pCode       = NULL;

    PREFIX_ASSUME(pClass != NULL);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Get or create the marshaling stub information
    //

    Stub* pMLStub = pClass->m_pMLStub;
    if (pMLStub == NULL)
    {
        
        // Create a new marshaling stub
        Stub* pMarshalingStub = ComputeDllImportStub(pMD);

        // Link it on the loader heap for X86
        CPUSTUBLINKER sltemp;
        sltemp.EmitBytes(pMarshalingStub->GetEntryPoint(), sizeof(MLHeader));
        MLHeader* pMLTemp = (MLHeader*) pMarshalingStub->GetEntryPoint();
        sltemp.EmitBytes(pMLTemp->GetMLCode(), MLStreamLength((const UINT8 *)(pMLTemp->GetMLCode())));
        pMLStub = sltemp.Link(pClass->GetStubHeap());
        pMarshalingStub->DecRef();
        

        // Save this new stub on the DelegateEEClass.       
        if (FastInterlockCompareExchangePointer( (void*volatile*) &(pClass->m_pMLStub), pMLStub, NULL ) != NULL)
        {
            pMLStub->DecRef();
            pMLStub = pClass->m_pMLStub;
        }
    }

    Stub* pCallStub = NULL;

#ifdef _X86_

    ////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    // Get or create our call thunk.  This will live on the loader heap as it is a per-type data structure.
    //

    pCallStub = pClass->m_pUMCallStub;
    if (pCallStub == NULL)
    {
        // Create a new call stub
        CPUSTUBLINKER sl;
        MLHeader* pMLHeader = (MLHeader*) pMLStub->GetEntryPoint();
        pCallStub = CreateGenericDllImportStubForDelegate(&sl, pMLHeader->m_cbStackPop, pClass);      

        // Save this new stub on the DelegateEEClass
        if (FastInterlockCompareExchangePointer( (void*volatile*) &(pClass->m_pUMCallStub), pCallStub, NULL) != NULL)
        {
            pCallStub->DecRef();
            pCallStub = pClass->m_pUMCallStub; 
        }
    }

    pCode = (LPVOID)pCallStub->GetEntryPoint();
    
#else // !_X86_

    pCallStub = pMLStub;

    LPVOID pILStubCode;
    pILStubCode = (LPVOID)pMLStub->GetEntryPoint();


    pCode = 0;
    _ASSERTE(!"NYI -- fn ptr to delegate marshaling");

#endif // !_X86_

    _ASSERTE(CheckPointer(pCode));
    _ASSERTE(CheckPointer(pCallStub));

    //
    // Wrap the pinvoke stub with a security stub if security is not
    // suppressed for the delegate class.
    //
    //

    Stub *pSecurityStub = pClass->m_pSecurityStub;

    if (!pSecurityStub &&
        Security::IsSecurityOn() &&
        pClass->GetMDImport()->GetCustomAttributeByName(pClass->GetCl(),
                                                 COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                 NULL,
                                                 NULL) == S_FALSE)
    {
        // Add the security stub
        LOG((LF_CORDB, LL_INFO10000,
             "::PSW: Placing security interceptor before delegate dllimport stub " FMT_ADDR "\n",
             DBG_ADDR(pCallStub)));

        UINT_PTR uSecurityStubToken = Security::GetSecurityStubToken(pClass->m_pInvokeMethod, DECLSEC_UNMNGD_ACCESS_DEMAND);
        DeclActionInfo *actionsNeeded = (DeclActionInfo*) uSecurityStubToken;

        if (actionsNeeded != NULL)
        {
            SecurityStubLinker sl;           
            LOG((LF_CORDB, LL_INFO10000, "ConvertToDelegate (Security): Real Stub 0x%x\n", pCallStub));

#ifdef _X86_
            // Initialize the EAX with MD explicitly as we won't have a precode doing it for us.
            sl.X86EmitRegLoad(kEAX, (UINT_PTR)pMD);
#endif // _X86_

            sl.EmitSecurityInterceptorStub(pMD, FALSE, pCode, actionsNeeded);

            // The security stub is built per-Delegate type
            _ASSERTE(pMD->GetDomain()->GetStubHeap() == pClass->GetStubHeap());
            pSecurityStub = sl.LinkInterceptor(pClass->GetStubHeap(), pCallStub, pCode);
          
            LOG((LF_CORDB, LL_INFO10000,
                 "::PSW special security interceptor stub " FMT_ADDR "\n", DBG_ADDR(pSecurityStub)));

// The security stub cannot be cached on IA64 because pCode/FP field is unique
// for each delegate instance.
            // Save this new security stub on the DelegateEEClass
            if (FastInterlockCompareExchangePointer( (void*volatile*) &(pClass->m_pSecurityStub), pSecurityStub, NULL) != NULL)
            {
                // We must clear the intercepted stub pointer in the security stub which we are going to delete.
                // Otherwise, calling DecRef() below will cause us to delete the intercepted stub as well.  
                // See Stub::DecRef() and InterceptStub::ReleaseInterceptedStub().
                *(((InterceptStub*)pSecurityStub)->GetInterceptedStub()) = NULL;

                pSecurityStub->DecRef();
                pSecurityStub = pClass->m_pSecurityStub; 
            }
        }
    }

    if (pSecurityStub)
        pCode = (LPVOID)pSecurityStub->GetEntryPoint();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    // Wire up the stubs to the new delegate instance.
    // 
    
    LOG((LF_INTEROP, LL_INFO10000, "Created delegate for function pointer: entrypoint: %p\n", pCode));

    // Create the new delegate
    DELEGATEREF delObj = (DELEGATEREF) pMT->Allocate();
    
    // delObj is not protected
    GCX_NOTRIGGER();
    
    // Wire up the unmanaged call stub to the delegate.
    delObj->SetTarget(delObj);              // We are the "this" object
    
    // For X86, we save the entry point in the delegate's method pointer and the UM Callsite in the aux pointer.
    delObj->SetMethodPtr(pCode);
    delObj->SetMethodPtrAux(pCallback);

    // Also, mark this delegate as an unmanaged function pointer wrapper.
    delObj->SetInvocationCount(DELEGATE_MARKER_UNMANAGEDFPTR);

    return delObj;
}



// Called from the constructor. The method must be a static method.
FCIMPL1(void*, COMDelegate::GetUnmanagedCallSite, Object* refThisUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(refThisUNSAFE != NULL);
    }
    CONTRACTL_END;

    LPVOID callsite = NULL;

    DELEGATEREF refThis = (DELEGATEREF) ObjectToOBJECTREF(refThisUNSAFE);

    // Make sure this is an unmanaged function pointer wrapped in a delegate.
    // This decodes the state stored by COMDelegate::ConvertToDelegate.
    INT_PTR check = refThis->GetInvocationCount();
    if (check == DELEGATE_MARKER_UNMANAGEDFPTR)
    {
        callsite = refThis->GetMethodPtrAux();
    }
    
    return callsite;
}
FCIMPLEND




void COMDelegate::RemoveEntryFromFPtrHash(UPTR key)
{
    WRAPPER_CONTRACT;
    
    // Remove this entry from the lookup hash.
    CrstHolder ch(&COMDelegate::s_DelegateToFPtrHashCrst);
    COMDelegate::s_pDelegateToFPtrHash->DeleteValue(key, NULL);
}

void COMDelegate::ValidateDelegatePInvoke(MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;
    
    if (pMD->IsSynchronized())
        COMPlusThrow(kTypeLoadException, IDS_EE_NOSYNCHRONIZED);

    if (pMD->MethodDesc::IsVarArg())
        COMPlusThrow(kNotSupportedException, IDS_EE_VARARG_NOT_SUPPORTED);
}


// Creates the ML marshaling stub for a delegate -> unmanaged call
Stub* COMDelegate::ComputeDllImportStub(MethodDesc* pMD)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pMD));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    ValidateDelegatePInvoke(pMD);

    PInvokeStaticSigInfo sigInfo(pMD);
    Stub *pMLStub = NULL;
    DWORD dwStubFlags = NDIRECTSTUB_FL_DELEGATE;

    pMLStub = CreateNDirectStub(&sigInfo, dwStubFlags, pMD, NULL
                                  DEBUG_ARG(pMD->m_pszDebugMethodName)
                                  DEBUG_ARG(pMD->m_pszDebugClassName)
                                  DEBUG_ARG(NULL)
                                  );

    RETURN pMLStub;
}


FCIMPL2(void*, COMDelegate::GetCallStub, Object* refThisUNSAFE, SLOT method)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    void* target = NULL;

    DELEGATEREF refThis = (DELEGATEREF)ObjectToOBJECTREF(refThisUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_RET_1(refThis);
    MethodDesc *pMeth = MethodTable::GetUnknownMethodDescForSlotAddress(method);
    _ASSERTE(pMeth);
    _ASSERTE(!pMeth->IsStatic() && pMeth->IsVirtual());
    target = GetVirtualCallStub(pMeth, TypeHandle(pMeth->GetMethodTable()));
    refThis->SetInvocationCount((INT_PTR)(void*)pMeth);
    HELPER_METHOD_FRAME_END();
    return target;
}
FCIMPLEND

FCIMPL3(void*, COMDelegate::AdjustTarget, Object* refThisUNSAFE, Object* targetUNSAFE, SLOT method)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (targetUNSAFE == NULL)
        FCThrow(kNullReferenceException);
    
    OBJECTREF refThis = ObjectToOBJECTREF(refThisUNSAFE);
    OBJECTREF target  = ObjectToOBJECTREF(targetUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_2(refThis, target);

    _ASSERTE(refThis);
    _ASSERTE(method);
    
    if (target == NULL)
    {
        COMPlusThrow(kArgumentException, L"Arg_DlgtNullInst");
    }
    
    MethodTable *pMT = target->GetMethodTable();
    MethodTable *pRealMT = target->GetTrueMethodTable();
    _ASSERTE((NULL == pMT) || pMT->IsTransparentProxyType() || !pRealMT->IsContextful());

    MethodDesc *pMeth = Entry2MethodDesc((BYTE*)method, pRealMT);
    _ASSERTE(pMeth);
    _ASSERTE(!pMeth->IsStatic());

    MethodTable* pDelMT = refThis->GetMethodTable();
    
    // In non-stub dispatch, System.Delegate implements interfaces and so needs restoring.
    pDelMT->CheckRestore();

    // Make sure we call the <cinit>
    pDelMT->EnsureInstanceActive();
    pDelMT->CheckRunClassInitThrowing();

    // close delegates
    MethodTable* pMTTarg = target->GetMethodTable();
    MethodTable* pMTMeth = pMeth->GetMethodTable();

    BOOL isComObject = pMTTarg->IsComObjectType();
    if (isComObject)
        DoUnmanagedCodeAccessCheck(pMeth);
    
    if (!pMT->IsThunking())
    {
        MethodDesc *pCorrectedMethod = pMeth;

        if (pMTMeth != pMTTarg)
        {
            //They cast to an interface before creating the delegate, so we now need 
            //to figure out where this actually lives before we continue.
            if (pCorrectedMethod->IsInterface())
            {
                // No need to resolve the interface based method desc to a class based
                // one for COM objects because we invoke directly thru the interface MT.
                if (!isComObject)
                {
                    // 
                    pCorrectedMethod = pMTTarg->FindDispatchSlotForInterfaceMD(pCorrectedMethod).GetMethodDesc();
                    _ASSERTE(pCorrectedMethod != NULL);
                }
            }
        }

        g_IBCLogger.LogEEClassAndMethodTableAccess(pMTTarg->GetClass());

        // Use the Unboxing stub for value class methods, since the value
        // class is constructed using the boxed instance.
        if (pMTTarg->IsValueType() && !pCorrectedMethod->IsUnboxingStub())
        {
            // those should have been ruled out at jit time (GetDelegateConstructor)
            _ASSERTE((pMTMeth != g_pValueTypeClass) && (pMTMeth != g_pObjectClass));
            pCorrectedMethod->CheckRestore();
            pCorrectedMethod = pMTTarg->GetBoxedEntryPointMD(pCorrectedMethod);
            _ASSERTE(pCorrectedMethod != NULL);
        }
        
        if (pMeth != pCorrectedMethod)
        {
            method = (BYTE*)pCorrectedMethod->GetMultiCallableAddrOfCode();
        }
    }
    HELPER_METHOD_FRAME_END();

    return method;
}
FCIMPLEND


// This is the single constructor for all Delegates.  The compiler
//  doesn't provide an implementation of the Delegate constructor.  We
//  provide that implementation through an ECall call to this method.
FCIMPL3(void, COMDelegate::DelegateConstruct, Object* refThisUNSAFE, Object* targetUNSAFE, SLOT method)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    }
    CONTRACTL_END;
    
    struct _gc
    {
        DELEGATEREF refThis;
        OBJECTREF target;
    } gc;

    gc.refThis = (DELEGATEREF) ObjectToOBJECTREF(refThisUNSAFE);
    gc.target  = (OBJECTREF) targetUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);

    MethodDesc *pCaller = NULL;
    AppDomain *pCallerDomain = NULL;
    Assembly *pCreatorAssembly = NULL;
    if (pCaller)
    {
        pCreatorAssembly = pCaller->GetAssembly();
    }

    //                         programmers could feed garbage data to DelegateConstruct().
    // It's difficult to validate a method code pointer, but at least we'll
    // try to catch the easy garbage.
    _ASSERTE(isMemoryReadable((TADDR)method, 1));

    _ASSERTE(gc.refThis);
    _ASSERTE(method);
    
    MethodTable *pMTTarg = NULL;
    MethodTable *pRealMT = NULL;

    if (gc.target != NULL)
    {
        pMTTarg = gc.target->GetMethodTable();
        pRealMT = gc.target->GetTrueMethodTable();
    }

    MethodDesc *pMethOrig = Entry2MethodDesc((BYTE*)method, pRealMT);
    MethodDesc *pMeth = pMethOrig;

    //
    // If target is a contextful class, then it must be a proxy
    //    
    _ASSERTE((NULL == pMTTarg) || pMTTarg->IsTransparentProxyType() || !pRealMT->IsContextful());

    MethodTable* pDelMT = gc.refThis->GetMethodTable();
    
    // In non-stub dispatch, System.Delegate implements interfaces and so needs restoring.
    pDelMT->CheckRestore();

    // Make sure we call the <cinit>
    pDelMT->EnsureInstanceActive();
    pDelMT->CheckRunClassInitThrowing();

    LOG((LF_STUBS, LL_INFO1000, "In DelegateConstruct: for delegate type %s binding to method %s::%s%s, static = %d\n", 
            pDelMT->GetDebugClassName(),
            pMeth->m_pszDebugClassName, pMeth->m_pszDebugMethodName, pMeth->m_pszDebugMethodSignature, pMeth->IsStatic()));

    _ASSERTE(pMeth);

#ifdef _DEBUG
    // Assert that everything is OK...This is not some bogus
    //  address...Very unlikely that the code below would work
    //  for a random address in memory....
    MethodTable* p = pMeth->GetMethodTable();
    _ASSERTE(p);
    _ASSERTE(p->ValidateWithPossibleAV());
#endif // _DEBUG

    if (Nullable::IsNullableType(pMeth->GetMethodTable()))
        COMPlusThrow(kNotSupportedException); 

#define ASSERT_DELEGATE_OFFSET_CORRECT(fieldGetter, constant, managedFieldName)                                     \
    IA64_ONLY(CONSISTENCY_CHECK_MSGF(DelegateObject::GetOffsetOf##fieldGetter() == (constant),                        \
        ("The managed layout of System.Delegate changed!  We found the " managedFieldName " field at offset 0x%x, " \
         "but we expected it at offset 0x%x.  Please update the "#constant" constant "                              \
         "in vm\\ia64\\asmconstants.h", DelegateObject::GetOffsetOf##fieldGetter(), (constant))));  

    ASSERT_DELEGATE_OFFSET_CORRECT(Target,          DELEGATE_FIELD_OFFSET__TARGET,            "_target");
    ASSERT_DELEGATE_OFFSET_CORRECT(MethodPtr,       DELEGATE_FIELD_OFFSET__METHOD,            "_method");
    ASSERT_DELEGATE_OFFSET_CORRECT(MethodPtrAux,    DELEGATE_FIELD_OFFSET__METHOD_AUX,        "_methodPtrAux");
    ASSERT_DELEGATE_OFFSET_CORRECT(InvocationList,  DELEGATE_FIELD_OFFSET__INVOCATION_LIST,   "_invocationList");
    ASSERT_DELEGATE_OFFSET_CORRECT(InvocationCount, DELEGATE_FIELD_OFFSET__INVOCATION_COUNT,  "_invocationCount");

    DelegateEEClass *pDelCls = (DelegateEEClass*)pDelMT->GetClass();    
    MethodDesc *pDelegateInvoke = COMDelegate::FindDelegateInvokeMethod(pDelMT);

    MetaSig invokeSig(pDelegateInvoke);
    MetaSig methodSig(pMeth);
    UINT invokeArgCount = invokeSig.NumFixedArgs();
    UINT methodArgCount = methodSig.NumFixedArgs();
    BOOL isStatic = pMeth->IsStatic();
    if (!isStatic) 
    {
        methodArgCount++; // count 'this'
    }

    // do we need a secure delegate?
    gc.refThis = NeedsSecureDelegate(pCreatorAssembly, pCallerDomain, pMeth) ?
            CreateSecureDelegate(gc.refThis, pCreatorAssembly, pMeth) : gc.refThis;

    
    // Open delegates.
    if (invokeArgCount == methodArgCount) 
    {
        // set the target
        gc.refThis->SetTarget(gc.refThis);

        g_IBCLogger.LogEEClassAndMethodTableAccess(pDelCls);

        // set the shuffle thunk
        Stub *pShuffleThunk = NULL;
        if (!pMeth->IsStatic() && pMeth->HasRetBuffArg()) 
            pShuffleThunk = pDelCls->m_pInstRetBuffCallStub;
        else
            pShuffleThunk = pDelCls->m_pStaticCallStub;
        if (!pShuffleThunk) 
            pShuffleThunk = SetupShuffleThunk(pDelCls, pMeth);
        
        if (pMTTarg)
            g_IBCLogger.LogEEClassAndMethodTableAccess(pMTTarg->GetClass());

        gc.refThis->SetMethodPtr((void*)pShuffleThunk->GetEntryPoint());

        // set the ptr aux according to what is needed, if virtual need to call make virtual stub dispatch
        if (!pMeth->IsStatic() && pMeth->IsVirtual())
        {
            BYTE *pTargetCall = GetVirtualCallStub(pMeth, TypeHandle(pMeth->GetMethodTable()));
            gc.refThis->SetMethodPtrAux((void *)pTargetCall);
            gc.refThis->SetInvocationCount((INT_PTR)(void *)pMeth);
        }
        else
        {
            gc.refThis->SetMethodPtrAux(method);
        }

    }
    else 
    {
        MethodTable* pMTMeth = pMeth->GetMethodTable();

        if (!pMeth->IsStatic())
        {
            if (pMTTarg)
            {
                // We can skip the demand if SuppressUnmanagedCodePermission is present on the class,
                //  or in the case where we are setting up a delegate for a COM event sink
                //   we can skip the check if the source interface is defined in fully trusted code
                //   we can skip the check if the source interface is a disp-only interface
                BOOL isComObject = pMTTarg->IsComObjectType();
                if (isComObject)
                    DoUnmanagedCodeAccessCheck(pMeth);
            
                if (!pMTTarg->IsThunking())
                {
                    if (pMTMeth != pMTTarg)
                    {
                        // They cast to an interface before creating the delegate, so we now need 
                        // to figure out where this actually lives before we continue.
                        if (pMeth->IsInterface())
                        {
                            // No need to resolve the interface based method desc to a class based
                            // one for COM objects because we invoke directly thru the interface MT.
                            if (!isComObject)
                            {
                                // 
                                pMeth = pMTTarg->FindDispatchSlotForInterfaceMD(pMeth).GetMethodDesc();
                                if (pMeth == NULL)
                                    COMPlusThrow(kArgumentException, L"Arg_DlgtTargMeth");
                            }
                        }
                    }

                    g_IBCLogger.LogMethodTableAccess(pMTTarg);

                    // Use the Unboxing stub for value class methods, since the value
                    // class is constructed using the boxed instance.
                    //
                    // <NICE> We could get the JIT to recognise all delegate creation sequences and
                    // ensure the thing is always an BoxedEntryPointStub anyway </NICE>

                    if (pMTMeth->IsValueType() && !pMeth->IsUnboxingStub())
                    {
                        // If these are Object/ValueType.ToString().. etc,
                        // don't need an unboxing Stub.

                        if ((pMTMeth != g_pValueTypeClass) 
                            && (pMTMeth != g_pObjectClass))
                        {
                            pMeth->CheckRestore();
                            pMeth = pMTTarg->GetBoxedEntryPointMD(pMeth);
                            _ASSERTE(pMeth != NULL);
                        }
                    }
                    // Only update the code address if we've decided to go to a different target...
                    // <NICE> We should make sure the code address that the JIT provided to us is always the right one anyway,
                    // so we don't have to do all this mucking about. </NICE>
                    if (pMeth != pMethOrig)
                    {
                        method = (BYTE*)pMeth->GetMultiCallableAddrOfCode();
                    }
                }
            }
            
            if (gc.target == NULL)
            {
                COMPlusThrow(kArgumentException, L"Arg_DlgtNullInst");
            }
        }
#ifdef HAS_THISPTR_RETBUF_PRECODE
        else if (pMeth->HasRetBuffArg())
            method = pMeth->GetDomainForAllocation()->GetFuncPtrStubs()->GetFuncPtrStub(pMeth, PRECODE_THISPTR_RETBUF);
#endif // HAS_THISPTR_RETBUF_PRECODE

        gc.refThis->SetTarget(gc.target);
        gc.refThis->SetMethodPtr(method);
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


void COMDelegate::DoUnmanagedCodeAccessCheck(MethodDesc* pMeth)
{
    MethodTable* pMTMeth = pMeth->GetMethodTable();
    
    // Skip if SuppressUnmanagedCodePermission is present
    if (pMeth->RequiresLinktimeCheck())
    {
        // Check whether this is actually a SuppressUnmanagedCodePermission attribute and
        // if so, don't do a demand
        if (pMTMeth->GetMDImport()->GetCustomAttributeByName(pMeth->GetClass()->GetCl(),
                                                             COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                             NULL,
                                                             NULL) == S_OK ||
            pMTMeth->GetMDImport()->GetCustomAttributeByName(pMeth->GetMemberDef(),
                                                             COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                             NULL,
                                                             NULL) == S_OK)
        {
            return;
        }
    }


    Security::SpecialDemand(SSWT_DEMAND_FROM_NATIVE, SECURITY_UNMANAGED_CODE);
}


MethodDesc *COMDelegate::GetMethodDesc(OBJECTREF orDelegate)
{        
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    MethodDesc *pMethodHandle = NULL;

    DELEGATEREF thisDel = (DELEGATEREF) orDelegate;
    DELEGATEREF innerDel = NULL;

    INT_PTR count = thisDel->GetInvocationCount();
    if (count != 0)
    {
        // this is one of the following:
        // - multicast - _invocationList is Array && _invocationCount != 0
        // - unamanaged ftn ptr - _invocationList == NULL && _invocationCount == -1
        // - secure delegate - _invocationList is Delegate && _invocationCount != NULL
        // - virtual delegate - _invocationList == null && _invocationCount == (target MethodDesc)
        // in the secure delegate case we want to unwrap and return the method desc of the inner delegate
        // in the other cases we return the method desc for the invoke
        innerDel = (DELEGATEREF) thisDel->GetInvocationList();
        if (innerDel != NULL) 
        {
            MethodTable *pMT = innerDel->GetMethodTable();
            if (pMT->GetClass()->IsAnyDelegateClass()) 
                return GetMethodDesc(innerDel);
        }
        else
        {
            if (count != DELEGATE_MARKER_UNMANAGEDFPTR) 
            {
                // must be a virtual one
                pMethodHandle = (MethodDesc*)thisDel->GetInvocationCount();
            }
        }
        if (pMethodHandle == NULL)
            pMethodHandle = FindDelegateInvokeMethod(thisDel->GetMethodTable());
    }
    
    if (pMethodHandle == NULL)
    {
        // Next, check for an open delegate
        void *code = thisDel->GetMethodPtrAux();
        if (code == NULL)
        {
            // Must be a normal delegate
            code = thisDel->GetMethodPtr();

            // Weird case - need to check for a prejit vtable fixup stub.
            if (StubManager::IsStub((TADDR) (const BYTE *)code))
            {
                OBJECTREF orThis = thisDel->GetTarget();
                if (orThis!=NULL)
                {
                    MethodDesc *pMD = StubManager::MethodDescFromEntry((const BYTE *) code, 
                                                                       orThis->GetTrueMethodTable());
                    if (pMD != NULL)
                        pMethodHandle = pMD;
                }
            }
        }
        
        if (pMethodHandle == NULL)
            pMethodHandle = MethodTable::GetUnknownMethodDescForSlotAddress((SLOT)code);
    }

    _ASSERTE(pMethodHandle);
    return pMethodHandle;
}
    
OBJECTREF COMDelegate::GetTargetObject(OBJECTREF obj)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    OBJECTREF targetObject = NULL;

    DELEGATEREF thisDel = (DELEGATEREF) obj; 
    OBJECTREF innerDel = NULL;

    if (thisDel->GetInvocationCount() != 0)
    {
        // this is one of the following:
        // - multicast
        // - unmanaged ftn ptr
        // - secure delegate
        // in the secure delegate case we want to unwrap and return the object of the inner delegate
        innerDel = (DELEGATEREF) thisDel->GetInvocationList();
        if (innerDel != NULL) 
        {
            MethodTable *pMT = innerDel->GetMethodTable();
            if (pMT->GetClass()->IsAnyDelegateClass()) 
            {
                targetObject = GetTargetObject(innerDel);
            }
        }
    }
    
    if (targetObject == NULL)
        targetObject = thisDel->GetTarget();

    return targetObject;
}

BOOL COMDelegate::IsTrueMulticastDelegate(OBJECTREF delegate)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    BOOL isMulticast = FALSE;

    size_t invocationCount = ((DELEGATEREF)delegate)->GetInvocationCount();
    if (invocationCount)
    {
        OBJECTREF invocationList = ((DELEGATEREF)delegate)->GetInvocationList();
        if (invocationList != NULL)
        {
            MethodTable *pMT = invocationList->GetTrueMethodTable();
            isMulticast = !(pMT->GetClass()->IsAnyDelegateClass());
        }
    }

    return isMulticast;
}


Stub *COMDelegate::TheDelegateInvokeStub(BOOL bHasReturnBuffer)
{                           
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;

#if (THISPTR_LOCATION > 0) && !defined(_AMD64_)
    if (bHasReturnBuffer)
    {
        if (s_pHasRetBuffInvokeStub == NULL)
        {
            CPUSTUBLINKER sl;
            sl.EmitDelegateInvoke(TRUE);

            // Process-wide singleton stub that never unloads
            Stub *pCandidate = sl.Link(SystemDomain::System()->GetStubHeap(), NULL, TRUE);

            if (s_pHasRetBuffInvokeStub == NULL)
            {
                if (!FastInterlockCompareExchangePointer((void**)&s_pHasRetBuffInvokeStub, *(void**)&pCandidate, NULL))
                {
                    RETURN s_pHasRetBuffInvokeStub;
                }
            }
            // if we are here someone managed to set the stub before us so we release the current
            pCandidate->DecRef();
        }

        RETURN s_pHasRetBuffInvokeStub;
    }
#endif // (THISPTR_LOCATION > 0) && !defined(_AMD64_)

    if (s_pInvokeStub == NULL)
    {
        CPUSTUBLINKER sl;
        sl.EmitDelegateInvoke();
        // Process-wide singleton stub that never unloads
        Stub *pCandidate = sl.Link(SystemDomain::System()->GetStubHeap(), NULL, TRUE);

        if (s_pInvokeStub == NULL)
        {
            if (!FastInterlockCompareExchangePointer((void**)&s_pInvokeStub, *(void**)&pCandidate, NULL))
            {
                RETURN s_pInvokeStub;
            }
        }
        // if we are here someone managed to set the stub before us so we release the current
        pCandidate->DecRef();
    }

    RETURN s_pInvokeStub;
}

// Get the cpu stub for a delegate invoke.
Stub *COMDelegate::GetInvokeMethodStub(EEImplMethodDesc* pMD)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;

    Stub*               ret = NULL;
    DelegateEEClass*    pClass = (DelegateEEClass*) pMD->GetClass();
    bool                fHasRetBuffArg = false;     // we only care if THISPTR_LOCATION > 0
    bool                fIsAsync = false;

#if THISPTR_LOCATION > 0
    MetaSig sig(pMD);
    fHasRetBuffArg = sig.HasRetBuffArg();
#endif // THISPTR_LOCATION > 0

    g_IBCLogger.LogEEClassAndMethodTableAccess(pClass);
    if (pMD == pClass->m_pInvokeMethod)
    {
        // Validate the invoke method, which at the moment just means checking the calling convention

        g_IBCLogger.LogStoredMethodDataAccess(pMD);
        if (*pMD->GetSig() != (IMAGE_CEE_CS_CALLCONV_HASTHIS | IMAGE_CEE_CS_CALLCONV_DEFAULT))
            COMPlusThrow(kInvalidProgramException);

        ret = COMDelegate::TheDelegateInvokeStub(fHasRetBuffArg);
        ret->IncRef();
    }
    else if (pMD == pClass->m_pBeginInvokeMethod)
    {
        CRemotingServices::EnsureRemotingStarted();

        if (!ValidateBeginInvoke(pClass))
            COMPlusThrow(kInvalidProgramException);

        ret = TheAsyncDelegateStub();
        ret->IncRef();

        fIsAsync = TRUE;
    }
    else if (pMD == pClass->m_pEndInvokeMethod)
    {
        CRemotingServices::EnsureRemotingStarted();

        if (!ValidateEndInvoke(pClass))
            COMPlusThrow(kInvalidProgramException);

        ret = TheAsyncDelegateStub();
        ret->IncRef();

        fIsAsync = TRUE;
    }
    else
    {
        _ASSERTE(!"Bad Delegate layout");
        COMPlusThrow(kInvalidProgramException);
    }



    RETURN ret;    
}

FCIMPL1(Object*, COMDelegate::InternalAlloc, void* target)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF refRetVal = NULL;
    TypeHandle targetTH = TypeHandle::FromPtr(target);
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);

    _ASSERTE(targetTH.GetMethodTable() != NULL && targetTH.GetMethodTable()->GetParentMethodTable()->IsMultiDelegateExact());
    
    refRetVal = targetTH.GetMethodTable()->Allocate();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(Object*, COMDelegate::InternalAllocLike, Object* pThis)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);

    _ASSERTE(pThis->GetMethodTable() != NULL && pThis->GetMethodTable()->GetParentMethodTable()->IsMultiDelegateExact());
    
    refRetVal = pThis->GetMethodTable()->AllocateNoChecks();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, COMDelegate::InternalEqualTypes, Object* pThis, Object *pThat)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(pThis->GetMethodTable() != NULL && pThis->GetMethodTable()->GetParentMethodTable()->IsMultiDelegateExact());
    _ASSERTE(pThat->GetMethodTable() != NULL);
    
    FC_RETURN_BOOL(pThis->GetMethodTable() == pThat->GetMethodTable());
}
FCIMPLEND

BOOL COMDelegate::NeedsSecureDelegate(Assembly* pCreatorAssembly, AppDomain *pCreatorDomain, MethodDesc* pTargetMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (pCreatorAssembly)
    {
        Assembly* pTargetAssembly = pTargetMD->GetAssembly();
        if (pCreatorAssembly != pTargetAssembly && !Security::IsFullyTrusted(pCreatorAssembly->GetSecurityDescriptor(pCreatorDomain)))
        {
            return TRUE;
        }
    }

    return FALSE;
}

// to create a secure delegate wrapper we need:
// - the delegate to forward to         -> _invocationList
// - the creator assembly               -> _methodAuxPtr
// - the delegate invoke MethodDesc     -> _count
// the 2 fields used for invocation will contain:
// - the delegate itself                -> _pORField
// - the secure stub                    -> _pFPField
DELEGATEREF COMDelegate::CreateSecureDelegate(DELEGATEREF delegate, Assembly* pCreatorAssembly, MethodDesc* pTargetMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

#if defined(_X86_) || defined(_AMD64_) || defined(_IA64_)
    MethodTable *pDelegateType = delegate->GetMethodTable();
    MethodDesc *pMD = ((DelegateEEClass*)(pDelegateType->GetClass()))->m_pInvokeMethod;
    // allocate the object
    struct _gc {
        DELEGATEREF refSecDel;
        DELEGATEREF innerDel;
    } gc;
    gc.refSecDel = delegate; 
    gc.innerDel = NULL;

    GCPROTECT_BEGIN(gc);

    // set the proper fields
    //

    // Object reference field... 
    gc.refSecDel->SetTarget(gc.refSecDel);       

    // save the secure invoke stub.  GetSecureInvoke() can trigger GC.
    SLOT tmp = GetSecureInvoke(pMD);
    gc.refSecDel->SetMethodPtr(tmp);       
    // save the assembly
    gc.refSecDel->SetMethodPtrAux(pCreatorAssembly);
    // save the delegate MethodDesc for the frame
    gc.refSecDel->SetInvocationCount((INT_PTR)pMD);
    
    // save the delegate to forward to
    gc.innerDel = (DELEGATEREF) pDelegateType->Allocate();
    gc.refSecDel->SetInvocationList(gc.innerDel); 


    GCPROTECT_END();

    return gc.innerDel;
#else // !(defined(_X86_) || defined(_AMD64_) || defined(_IA64_))
    return delegate;
#endif // !(defined(_X86_) || defined(_AMD64_) || defined(_IA64_))
}

// InternalGetMethodInfo
// This method will get the MethodInfo for a delegate
FCIMPL1(MethodDesc*, COMDelegate::FindMethodHandle, Object* refThisIn)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc* pMD = NULL;
    OBJECTREF refThis = ObjectToOBJECTREF(refThisIn);

    HELPER_METHOD_FRAME_BEGIN_RET_1(refThis);
    
    pMD = GetMethodDesc(refThis);
    HELPER_METHOD_FRAME_END();

    return pMD;
}
FCIMPLEND

FCIMPL1(MethodDesc*, COMDelegate::GetInvokeMethod, Object* refThisIn)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF refThis = ObjectToOBJECTREF(refThisIn);
    g_IBCLogger.LogEEClassAndMethodTableAccess(refThis->GetMethodTable()->GetClass_NoLogging());
    MethodDesc* pMD = ((DelegateEEClass*)(refThis->GetMethodTable()->GetClass_NoLogging()))->m_pInvokeMethod;
    _ASSERTE(pMD);
    return pMD;
}
FCIMPLEND

FCIMPL1(SLOT, COMDelegate::GetMulticastInvoke, Object* refThisIn)
{        
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF refThis = ObjectToOBJECTREF(refThisIn);

    DelegateEEClass *delegateEEClass = ((DelegateEEClass*)(refThis->GetMethodTable()->GetClass_NoLogging()));
    g_IBCLogger.LogEEClassAndMethodTableAccess(delegateEEClass);
    Stub *pStub = delegateEEClass->m_pMultiCastInvokeStub;
    SLOT slot = NULL;
    if (pStub == NULL)
    {
        MethodDesc* pMD = delegateEEClass->m_pInvokeMethod;
    
        HELPER_METHOD_FRAME_BEGIN_RET_0();

        CPUSTUBLINKER sl;

        g_IBCLogger.LogStoredMethodDataAccess(pMD);
        MetaSig sig(pMD);

        UINT numStackBytes = sig.SizeOfActualFixedArgStack(pMD->IsStatic());
        if (numStackBytes > 0x7FFF) 
            COMPlusThrow(kNotSupportedException, L"NotSupported_TooManyArgs");

        UINT hash = CPUSTUBLINKER::HashMulticastInvoke(numStackBytes, &sig);

        pStub = m_pMulticastStubCache->GetStub(hash);
        if (!pStub)
        {
            LOG((LF_CORDB,LL_INFO10000, "COMD::GIMS making a multicast delegate\n"));
            sl.EmitMulticastInvoke(numStackBytes, &sig);

            UINT cbSize;

            // The cache is process-wide, based on signature.  It never unloads
            Stub *pCandidate = sl.Link(SystemDomain::System()->GetStubHeap(), &cbSize, TRUE);

            Stub *pWinner = m_pMulticastStubCache->AttemptToSetStub(hash,pCandidate);
            pCandidate->DecRef();
            if (!pWinner)
                COMPlusThrowOM();

            LOG((LF_CORDB,LL_INFO10000, "Putting a MC stub at 0x%x (code:0x%x)\n",
                pWinner, (BYTE*)pWinner+sizeof(Stub)));

            pStub = pWinner;
        }
        g_IBCLogger.LogEEClassCOWTableAccess(delegateEEClass);

        // we don't need to do an InterlockedCompareExchange here - the m_pMulticastStubCache->AttemptToSetStub
        // will make sure all threads racing here will get the same stub, so they'll all store the same value
        delegateEEClass->m_pMultiCastInvokeStub = pStub;

        HELPER_METHOD_FRAME_END();
    }

    slot = (SLOT)pStub->GetEntryPoint();
    return slot;
}
FCIMPLEND


SLOT COMDelegate::GetSecureInvoke(MethodDesc* pMD)
{
    CONTRACT (SLOT)
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    CPUSTUBLINKER sl;
    SLOT slot = NULL;

    MetaSig sig(pMD);

    UINT numStackBytes = sig.SizeOfActualFixedArgStack(pMD->IsStatic());
    if (numStackBytes > 0x7FFF) 
        COMPlusThrow(kNotSupportedException, L"NotSupported_TooManyArgs");

    UINT hash = CPUSTUBLINKER::HashMulticastInvoke(numStackBytes, &sig);

    Stub *pStub = m_pSecureDelegateStubCache->GetStub(hash);
    if (!pStub)
    {
        LOG((LF_CORDB,LL_INFO10000, "COMD::GIMS making a multicast delegate\n"));
        sl.EmitSecureDelegateInvoke(numStackBytes, &sig);

        UINT cbSize;

        // The cache is process-wide, based on signature.  It never unloads
        Stub *pCandidate = sl.Link(SystemDomain::System()->GetStubHeap(), &cbSize, TRUE);

        Stub *pWinner = m_pSecureDelegateStubCache->AttemptToSetStub(hash, pCandidate);
        pCandidate->DecRef();
        if (!pWinner)
            COMPlusThrowOM();

        LOG((LF_CORDB,LL_INFO10000, "Putting a MC stub at 0x%x (code:0x%x)\n",
            pWinner, (BYTE*)pWinner+sizeof(Stub)));

        pStub = pWinner;
    }
    slot = (SLOT)pStub->GetEntryPoint();

    RETURN slot;
}

static BOOL IsLocationAssignable(TypeHandle fromHandle, TypeHandle toHandle, BOOL relaxedMatch, BOOL fromHandleIsBoxed)
{
    // Identical types are obviously compatible.
    if (fromHandle == toHandle)
        return TRUE;

    // If we allow relaxed matching then any subtype of toHandle is probably
    // compatible (definitely so if we know fromHandle is coming from a boxed
    // value such as we get from the bound argument in a closed delegate).
    if (relaxedMatch && fromHandle.CanCastTo(toHandle))
    {
        // If the fromHandle isn't boxed then we need to be careful since
        // non-object reference arguments aren't going to be compatible with
        // object reference locations (there's no implicit boxing going to happen
        // for us).
        if (!fromHandleIsBoxed)
        {
            // Check that the "objrefness" of source and destination matches. In
            // reality there are only three objref classes that would have
            // passed the CanCastTo above given a value type source (Object,
            // ValueType and Enum), but why hard code these in when we can be
            // more robust?
            if (CorTypeInfo::IsObjRef(fromHandle.GetInternalCorElementType()) !=
                CorTypeInfo::IsObjRef(toHandle.GetInternalCorElementType()))
                return FALSE;
        }

        return TRUE;
    }
    else
    {
        // they are not compatible yet enums can go into each other if their underlying element type is the same
        if (toHandle.GetVerifierCorElementType() == fromHandle.GetVerifierCorElementType()
            && (toHandle.IsEnum() || fromHandle.IsEnum()))
            return TRUE;

    }

    return FALSE;
}

MethodDesc* COMDelegate::FindDelegateInvokeMethod(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(pMT->GetClass()->IsAnyDelegateClass());
    g_IBCLogger.LogEEClassAndMethodTableAccess(pMT->GetClass_NoLogging());

    if (((DelegateEEClass*)pMT->GetClass_NoLogging())->m_pInvokeMethod == NULL)
        COMPlusThrowNonLocalized(kMissingMethodException, L"Invoke");
    return ((DelegateEEClass*)pMT->GetClass_NoLogging())->m_pInvokeMethod;
}

BOOL COMDelegate::IsMethodDescCompatible(TypeHandle   thFirstArg,
                                         TypeHandle   thExactMethodType,
                                         MethodDesc  *pTargetMethod,
                                         TypeHandle   thDelegate,
                                         MethodDesc  *pInvokeMethod,
                                         int          flags,
                                         BOOL        *pfIsOpenDelegate)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Handle easy cases first -- if there's a constraint on whether the target method is static or instance we can check that very
    // quickly.
    if (flags & DBF_StaticMethodOnly && !pTargetMethod->IsStatic())
        return FALSE;
    if (flags & DBF_InstanceMethodOnly && pTargetMethod->IsStatic())
        return FALSE;

    // we don't allow you to bind to methods on Nullable<T> because the unboxing stubs don't know how to 
    // handle this case.   
    if (!pTargetMethod->IsStatic() && Nullable::IsNullableType(pTargetMethod->GetMethodTable()))
        return FALSE;

    // Have to be careful with automatically generated array methods (Get, Set, etc.). The TypeHandle here may actually be one
    // of the "special case" MethodTables (such as Object[]) instead of an ArrayTypeDesc and our TypeHandle CanCastTo code can't
    // cope with all the different possible combinations. In general we want to normalize the TypeHandle into an ArrayTypeDesc
    // for these cases.
    if (thExactMethodType.IsArrayType() && !thExactMethodType.IsArray())
    {
        TypeHandle thElement = thExactMethodType.AsMethodTable()->GetApproxArrayElementTypeHandle();
        CorElementType etElement = thExactMethodType.AsMethodTable()->GetInternalCorElementType();
        unsigned uRank = thExactMethodType.AsMethodTable()->GetRank();

        thExactMethodType = ClassLoader::LoadArrayTypeThrowing(thElement,
                                                               etElement,
                                                               uRank,
                                                               ClassLoader::DontLoadTypes);
    }

    // Get signatures for the delegate invoke and target methods.
    MetaSig sigInvoke(pInvokeMethod, thDelegate);
    MetaSig sigTarget(pTargetMethod, thExactMethodType);

    // Check that there is no vararg mismatch.
    if (sigInvoke.IsVarArg() != sigTarget.IsVarArg())
        return FALSE;

    // The relationship between the number of arguments on the delegate invoke and target methods tells us a lot about the type of
    // delegate we'll create (open or closed over the first argument). We're getting the fixed argument counts here, which are all
    // the arguments apart from any implicit 'this' pointers.
    // On the delegate invoke side (the caller) the total number of arguments is the number of fixed args to Invoke plus one if the
    // delegate is closed over an argument (i.e. that argument is provided at delegate creation time).
    // On the target method side (the callee) the total number of arguments is the number of fixed args plus one if the target is an
    // instance method.
    // These two totals should match for any compatible delegate and target method.
    UINT numFixedInvokeArgs = sigInvoke.NumFixedArgs();
    UINT numFixedTargetArgs = sigTarget.NumFixedArgs();
    UINT numTotalTargetArgs = numFixedTargetArgs + (pTargetMethod->IsStatic() ? 0 : 1);

    // Determine whether the match (if it is otherwise compatible) would result in an open or closed delegate or is just completely
    // out of whack.
    BOOL fIsOpenDelegate;
    if (numTotalTargetArgs == numFixedInvokeArgs)
        // All arguments provided by invoke, delegate must be open.
        fIsOpenDelegate = TRUE;
    else if (numTotalTargetArgs == numFixedInvokeArgs + 1)
        // One too few arguments provided by invoke, delegate must be closed.
        fIsOpenDelegate = FALSE;
    else
        // Target method cannot possibly match the invoke method.
        return FALSE;

    // Deal with cases where the caller wants a specific type of delegate.
    if (flags & DBF_OpenDelegateOnly && !fIsOpenDelegate)
        return FALSE;
    if (flags & DBF_ClosedDelegateOnly && fIsOpenDelegate)
        return FALSE;

    // If the target (or first argument) is null, the delegate type would be closed and the caller explicitly doesn't want to allow
    // closing over null then filter that case now.
    if (flags & DBF_NeverCloseOverNull && thFirstArg.IsNull() && !fIsOpenDelegate)
        return FALSE;

    // If, on the other hand, we're looking at an open delegate but the caller has provided a target it's also not a match.
    if (fIsOpenDelegate && !thFirstArg.IsNull())
        return FALSE;

    // We don't allow open delegates over virtual value type methods. That's because we currently have no way to allow the first
    // argument of the invoke method to be specified in such a way that the passed value would be both compatible with the target
    // method and type safe. Virtual methods always have an objref instance (they depend on this for the vtable lookup algorithm) so
    // we can't take a Foo& first argument like other value type methods. We also can't accept System.Object or System.ValueType in
    // the invoke signature since that's not specific enough and would allow type safety violations.
    // Someday we may invent a boxing stub which would take a Foo& passed in box it before dispatch. This is unlikely given that
    // it's a lot of work for an edge case (especially considering that open delegates over value types are always going to be
    // tightly bound to the specific value type). It would also be an odd case where merely calling a delegate would involve an
    // allocation and thus potential failure before you even entered the method.
    // So for now we simply disallow this case.
    if (fIsOpenDelegate && pTargetMethod->IsVirtual() && thExactMethodType.IsValueType())
        return FALSE;

    // If we get here the basic shape of the signatures match up for either an open or closed delegate. Now we need to verify that
    // those signatures are type compatible. This is complicated somewhat by the matrix of delegate type to target method types
    // (open static vs closed instance etc.). Where we get the first argument type on the invoke side is controlled by open vs
    // closed: closed delegates get the type from the target, open from the first invoke method argument (which is always a fixed
    // arg). Similarly the location of the first argument type on the target method side is based on static vs instance (static from
    // the first fixed arg, instance from the type of the method).

    TypeHandle thFirstInvokeArg;
    TypeHandle thFirstTargetArg;

    // There is one edge case for an open static delegate which takes no arguments. In that case we're nearly done, just compare the
    // return types.
    if (numTotalTargetArgs == 0)
    {
        _ASSERTE(pTargetMethod->IsStatic());
        _ASSERTE(fIsOpenDelegate);

        goto CheckReturnType;
    }
                
    // Invoke side first...
    if (fIsOpenDelegate)
    {
        // No bound arguments, take first type from invoke signature.
        if (sigInvoke.NextArgNormalized() == ELEMENT_TYPE_END)
            return FALSE;
        thFirstInvokeArg = sigInvoke.GetLastTypeHandleThrowing();
    }
    else
        // We have one bound argument and the type of that is what we must compare first.
        thFirstInvokeArg = thFirstArg;

    // And now the first target method argument for comparison...
    if (pTargetMethod->IsStatic())
    {
        // The first argument for a static method is the first fixed arg.
        if (sigTarget.NextArgNormalized() == ELEMENT_TYPE_END)
            return FALSE;
        thFirstTargetArg = sigTarget.GetLastTypeHandleThrowing();

        // Delegates closed over static methods have a further constraint: the first argument of the target must be an object
        // reference type (otherwise the argument shuffling logic could get complicated).
        if (!fIsOpenDelegate)
        {
            CorElementType etFirstTargetArg = thFirstTargetArg.GetInternalCorElementType();
            if (!CorTypeInfo::IsObjRef(etFirstTargetArg))
                return FALSE;
        }
    }
    else
    {
        // The type of the first argument to an instance method is from the method type.
        thFirstTargetArg = thExactMethodType;

        // If the delegate is open and the target method is on a value type or primitive then the first argument of the invoke
        // method must be a reference to that type. So make promote the type we got from the reference to a ref. (We don't need to
        // do this for the closed instance case because there we got the invocation side type from the first arg passed in, i.e.
        // it's had the ref stripped from it implicitly).
        if (fIsOpenDelegate)
        {
            CorElementType etFirstTargetArg = thFirstTargetArg.GetInternalCorElementType();
            if (etFirstTargetArg <= ELEMENT_TYPE_R8 ||
                etFirstTargetArg == ELEMENT_TYPE_VALUETYPE ||
                etFirstTargetArg == ELEMENT_TYPE_I ||
                etFirstTargetArg == ELEMENT_TYPE_U)
                thFirstTargetArg = thFirstTargetArg.MakeByRef();
        }
    }

    // Now we have enough data to compare the first arguments on the invoke and target side. Skip this if we are closed over null
    // (we don't have enough type information for the match but it doesn't matter because the null matches all object reference
    // types, which our first arg must be in this case). We always relax signature matching for the first argument of an instance
    // method, since it's always allowable to call the method on a more derived type. In cases where we're closed over the first
    // argument we know that argument is boxed (because it was passed to us as an object). We provide this information to
    // IsLocationAssignable because it relaxes signature matching for some important cases (e.g. passing a value type to an argument
    // typed as Object).
    if (!thFirstInvokeArg.IsNull())
        if (!IsLocationAssignable(thFirstInvokeArg,
                                  thFirstTargetArg,
                                  !pTargetMethod->IsStatic() || flags & DBF_RelaxedSignature,
                                  !fIsOpenDelegate))
            return FALSE;

    // Loop over the remaining fixed args, the list should be one to one at this point.
    while (TRUE)
    {
        CorElementType etInvokeArg = sigInvoke.NextArgNormalized();
        CorElementType etTargetArg = sigTarget.NextArgNormalized();
        if (etInvokeArg == ELEMENT_TYPE_END || etTargetArg == ELEMENT_TYPE_END)
        {
            // We've reached the end of one signature. We better be at the end of the other or it's not a match.
            if (etInvokeArg != etTargetArg)
                return FALSE;
            break;
        }
        else
        {
            TypeHandle thInvokeArg = sigInvoke.GetLastTypeHandleThrowing();
            TypeHandle thTargetArg = sigTarget.GetLastTypeHandleThrowing();

            if (!IsLocationAssignable(thInvokeArg, thTargetArg, flags & DBF_RelaxedSignature, FALSE))
                return FALSE;
        }
    } 

 CheckReturnType:

    // Almost there, just compare the return types (remember that the assignment is in the other direction here, from callee to
    // caller, so switch the order of the arguments to IsLocationAssignable).
    // If we ever relax this we have to think about how to unbox this arg in the Nullable<T> case also.  
    if (!IsLocationAssignable(sigTarget.GetRetTypeHandleThrowing(),
                              sigInvoke.GetRetTypeHandleThrowing(),
                              flags & DBF_RelaxedSignature,
                              FALSE))
        return FALSE;

    // We must have a match.
    if (pfIsOpenDelegate)
        *pfIsOpenDelegate = fIsOpenDelegate;
    return TRUE;
}

MethodDesc* COMDelegate::GetDelegateCtor(TypeHandle delegateType, MethodDesc *pTargetMethod, DelegateCtorArgs *pCtorData)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    MethodDesc *pRealCtor = NULL;

    DelegateEEClass *pDelCls = (DelegateEEClass*)(delegateType.GetClass());
    PREFIX_ASSUME(pDelCls != NULL);
    g_IBCLogger.LogEEClassAndMethodTableAccess(pDelCls);
    MethodDesc *pDelegateInvoke = COMDelegate::FindDelegateInvokeMethod(delegateType.GetMethodTable());

    MetaSig invokeSig(pDelegateInvoke);
    MetaSig methodSig(pTargetMethod);
    UINT invokeArgCount = invokeSig.NumFixedArgs();
    UINT methodArgCount = methodSig.NumFixedArgs();
    BOOL isStatic = pTargetMethod->IsStatic();
    if (!isStatic) 
        methodArgCount++; // count 'this'
    Assembly *pCallerAssembly = ((MethodDesc*)pCtorData->pMethod)->GetAssembly();
    BOOL needsSecureDelegate = NeedsSecureDelegate(pCallerAssembly, GetAppDomain(), pTargetMethod);

    // Force the slow path for nullable so that we can give the user an error in case were the verifier is not run. 
    MethodTable* pMT = pTargetMethod->GetMethodTable();
    if (!pTargetMethod->IsStatic() && Nullable::IsNullableType(pMT))
        return NULL;


    // DELEGATE KINDS TABLE
    //
    //                                  _target         _methodPtr              _methodPtrAux       _invocationList     _invocationCount
    //
    // 1- Instance closed               'this' ptr      target method           null                null                0
    // 2- Instance open non-virt        delegate        shuffle thunk           target method       null                0
    // 3- Instance open virtual         delegate        Virtual-stub dispatch   method id           null                0
    // 4- Static closed                 first arg       target method           null                null                0
    // 5- Static closed (special sig)   delegate        specialSig thunk        target method       first arg           0
    // 6- Static opened                 delegate        shuffle thunk           target method       null                0
    // 7- Secure                        delegate        call thunk              MethodDesc (frame)  target delegate     creator assembly 
    //
    // Delegate invoke arg count == target method arg count - 2, 3, 6
    // Delegate invoke arg count == 1 + target method arg count - 1, 4, 5
    //
    // 1, 4     - MulticastDelegate.ctor1 (simply assign _target and _methodPtr)
    // 5        - MulticastDelegate.ctor2 (see table, takes 3 args)
    // 2, 6     - MulticastDelegate.ctor3 (take shuffle thunk)
    // 3        - MulticastDelegate.ctor4 (take shuffle thunk, retrive MethodDesc) ???
    //
    // 7 - Needs special handling

    if (invokeArgCount == methodArgCount) 
    {
        // case 2, 3, 6
        if (!isStatic && pTargetMethod->IsVirtual()) 
        {
            // case 3
            if (needsSecureDelegate)
                pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_SECURE_VIRTUAL_DISPATCH);
            else
                pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_VIRTUAL_DISPATCH);
        }
        else
        {
            // case 2, 6
            if (needsSecureDelegate)
                pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_SECURE_OPENED);
            else
                pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_OPENED);
        }
        Stub *pShuffleThunk = NULL;
        if (!pTargetMethod->IsStatic() && pTargetMethod->HasRetBuffArg()) 
            pShuffleThunk = pDelCls->m_pInstRetBuffCallStub;
        else
            pShuffleThunk = pDelCls->m_pStaticCallStub;

        if (!pShuffleThunk) 
            pShuffleThunk = SetupShuffleThunk(pDelCls, pTargetMethod);
        pCtorData->pArg3 = (void*)pShuffleThunk->GetEntryPoint();
        if (needsSecureDelegate)
        {
            // need to fill the info for the secure delegate
            pCtorData->pArg4 = GetSecureInvoke(pDelegateInvoke);
            pCtorData->pArg5 = pCallerAssembly;
        }
    }
    else 
    {
        _ASSERTE(invokeArgCount + 1 == methodArgCount);

#ifdef HAS_THISPTR_RETBUF_PRECODE
        // Force closed delegates over static methods with return buffer to go via 
        // the slow path to create ThisPtrRetBufPrecode
        if (isStatic && pTargetMethod->HasRetBuffArg())
            return NULL;
#endif

        // under the conditions below the delegate ctor needs to perform some heavy operation
        // to either resolve the interface call to the real target or to get the unboxing stub (or both)
        BOOL needsRuntimeInfo = !pTargetMethod->IsStatic() && 
                    (pTargetMethod->IsInterface() ||
                    (pTargetMethod->GetMethodTable()->IsValueType() && !pTargetMethod->IsUnboxingStub()));

        if (needsSecureDelegate)
        {
            if (needsRuntimeInfo)
                pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_SECURE_RT_CLOSED);
            else
            {
                if (!isStatic) 
                    pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_SECURE_CLOSED);
                else
                    pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_SECURE_CLOSED_STATIC);
            }

            // need to fill the info for the secure delegate
            pCtorData->pArg3 = GetSecureInvoke(pDelegateInvoke);
            pCtorData->pArg4 = pCallerAssembly;
        }
        else
        {
            if (needsRuntimeInfo)
                pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_RT_CLOSED);
            else
            {
                if (!isStatic) 
                    pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_CLOSED);
                else
                    pRealCtor = g_Mscorlib.GetMethod(METHOD__MULTICAST_DELEGATE__CTOR_CLOSED_STATIC);
            }
        }
    }

    return pRealCtor;
}


/*@GENERICSVER: new (works for generics too)
    Does a static validation of parameters passed into a delegate constructor.


    For "new Delegate(obj.method)" where method is statically typed as "C::m" and
    the static type of obj is D (some subclass of C)...

    Params:
    instHnd : Static type of the instance, from which pFtn is obtained. Ignored if pFtn 
             is static (i.e. D)
    ftnParentHnd: Parent of the MethodDesc, pFtn, used to create the delegate (i.e. type C)
    pFtn  : (possibly shared) MethodDesc of the function pointer used to create the delegate (i.e. C::m)
    pDlgt : The delegate type (i.e. Delegate)
    module: The module scoping methodMemberRef and delegateConstructorMemberRef
    methodMemberRef: the MemberRef, MemberDef or MemberSpec of the target method  (i.e. a mdToken for C::m)
    delegateConstructorMemberRef: the MemberRef, MemberDef or MemberSpec of the delegate constructor (i.e. a mdToken for Delegate::.ctor)

    Validates the following conditions:
    1.  If the function (pFtn) is not static, pInst should be equal to the type where 
        pFtn is defined or pInst should be a parent of pFtn's type.
    2.  The signature of the function should be compatible with the signature
        of the Invoke method of the delegate type.
        The signature is retrieved from module, methodMemberRef and delegateConstructorMemberRef

    NB: Although some of these arguments are redundant, we pass them in to avoid looking up 
        information that should already be available.
        Instead of comparing type handles modulo some context, the method directly compares metadata to avoid 
        loading classes referenced in the method signatures (hence the need for the module and member refs).
        Also, because this method works directly on metadata, without allowing any additional instantiation of the
        free type variables in the signature of the method or delegate constructor, this code
        will *only* verify a constructor application at the typical (ie. formal) instantiation.
*/
/* static */
BOOL COMDelegate::ValidateCtor(TypeHandle instHnd,
                               TypeHandle ftnParentHnd, 
                               MethodDesc *pFtn, 
                               TypeHandle dlgtHnd)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pFtn));
        PRECONDITION(!dlgtHnd.IsNull());
        PRECONDITION(!ftnParentHnd.IsNull());

        INJECT_FAULT(COMPlusThrowOM()); // from MetaSig::CompareElementType
    }
    CONTRACTL_END;

    DelegateEEClass *pdlgEEClass = (DelegateEEClass*)dlgtHnd.GetClass();
    PREFIX_ASSUME(pdlgEEClass != NULL);
    MethodDesc *pDlgtInvoke = pdlgEEClass->m_pInvokeMethod;
    if (pDlgtInvoke == NULL)
        return FALSE;
    return IsMethodDescCompatible(instHnd, ftnParentHnd, pFtn, dlgtHnd, pDlgtInvoke, DBF_RelaxedSignature, NULL);
}



BOOL COMDelegate::ValidateBeginInvoke(DelegateEEClass* pClass)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pClass));
        PRECONDITION(CheckPointer(pClass->m_pBeginInvokeMethod));

        // insert fault. Can the binder throw an OOM?
    }
    CONTRACTL_END;

    if (pClass->m_pInvokeMethod == NULL) 
        return FALSE;

    // We check the signatures under the typical instantiation of the possibly generic class 
    MetaSig beginInvokeSig(pClass->m_pBeginInvokeMethod->LoadTypicalMethodDefinition());
    MetaSig invokeSig(pClass->m_pInvokeMethod->LoadTypicalMethodDefinition());

    if (beginInvokeSig.GetCallingConventionInfo() != (IMAGE_CEE_CS_CALLCONV_HASTHIS | IMAGE_CEE_CS_CALLCONV_DEFAULT))
        return FALSE;

    if (beginInvokeSig.NumFixedArgs() != invokeSig.NumFixedArgs() + 2)
        return FALSE;

    if (s_pIAsyncResult == 0) {
        s_pIAsyncResult = g_Mscorlib.FetchClass(CLASS__IASYNCRESULT);
        if (s_pIAsyncResult == 0)
            return FALSE;
    }

    if (beginInvokeSig.GetRetTypeHandleThrowing() != TypeHandle(s_pIAsyncResult))
        return FALSE;

    while(invokeSig.NextArg() != ELEMENT_TYPE_END)
    {
        beginInvokeSig.NextArg();
        if (beginInvokeSig.GetLastTypeHandleThrowing() != invokeSig.GetLastTypeHandleThrowing())
            return FALSE;
    }

    if (s_pAsyncCallback == 0)
    {
        s_pAsyncCallback = g_Mscorlib.FetchClass(CLASS__ASYNCCALLBACK);
        if (s_pAsyncCallback == 0)
            return FALSE;
    }

    beginInvokeSig.NextArg();
    if (beginInvokeSig.GetLastTypeHandleThrowing()!= TypeHandle(s_pAsyncCallback))
        return FALSE;

    beginInvokeSig.NextArg();
    if (beginInvokeSig.GetLastTypeHandleThrowing()!= TypeHandle(g_pObjectClass))
        return FALSE;

    if (beginInvokeSig.NextArg() != ELEMENT_TYPE_END)
        return FALSE;

    return TRUE;
}

BOOL COMDelegate::ValidateEndInvoke(DelegateEEClass* pClass)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pClass));
        PRECONDITION(CheckPointer(pClass->m_pEndInvokeMethod));

        // insert fault. Can the binder throw an OOM?
    }
    CONTRACTL_END;

    if (pClass->m_pInvokeMethod == NULL) 
        return FALSE;

    // We check the signatures under the typical instantiation of the possibly generic class 
    MetaSig endInvokeSig(pClass->m_pEndInvokeMethod->LoadTypicalMethodDefinition());
    MetaSig invokeSig(pClass->m_pInvokeMethod->LoadTypicalMethodDefinition());

    if (endInvokeSig.GetCallingConventionInfo() != (IMAGE_CEE_CS_CALLCONV_HASTHIS | IMAGE_CEE_CS_CALLCONV_DEFAULT))
        return FALSE;

    if (endInvokeSig.GetRetTypeHandleThrowing() != invokeSig.GetRetTypeHandleThrowing())
        return FALSE;

    CorElementType type;
    while((type = invokeSig.NextArg()) != ELEMENT_TYPE_END)
    {
        if (type == ELEMENT_TYPE_BYREF)
        {
            endInvokeSig.NextArg();
            if (endInvokeSig.GetLastTypeHandleThrowing() != invokeSig.GetLastTypeHandleThrowing())
                return FALSE;
        }
    }

    if (s_pIAsyncResult == 0)
    {
        s_pIAsyncResult = g_Mscorlib.FetchClass(CLASS__IASYNCRESULT);
        if (s_pIAsyncResult == 0)
            return FALSE;
    }

    if (endInvokeSig.NextArg() == ELEMENT_TYPE_END)
        return FALSE;

    if (endInvokeSig.GetLastTypeHandleThrowing() != TypeHandle(s_pIAsyncResult))
        return FALSE;

    if (endInvokeSig.NextArg() != ELEMENT_TYPE_END)
        return FALSE;

    return TRUE;
}

BOOL COMDelegate::IsSecureDelegate(DELEGATEREF dRef)
{
    CONTRACTL
    {
        MODE_ANY;
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    DELEGATEREF innerDel = NULL;
    if (dRef->GetInvocationCount() != 0)
    {
        innerDel = (DELEGATEREF) dRef->GetInvocationList();
        if (innerDel != NULL && innerDel->GetMethodTable()->GetClass()->IsAnyDelegateClass()) 
        {
            // We have a secure delegate
            return TRUE;
        }
    }        
    return FALSE;
}

#endif // !DACCESS_COMPILE


// Decides if pcls derives from Delegate.
BOOL COMDelegate::IsDelegate(MethodTable *pMT)
{
    WRAPPER_CONTRACT;
    return pMT->IsAnyDelegateExact() || pMT->IsAnyDelegateClass();
}



BOOL DelegateTransitionFrame::TraceFrame(Thread *thread, BOOL fromPatch, TraceDestination *trace, REGDISPLAY *regs)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) GC_TRIGGERS; else GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(thread));
        PRECONDITION(CheckPointer(trace));
    }
    CONTRACTL_END;


    //
    // Get the call site info
    //

    TADDR ip, returnIP, returnSP;
    GetUnmanagedCallSite(&ip, &returnIP, &returnSP);

    //
    // If we've already made the call, we can't trace any more.
    //
    // !!! Note that this test isn't exact.
    //

    if (!fromPatch
        && (PTR_HOST_TO_TADDR(thread->GetFrame()) != PTR_HOST_TO_TADDR(this)
            || !thread->m_fPreemptiveGCDisabled
            || *PTR_TADDR(returnSP) == returnIP))
    {
        LOG((LF_CORDB, LL_INFO10000, "DelegateTransitionFrame::TraceFrame: can't trace...\n"));
        return FALSE;
    }

    //
    // Otherwise, return the unmanaged destination.
    //

    trace->InitForUnmanaged(ip);

    LOG((LF_CORDB, LL_INFO10000, "DelegateTransitionFrame::TraceFrame: ip=0x%p\n", ip));

    return TRUE;
}

void DelegateTransitionFrame::GetUnmanagedCallSite(TADDR* ip,
                                                   TADDR* returnIP,
                                                   TADDR* returnSP)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) GC_TRIGGERS; else GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (ip != NULL)
        *ip = GetFunctionPointer();

    //
    // Get ML header
    //

    MLHeader *pheader;

    DWORD cbLocals;
    DWORD cbDstBuffer;
    DWORD cbThisCallInfo;

    pheader = PTR_MLHeader((TADDR)GetDelegateClass()->m_pMLStub->
                           GetEntryPoint());
    _ASSERTE(pheader);
    
    cbLocals = pheader->m_cbLocals;
    cbDstBuffer = pheader->m_cbDstBuffer;
    cbThisCallInfo = (pheader->m_Flags & MLHF_THISCALL) ? 4 : 0;

    //
    // Compute call site info for ComputeDllImportStub
    //

    if (returnIP != NULL)
        *returnIP = GFN_TADDR(DllImportForDelegateGenericStubReturnFromCall);

    //
    // !!! yow, this is a bit fragile...
    //

    if (returnSP != NULL)
        *returnSP = PTR_HOST_TO_TADDR(this)
                            - GetNegSpaceSize()
                            - DllImportForDelegateGenericWorkerStackSize
                            - cbLocals
                            - cbDstBuffer
                            + cbThisCallInfo
                            - sizeof(TADDR);
}

#ifndef DACCESS_COMPILE


//
// Promote Caller Stack
//
//

void DelegateTransitionFrame::PromoteCallerStackWorker(promote_func* fn, ScanContext* sc, BOOL fPinArgs)
{
    WRAPPER_CONTRACT;

    MethodDesc   *pFunction;

    LOG((LF_GC, INFO3, "    Promoting method caller Arguments\n" ));

    // We're going to have to look at the signature to determine
    // which arguments are pointers....First we need the function
    pFunction = GetInvokeFunctionUnsafe();
    if (! pFunction)
        return;

    MetaSig msig(pFunction);
    msig.DangerousRemoveHasThis();
        
    ArgIterator argit ((LPBYTE)(LPVOID)this, &msig, TRUE);
    PromoteCallerStackHelper (fn, sc, fPinArgs, &argit, &msig);
}

void DelegateTransitionFrame::PromoteCallerStackHelper(promote_func* fn, 
                                                 ScanContext* sc, BOOL fPinArgs,
                                                 ArgIterator *pargit, MetaSig *pmsig)
{
    WRAPPER_CONTRACT;

    DWORD           GcFlags;

    if (pmsig->HasRetBuffArg())
    {
        LPVOID* pRetBuffArg = pargit->GetRetBuffArgAddr();
        GcFlags = GC_CALL_INTERIOR;
        if (fPinArgs)
        {
            GcFlags |= GC_CALL_PINNED;
            LOG((LF_GC, INFO3, "    ret buf Argument pinned at " FMT_ADDR "\n", DBG_ADDR(*pRetBuffArg) ));
        }
        LOG((LF_GC, INFO3, "    ret buf Argument promoted from" FMT_ADDR "to ", DBG_ADDR(*pRetBuffArg) ));
        PromoteCarefully(fn, *(Object**) pRetBuffArg, sc, GcFlags|CHECK_APP_DOMAIN);
    }

    if (fPinArgs)
    {
        CorElementType typ;
        LPVOID pArgAddr;
        while (typ = pmsig->PeekArg(), NULL != (pArgAddr = pargit->GetNextArgAddr()))
        {
            if (typ == ELEMENT_TYPE_SZARRAY)
            {
                ArrayBase *pArray = *((ArrayBase**)pArgAddr);

                if (pArray)
                {
                    (fn)(*(Object**)pArgAddr, sc, 
                         GC_CALL_PINNED | CHECK_APP_DOMAIN);
                }
            }
            else if (typ == ELEMENT_TYPE_BYREF)
            {
                {
                    (fn)(*(Object**)pArgAddr, sc, 
                         GC_CALL_PINNED | GC_CALL_INTERIOR | CHECK_APP_DOMAIN);
                }


            }
            else if (typ == ELEMENT_TYPE_STRING || (typ == ELEMENT_TYPE_CLASS && pmsig->IsStringType()))
            {
                (fn)(*(Object**)pArgAddr, sc, GC_CALL_PINNED);

            }
            else if (typ == ELEMENT_TYPE_VAR || typ == ELEMENT_TYPE_MVAR)
            {
              // Shouldn't appear because PeekArg does the instantiation
              _ASSERTE(!"Class or method variable");
            }
            else if (typ == ELEMENT_TYPE_GENERICINST)
            { 
              // Shouldn't appear because PeekArg looks at the generic type element type
              _ASSERTE(!"ELEMENT_TYPE_GENERICINST");
            }
            else if (typ == ELEMENT_TYPE_CLASS || typ == ELEMENT_TYPE_OBJECT)
            {
                Object *pObj = *(Object**)pArgAddr;
                if (pObj != NULL)
                {
                    MethodTable *pMT = pObj->GetGCSafeMethodTable();
                    _ASSERTE(sizeof(UINT_PTR) == sizeof(MethodTable*));;
                    if (pMT->IsObjectClass() || pMT->IsBlittable() || pMT->HasLayout())
                    {
                        (fn)(*(Object**)pArgAddr, sc, 
                             GC_CALL_PINNED | CHECK_APP_DOMAIN);
                    }
                    else
                    {
                        (fn)(*(Object**)pArgAddr, sc, 
                             CHECK_APP_DOMAIN);
                    }
                }
            }
            else
            {
                pmsig->GcScanRoots(pArgAddr, fn, sc);
            }
        }
    }
    else
    {
        LPVOID pArgAddr;
    
        while (NULL != (pArgAddr = pargit->GetNextArgAddr()))
        {
            pmsig->GcScanRoots(pArgAddr, fn, sc);
        }
    }
}



EXTERN_C int __stdcall
DllImportForDelegateGenericStubComputeFrameSize(Thread *pThread, DelegateTransitionFrame *pFrame, MLHeader** ppHeader)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
    }
    CONTRACTL_END;

    MLHeader *pheader = (MLHeader*) pFrame->GetDelegateClass()->m_pMLStub->GetEntryPoint();
    _ASSERTE(pheader);

#ifdef _DEBUG
    MethodDesc *pMD = pFrame->GetInvokeFunctionUnsafe();
    LOG((LF_STUBS, LL_INFO1000, "Calling DllImportForDelegateGenericStubWorker %s::%s \n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName));
#endif // _DEBUG

    // Tell our caller to allocate enough memory to store both the
    // destination buffer and the locals.
    *ppHeader = pheader;
    return pheader->m_cbDstBuffer + pheader->m_cbLocals;
}


EXTERN_C int __stdcall
DllImportForDelegateGenericStubBuildArguments(
    Thread *pThread,
    DelegateTransitionFrame *pFrame,
    MLHeader *pheader,
    MLCode const **ppMLCode,
    BYTE **pplocals,
    LPVOID* ppvFn,
    BYTE *pAlloc
    )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(pheader));
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(SOToleranceViolation);

#ifdef _DEBUG
    FillMemory(pAlloc, pheader->m_cbDstBuffer + pheader->m_cbLocals, 0xcc);
#endif

    BYTE   *pdst    = pAlloc;
    BYTE   *plocals = pdst + pheader->m_cbDstBuffer;

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
    pdst += pheader->m_cbDstBuffer;
#endif

    VOID   *psrc          = (VOID*)pFrame;

    CleanupWorkList *pCleanup = pFrame->GetCleanupWorkList();

    if (pCleanup)
    {
        // Checkpoint the current thread's fast allocator (used for temporary
        // buffers over the call) and schedule a collapse back to the
        // checkpoint in the cleanup list. Note that if we need the allocator,
        // it is guaranteed that a cleanup list has been allocated.
        void* cp = pThread->m_MarshalAlloc.GetCheckpoint();
        pCleanup->ScheduleFastFree(cp);
        pCleanup->IsVisibleToGc();
    }

#ifdef _PPC_
    // float registers
    DOUBLE *pFloatRegs = (DOUBLE*)(pdst + 
        AlignStack(pheader->m_cbLocals + pheader->m_cbDstBuffer 
            + NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE)) - NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE));
    ZeroMemory(pFloatRegs, NUM_FLOAT_ARGUMENT_REGISTERS * sizeof(DOUBLE));
#endif // _PPC_

    // Call the ML interpreter to translate the arguments. Assuming
    // it returns, we get back a pointer to the succeeding code stream
    // which we will save away for post-call execution.
    *ppMLCode = RunML(pheader->GetMLCode(),
                      psrc,
                      pdst,
                      (UINT8*const) plocals,
                      pCleanup
                      PPC_ARG(pFloatRegs)
                      );

#if (defined(_DEBUG) || defined(PROFILING_SUPPORTED))
    MethodDesc *pMD = (MethodDesc*)(pFrame->GetInvokeFunctionUnsafe());
#endif

    LOG((LF_IJW, LL_INFO1000, "Delegate DllImport call (\"%s.%s%s\")\n", pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName, pMD->m_pszDebugMethodSignature));


    *ppvFn = (LPVOID)pFrame->GetFunctionPointer();


    // Perf could be improved here by putting the hook information into the delegate instance
    // Alternatively, we could burn the hook info into the stub itself.
    IHostTaskManager *pManager= CorHost2::GetHostTaskManager();
    if (pManager != NULL)
    {
        BOOL fHook;
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->CallNeedsHostHook((size_t)*ppvFn, &fHook);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        _ASSERTE (hr == S_OK);
        if (SUCCEEDED(hr) && fHook)
        {
            Thread::LeaveRuntime(*(size_t*)ppvFn);
            pFrame->MarkEnterRuntimeRequired();
        }
    }

#if _DEBUG
    //
    // Call through debugger routines to double check their
    // implementation
    //
    *ppvFn = (void*) CheckExitFrameDebuggerCalls;
#endif // _DEBUG



    // Call the target.
    pThread->EnablePreemptiveGC();

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of call out of the runtime
    if (CORProfilerTrackTransitions())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ManagedToUnmanagedTransition((CodeID) pMD, COR_PRF_TRANSITION_CALL);
    }
#endif // PROFILING_SUPPORTED
    
    *pplocals = plocals;
    return pheader->m_Flags;
}

EXTERN_C INT64 __stdcall
DllImportForDelegateGenericStubPostCall(
    Thread *pThread,
    DelegateTransitionFrame *pFrame,
    MLHeader *pheader,
    MLCode const *pMLCode,
    BYTE *plocals,
    INT64 nativeReturnValue
#if defined(_X86_)
    ,
    DWORD PreESP,
    DWORD PostESP
#endif // defined(_X86_)
    )
{
    INT64 returnValue = 0;

    // Moved above the contract because Contract doesn't respect GetLastError
    if (pheader->m_Flags & MLHF_SETLASTERROR)
        pThread->m_dwLastError = GetLastError();

    // this function is called by ndirect stubs which are called by managed code
    // as a result, we need an unwind and continue handler to translate any of our
    // internal exceptions into managed exceptions.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;

        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(CheckPointer(pheader));
    }
    CONTRACTL_END;

    {
        // Ensure we don't respond to a thread abort request until after we
        // have marshaled our return result(s). In particular native handles
        // need to be back propagated into safe handles before it's safe to
        // take a thread abort.
        ThreadPreventAbortHolder preventAbort(TRUE);

#if defined(_X86_) && defined(_DEBUG)
        _ASSERTE ((PreESP <= PostESP && PostESP <= PreESP + pheader->m_cbDstBuffer)
                  ||!"esp is trashed by DllImport for Delegate call, possibly wrong signature");
#endif // defined(_X86_) && defined(_DEBUG)


#if defined(_X86_) || defined(PROFILING_SUPPORTED)
        MethodDesc *pMD = (MethodDesc*)(pFrame->GetInvokeFunctionUnsafe());
#endif

#if defined(_X86_)

    
#endif // defined(_X86_)

#if BIGENDIAN
        // returnValue and nativeReturnValue are treated as 8 bytes buffers. The 32 bit values
        // will be in the high dword!
#endif // BIGENDIAN

        if (pheader->GetUnmanagedRetValTypeCat() == MLHF_TYPECAT_FPU)
        {
            int fpNativeSize;
            if (pheader->m_Flags & MLHF_64BITUNMANAGEDRETVAL)
                fpNativeSize = 8;
            else
                fpNativeSize = 4;

            getFPReturn(fpNativeSize, &nativeReturnValue);
        }

#ifdef PROFILING_SUPPORTED
        // Notify the profiler of return from call out of the runtime
        if (CORProfilerTrackTransitions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->UnmanagedToManagedTransition((CodeID) pMD, COR_PRF_TRANSITION_RETURN);
        }
#endif // PROFILING_SUPPORTED

        if (pFrame->EnterRuntimeRequired())
        {
            Thread::EnterRuntime();
            pFrame->UnMarkEnterRuntimeRequired();
        }

        pThread->DisablePreemptiveGC();
    

        CleanupWorkList *pCleanup = pFrame->GetCleanupWorkList();


        // Marshal the return value and propagate any [out] parameters back.
        // Assumes a little-endian architecture!
        RunML(pMLCode,
              &nativeReturnValue,
              ((BYTE*)&returnValue)
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
              + ((pheader->m_Flags & MLHF_64BITMANAGEDRETVAL) ? 8 : 4)
#endif
              ,
              (UINT8*const)plocals,
              pCleanup);

        int managedRetValTypeCat = pheader->GetManagedRetValTypeCat();

        if (pCleanup)
        {
            if (managedRetValTypeCat == MLHF_TYPECAT_GCREF)
            {
                GCPROTECT_BEGIN(*(VOID**)&returnValue);
                pCleanup->Cleanup(FALSE);
                GCPROTECT_END();
            }
            else
            {
                pCleanup->Cleanup(FALSE);
            }
        }

        if (managedRetValTypeCat == MLHF_TYPECAT_FPU)
        {
            int fpComPlusSize;
            if (pheader->m_Flags & MLHF_64BITMANAGEDRETVAL)
                fpComPlusSize = 8;
            else
                fpComPlusSize = 4;

            setFPReturn(fpComPlusSize, returnValue);
        }
    }

    if (g_TrapReturningThreads)
        pThread->HandleThreadAbort();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    return returnValue;
}




// Helper to construct an UnhandledExceptionEventArgs.  This may fail for out-of-memory or
// other reasons.  Currently, we fall back on passing a NULL eventargs to the event sink.
// Another possibility is to have two shared immutable instances (one for isTerminating and
// another for !isTerminating).  These must be immutable because we perform no synchronization
// around delivery of unhandled exceptions.  They occur in a free-threaded manner on various
// threads.
//
// It doesn't add much value to communicate the isTerminating flag under these unusual
// conditions.
static void TryConstructUnhandledExceptionArgs(OBJECTREF *pThrowable,
                                               BOOL       isTerminating,
                                               OBJECTREF *pOutEventArgs)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(pThrowable    != NULL && IsProtectedByGCFrame(pThrowable));
    _ASSERTE(pOutEventArgs != NULL && IsProtectedByGCFrame(pOutEventArgs));
    _ASSERTE(*pOutEventArgs == NULL);

    EX_TRY
    {
        MethodTable *pMT = g_Mscorlib.FetchClass(CLASS__UNHANDLED_EVENTARGS);
        *pOutEventArgs = AllocateObject(pMT);

        MethodDescCallSite ctor(METHOD__UNHANDLED_EVENTARGS__CTOR, pOutEventArgs);

        ARG_SLOT args[] =
        {
            ObjToArgSlot(*pOutEventArgs),
            ObjToArgSlot(*pThrowable),
            isTerminating
        };

        ctor.Call(args);
    }
    EX_CATCH
    {
        *pOutEventArgs = NULL;      // arguably better than half-constructed object

        // It's not even worth asserting, because these aren't our bugs.  At
        // some point, a MDA may be warranted.
    }
    EX_END_CATCH(SwallowAllExceptions)
}


static void InvokeUnhandledSwallowing(OBJECTREF *pDelegate,
                                      OBJECTREF *pDomain,
                                      OBJECTREF *pEventArgs)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(pDelegate  != NULL && IsProtectedByGCFrame(pDelegate));
    _ASSERTE(pDomain    != NULL && IsProtectedByGCFrame(pDomain));
    _ASSERTE(pEventArgs == NULL || IsProtectedByGCFrame(pEventArgs));

    EX_TRY
    {
        // We've already exercised the prestub on this delegate's COMDelegate::GetMethodDesc,
        // as part of wiring up a reliable event sink.
        MethodDesc *pMD = COMDelegate::FindDelegateInvokeMethod((*pDelegate)->GetMethodTable());
        MethodDescCallSite invokeMethod(pMD, METHOD__UNHANDLED_HANDLER__INVOKE, pDelegate);

        ARG_SLOT    args[] =
        {
            ObjToArgSlot(*pDelegate),
            ObjToArgSlot(*pDomain),
            ObjToArgSlot(*pEventArgs),
        };

        invokeMethod.Call(args);
    }
    EX_CATCH
    {
        // It's not even worth asserting, because these aren't our bugs.  At
        // some point, a MDA may be warranted.
    }
    EX_END_CATCH(SwallowAllExceptions)
}


// cannot combine SEH & C++ exceptions in one method.  Split out from InvokeNotify.
static void InvokeNotifyInner(OBJECTREF *pDelegate, OBJECTREF *pDomain)
{
    // static contract, since we use SEH.
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    _ASSERTE(pDelegate  != NULL && IsProtectedByGCFrame(pDelegate));
    _ASSERTE(pDomain    != NULL && IsProtectedByGCFrame(pDomain));

    PAL_TRY
    {
        // We've already exercised the prestub on this delegate's COMDelegate::GetMethodDesc,
        // as part of wiring up a reliable event sink.
        MethodDesc *pMD = COMDelegate::FindDelegateInvokeMethod((*pDelegate)->GetMethodTable());
        MethodDescCallSite invokeMethod(pMD, METHOD__EVENT_HANDLER__INVOKE, pDelegate);

        ARG_SLOT    args[] =
        {
            ObjToArgSlot(*pDelegate),
            ObjToArgSlot(*pDomain),
            NULL                        // EventArgs
        };

        invokeMethod.Call(args);
    }
    PAL_EXCEPT_FILTER(ThreadBaseExceptionFilter, (PVOID)SystemNotification)
    {
        _ASSERTE(!"ThreadBaseExceptionFilter returned EXECUTE_HANDLER.");
    }
    PAL_ENDTRY;
}



static void InvokeNotify(OBJECTREF *pDelegate, OBJECTREF *pDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(pDelegate  != NULL && IsProtectedByGCFrame(pDelegate));
    _ASSERTE(pDomain    != NULL && IsProtectedByGCFrame(pDomain));

    STRESS_LOG2(LF_GC, LL_INFO1000, "Distributing reliable event: MethodPtr=%p MethodPtrAux=%p\n",
        DELEGATEREF(*pDelegate)->GetMethodPtr(),
        DELEGATEREF(*pDelegate)->GetMethodPtrAux());

    // All reliable events should be delivered on finalizer thread
    _ASSERTE(IsFinalizerThread());

    INDEBUG(Thread* pThread = GetThread());

    // This is an early check for condition that we assert in Thread::InternalReset called from DoOneFinalization later.
    _ASSERTE(!pThread->HasCriticalRegion());
    _ASSERTE(!pThread->HasThreadAffinity());

    EX_TRY
    {
        InvokeNotifyInner(pDelegate, pDomain);
    }
    EX_CATCH
    {
        // It's not even worth asserting, because these aren't our bugs.  At
        // some point, a MDA may be warranted.
    }
    EX_END_CATCH(SwallowAllExceptions)

    // This is an early check for condition that we assert in Thread::InternalReset called from DoOneFinalization later.
    _ASSERTE(!pThread->HasCriticalRegion());
    _ASSERTE(!pThread->HasThreadAffinity());
}


// For critical system events, ensure that each handler gets a notification --
// even if prior handlers in the chain have thrown an exception.  Also, try
// to deliver an unhandled exception event if we ever swallow an exception
// out of a reliable notification.  Note that the add_ event handers are
// responsible for any reliable preparation of the target, like eager JITting.
void DistributeEventReliably(OBJECTREF *pDelegate, OBJECTREF *pDomain)
{
    CONTRACTL
    {
        NOTHROW;  
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(pDelegate  != NULL && IsProtectedByGCFrame(pDelegate));
    _ASSERTE(pDomain    != NULL && IsProtectedByGCFrame(pDomain));

    Thread *pThread = GetThread();

    EX_TRY
    {
        struct _gc
        {
            PTRARRAYREF Array;
            OBJECTREF   InnerDelegate;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        GCPROTECT_BEGIN(gc);

        gc.Array = (PTRARRAYREF) ((DELEGATEREF)(*pDelegate))->GetInvocationList();
        if (gc.Array == NULL || !gc.Array->GetMethodTable()->IsArray())
        {
            InvokeNotify(pDelegate, pDomain);
        }
        else
        {
            // The _invocationCount could be less than the array size, if we are sharing
            // immutable arrays cleverly.
            DWORD      cnt = ((DELEGATEREF)(*pDelegate))->GetInvocationCount();

            _ASSERTE(cnt <= gc.Array->GetNumComponents());

            for (DWORD i=0; i<cnt; i++)
            {
                gc.InnerDelegate = gc.Array->m_Array[i];
                InvokeNotify(&gc.InnerDelegate, pDomain);
                if (pThread->IsAbortRequested())
                {
                    pThread->UnmarkThreadForAbort(Thread::TAR_Thread);
                }
            }
        }
        GCPROTECT_END();
    }
    EX_CATCH
    {
        // It's not even worth asserting, because these aren't our bugs.  At
        // some point, a MDA may be warranted.
    }
    EX_END_CATCH(SwallowAllExceptions)
}

// The unhandled exception event is a little easier to distribute, because
// we simply swallow any failures and proceed to the next event sink.
void DistributeUnhandledExceptionReliably(OBJECTREF *pDelegate,
                                          OBJECTREF *pDomain,
                                          OBJECTREF *pThrowable,
                                          BOOL       isTerminating)
{
    CONTRACTL
    {
        NOTHROW;  
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(pDelegate  != NULL && IsProtectedByGCFrame(pDelegate));
    _ASSERTE(pDomain    != NULL && IsProtectedByGCFrame(pDomain));
    _ASSERTE(pThrowable != NULL && IsProtectedByGCFrame(pThrowable));

    EX_TRY
    {
        struct _gc
        {
            PTRARRAYREF Array;
            OBJECTREF   InnerDelegate;
            OBJECTREF   EventArgs;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        GCPROTECT_BEGIN(gc);

        // Try to construct an UnhandledExceptionEventArgs out of pThrowable & isTerminating.
        // If unsuccessful, the best we can do is pass NULL.
        TryConstructUnhandledExceptionArgs(pThrowable, isTerminating, &gc.EventArgs);

        gc.Array = (PTRARRAYREF) ((DELEGATEREF)(*pDelegate))->GetInvocationList();
        if (gc.Array == NULL || !gc.Array->GetMethodTable()->IsArray())
        {
            InvokeUnhandledSwallowing(pDelegate, pDomain, &gc.EventArgs);
        }
        else
        {
            // The _invocationCount could be less than the array size, if we are sharing
            // immutable arrays cleverly.
            DWORD      cnt = ((DELEGATEREF)(*pDelegate))->GetInvocationCount();

            _ASSERTE(cnt <= gc.Array->GetNumComponents());

            for (DWORD i=0; i<cnt; i++)
            {
                gc.InnerDelegate = gc.Array->m_Array[i];
                InvokeUnhandledSwallowing(&gc.InnerDelegate, pDomain, &gc.EventArgs);
            }
        }
        GCPROTECT_END();
    }
    EX_CATCH
    {
        // It's not even worth asserting, because these aren't our bugs.  At
        // some point, a MDA may be warranted.
    }
    EX_END_CATCH(SwallowAllExceptions)
}



#endif // !DACCESS_COMPILE
