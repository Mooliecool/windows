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
/*
 *
 * COM+99 EE to Debugger Interface Implementation
 *
 */

#include "common.h"
#include "dbginterface.h"
#include "eedbginterfaceimpl.h"
#include "virtualcallstub.h"
#include "contractimpl.h"

#ifdef DEBUGGING_SUPPORTED

#ifndef DACCESS_COMPILE

//
// Cleanup any global data used by this interface.
//
void EEDbgInterfaceImpl::Terminate(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } 
    CONTRACTL_END;

    if (g_pEEDbgInterfaceImpl)
    {
        delete g_pEEDbgInterfaceImpl;
        g_pEEDbgInterfaceImpl = NULL;
    }
}

#endif // #ifndef DACCESS_COMPILE

Thread* EEDbgInterfaceImpl::GetThread(void) 
{ 
    CONTRACT(Thread *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    RETURN ::GetThread(); 
}

#ifndef DACCESS_COMPILE

void EEDbgInterfaceImpl::SetEEThreadPtr(VOID* newPtr)
{
    // Since this may be called from a Debugger Interop Hijack, the EEThread may be bogus.
    // Thus we can't use contracts. If we do fix that, then the contract below would be nice...
    // This should only be called by interop-debugging when we don't have an EE thread
    // object. 
    
    // Normally the LS & RS can communicate a pointer value using the EE thread's
    // m_debuggerWord field. If we have no EE thread, then we can use the 
    // TLS slot that the EE thread would have been in.
    UnsafeTlsSetValue(GetThreadTLSIndex(), newPtr);
}

StackWalkAction EEDbgInterfaceImpl::StackWalkFramesEx(Thread* pThread,
                                                      PREGDISPLAY pRD,
                                                      PSTACKWALKFRAMESCALLBACK pCallback,
                                                      VOID* pData,
                                                      unsigned int flags)
{ 
    CONTRACTL
    {
        DISABLED(NOTHROW); // FIX THIS when StackWalkFramesEx gets fixed.
        DISABLED(GC_TRIGGERS); // We cannot predict if pCallback will trigger or not. 
                               // Disabled is not a bug in this case.
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;

    return pThread->StackWalkFramesEx(pRD, pCallback, pData, flags); 
}

Frame *EEDbgInterfaceImpl::GetFrame(CrawlFrame *pCF)
{ 
    CONTRACT(Frame *)
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pCF));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    RETURN pCF->GetFrame(); 
}

bool EEDbgInterfaceImpl::InitRegDisplay(Thread* pThread, 
                                        const PREGDISPLAY pRD,
                                        const PCONTEXT pctx, 
                                        bool validContext)
{ 
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pRD));
        if (validContext)
        {
            PRECONDITION(CheckPointer(pctx));
        }
    }
    CONTRACTL_END;

    return pThread->InitRegDisplay(pRD, pctx, validContext); 
}

BOOL EEDbgInterfaceImpl::IsStringObject(Object* o)
{ 
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(o));
    }
    CONTRACTL_END;

    return (g_Mscorlib.IsClass(o->GetMethodTable(), CLASS__STRING)); 
}

BOOL EEDbgInterfaceImpl::IsTypedReference(MethodTable* pMT)
{ 
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;

    return (g_Mscorlib.IsClass(pMT, CLASS__TYPED_REFERENCE)); 
}

WCHAR* EEDbgInterfaceImpl::StringObjectGetBuffer(StringObject* so)
{ 
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(so));
    }
    CONTRACTL_END;

    return so->GetBuffer(); 
}

DWORD EEDbgInterfaceImpl::StringObjectGetStringLength(StringObject* so)
{ 
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(so));
    }
    CONTRACTL_END;

    return so->GetStringLength(); 
}

void* EEDbgInterfaceImpl::GetObjectFromHandle(OBJECTHANDLE handle)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    void *v;

    *((OBJECTREF *)&v) = *(OBJECTREF *)handle;

    return v;
}

OBJECTHANDLE EEDbgInterfaceImpl::GetHandleFromObject(void *obj, 
                                              bool fStrongNewRef, 
                                              AppDomain *pAppDomain)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;  // From CreateHandle
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pAppDomain));
    }
    CONTRACTL_END;

    OBJECTHANDLE oh;

    if (fStrongNewRef)
    {
        oh = pAppDomain->CreateStrongHandle(ObjectToOBJECTREF((Object *)obj));

        LOG((LF_CORDB, LL_INFO1000, "EEI::GHFO: Given objectref 0x%x,"
            "created strong handle 0x%x!\n", obj, oh));
    }
    else
    {
        oh = pAppDomain->CreateLongWeakHandle( ObjectToOBJECTREF((Object *)obj));

        LOG((LF_CORDB, LL_INFO1000, "EEI::GHFO: Given objectref 0x%x,"
            "created long weak handle 0x%x!\n", obj, oh));
    }

    return oh;
}

void EEDbgInterfaceImpl::DbgDestroyHandle(OBJECTHANDLE oh, 
                                          bool fStrongNewRef)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "EEI::GHFO: Destroyed given handle 0x%x,"
        "fStrong: 0x%x!\n", oh, fStrongNewRef));

    if (fStrongNewRef)
    {
        DestroyStrongHandle(oh);
    }
    else
    {
        DestroyLongWeakHandle(oh);
    }
}

    
OBJECTHANDLE EEDbgInterfaceImpl::GetThreadException(Thread *pThread)   
{   
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;

    OBJECTHANDLE oh = pThread->GetThrowableAsHandle();  

    if (oh != NULL)
    {
        return oh;
    }

    _ASSERTE("Non-WIN64 platforms should always have a current throwable");

    // Return the last thrown object if there's no current throwable.
    // This logic is similar to UpdateCurrentThrowable().
    return pThread->m_LastThrownObjectHandle;
}   

bool EEDbgInterfaceImpl::IsThreadExceptionNull(Thread *pThread) 
{
    CONTRACTL
    {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;

    //
    // Pure evil follows. We're assuming that the handle on the
    // thread is a strong handle and we're goona check it for
    // NULL. We're also assuming something about the
    // implementation of the handle here, too.
    //
    OBJECTHANDLE h = pThread->GetThrowableAsHandle();
    if (h == NULL)
    {
        return true;
    }

    void *pThrowable = *((void**)h);

    return (pThrowable == NULL);
}   

void EEDbgInterfaceImpl::ClearThreadException(Thread *pThread)  
{
    //
    LEAF_CONTRACT;
}   

bool EEDbgInterfaceImpl::StartSuspendForDebug(AppDomain *pAppDomain, 
                                              BOOL fHoldingThreadStoreLock) 
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO1000, "EEDbgII:SSFD: start suspend on AD:0x%x\n",
        pAppDomain));

    bool result = Thread::SysStartSuspendForDebug(pAppDomain);

    return result;  
}

bool EEDbgInterfaceImpl::SweepThreadsForDebug(bool forceSync)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        DISABLED(GC_TRIGGERS);  // Called by unmanaged threads.
    }
    CONTRACTL_END;

    return Thread::SysSweepThreadsForDebug(forceSync);
}

void EEDbgInterfaceImpl::ResumeFromDebug(AppDomain *pAppDomain) 
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Thread::SysResumeFromDebug(pAppDomain);
}

void EEDbgInterfaceImpl::MarkThreadForDebugSuspend(Thread* pRuntimeThread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pRuntimeThread));
    }
    CONTRACTL_END;

    pRuntimeThread->MarkForDebugSuspend();
}

void EEDbgInterfaceImpl::MarkThreadForDebugStepping(Thread* pRuntimeThread, 
                                                    bool onOff)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pRuntimeThread));
    }
    CONTRACTL_END;

    pRuntimeThread->MarkDebuggerIsStepping(onOff);
}
    
void EEDbgInterfaceImpl::SetThreadFilterContext(Thread *thread, 
                                                CONTEXT *context)   
{   
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;

    thread->SetFilterContext(context); 
}   

CONTEXT *EEDbgInterfaceImpl::GetThreadFilterContext(Thread *thread) 
{   
    CONTRACT(CONTEXT *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    RETURN thread->GetFilterContext();    
}

DWORD EEDbgInterfaceImpl::GetThreadDebuggerWord(Thread *thread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;

    return thread->m_debuggerWord;
}

void EEDbgInterfaceImpl::SetThreadDebuggerWord(Thread *thread, 
                                               DWORD dw)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;

    thread->m_debuggerWord = dw;
}

BOOL EEDbgInterfaceImpl::IsManagedNativeCode(const BYTE *address)
{ 
    return ExecutionManager::FindCodeMan((SLOT) address) != NULL; 
}

#endif // #ifndef DACCESS_COMPILE

MethodDesc *EEDbgInterfaceImpl::GetNativeCodeMethodDesc(const BYTE *address)
{ 
    CONTRACT(MethodDesc *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(address));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    MethodDesc* pMethodDesc;
    ExecutionManager::FindJitMan((SLOT) address)->JitCodeToMethodInfo((SLOT)address, &pMethodDesc); 
    RETURN pMethodDesc;
}

#ifndef DACCESS_COMPILE

// IsInPrologOrEpilog doesn't seem to be used for code that uses GC_INFO_DECODER
BOOL EEDbgInterfaceImpl::IsInPrologOrEpilog(const BYTE *address,
                                            size_t* prologSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    *prologSize = 0;

    IJitManager* pEEJM = ExecutionManager::FindJitMan((SLOT)address);   

    if (pEEJM != NULL)
    {
        METHODTOKEN methodtoken;
        DWORD relOffset;
        pEEJM->JitCodeToMethodInfo((SLOT)address, NULL, &methodtoken, &relOffset);
        LPVOID methodInfo = pEEJM->GetGCInfo(methodtoken);

        ICodeManager* codeMgrInstance = pEEJM->GetCodeManager();

        if (codeMgrInstance->IsInPrologOrEpilog(relOffset, methodInfo, prologSize))
        {
            return TRUE;
        }

    }

    return FALSE;
}

// 
// Given an IL offset, determines if that offset falls within a Filter or Handler in the given function
//
void EEDbgInterfaceImpl::DetermineIfOffsetsInFilterOrHandler(const BYTE *functionAddress,
                                                                  DebugOffsetToHandlerInfo *pOffsetToHandlerInfo, 
                                                                  unsigned offsetToHandlerInfoLength)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    IJitManager* pJitManager = ExecutionManager::FindJitMan((SLOT)functionAddress);   
    if (pJitManager == NULL)
    {
        return;
    }

    METHODTOKEN methodToken;
    pJitManager->JitCodeToMethodInfo((SLOT)functionAddress, NULL, &methodToken);

    EH_CLAUSE_ENUMERATOR pEnumState;
    unsigned EHCount = pJitManager->InitializeEHEnumeration(methodToken, &pEnumState);
    if (EHCount == 0)
    {
        return;
    }
    
    for (ULONG i=0; i < EHCount; i++)
    {
        EE_ILEXCEPTION_CLAUSE EHClause, *EHClausePtr;
        EHClausePtr = pJitManager->GetNextEHClause(methodToken,&pEnumState, &EHClause);

        for (ULONG j=0; j < offsetToHandlerInfoLength; j++)
        {
            SIZE_T offs = pOffsetToHandlerInfo[j].offset;

            // those with -1 indicate slots to skip
            if (offs == 0xffffffff)
            {
                continue;
            }
            // For a filter, the handler comes directly after it so check from start of filter 
            // to end of handler
            if (IsFilterHandler(EHClausePtr))
            {
                if (offs >= EHClausePtr->FilterOffset && offs < EHClausePtr->HandlerEndPC)
                {
                    pOffsetToHandlerInfo[j].isInFilterOrHandler = TRUE;
                }
            }
            // For anything else, only care about handler range
            else if (offs >= EHClausePtr->HandlerStartPC && offs < EHClausePtr->HandlerEndPC)
            {
                pOffsetToHandlerInfo[j].isInFilterOrHandler = TRUE;
            }
        }
    }
}
#endif // #ifndef DACCESS_COMPILE

void EEDbgInterfaceImpl::GetMethodRegionInfo(const BYTE *pStart, 
                                             BYTE **pCold, 
                                             size_t *hotSize, 
                                             size_t *coldSize)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pCold));
        PRECONDITION(CheckPointer(hotSize));
        PRECONDITION(CheckPointer(coldSize));
    }
    CONTRACTL_END;

    IJitManager::MethodRegionInfo methodRegionInfo = {NULL, 0, NULL, 0};
    IJitManager* pEEJM = ExecutionManager::FindJitMan((SLOT)pStart);   

    METHODTOKEN methodtoken;

    _ASSERTE(pEEJM != NULL);
    if (pEEJM != NULL)
    {
        pEEJM->JitCodeToMethodInfo((SLOT)pStart, 
                                   NULL, 
                                   &methodtoken
                                   );
        _ASSERTE(methodtoken);

        pEEJM->JitTokenToMethodRegionInfo(methodtoken, &methodRegionInfo);
    }

    *pCold    = methodRegionInfo.coldStartAddress;
    *hotSize  = methodRegionInfo.hotSize;
    *coldSize = methodRegionInfo.coldSize;
}


size_t EEDbgInterfaceImpl::GetFunctionSize(MethodDesc *pFD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
    }
    CONTRACTL_END;

    IJitManager* pEEJM;

    LPVOID methodStart = (LPVOID) pFD->GetFunctionAddress(&pEEJM);

    if (methodStart == NULL)
        return 0;

    METHODTOKEN methodtoken;
    pEEJM->JitCodeToMethodInfo((SLOT)methodStart, NULL, &methodtoken);

    LPVOID methodInfo = pEEJM->GetGCInfo(methodtoken);

    ICodeManager* codeMgrInstance = pEEJM->GetCodeManager();

    return codeMgrInstance->GetFunctionSize(methodInfo);
}

const BYTE* EEDbgInterfaceImpl::GetFunctionAddress(MethodDesc *pFD)
{ 
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
    }
    CONTRACTL_END;

    return pFD->GetFunctionAddress();
}

#ifndef DACCESS_COMPILE

void EEDbgInterfaceImpl::OnDebuggerTripThread(void) 
{ 
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    ::OnDebuggerTripThread(); 
}   

void EEDbgInterfaceImpl::DisablePreemptiveGC(void) 
{ 
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        DISABLED(GC_TRIGGERS); // Disabled because disabled in RareDisablePreemptiveGC()
    }
    CONTRACTL_END;

    ::GetThread()->DisablePreemptiveGC(); 
}    

void EEDbgInterfaceImpl::EnablePreemptiveGC(void) 
{ 
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        DISABLED(GC_TRIGGERS); // Disabled because disabled in RareEnablePreemptiveGC()
    }
    CONTRACTL_END;

    ::GetThread()->EnablePreemptiveGC(); 
}  

bool EEDbgInterfaceImpl::IsPreemptiveGCDisabled(void)   
{ 
    CONTRACTL
    {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ::GetThread()->m_fPreemptiveGCDisabled != 0; 
}   

DWORD EEDbgInterfaceImpl::MethodDescIsStatic(MethodDesc *pFD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
    }
    CONTRACTL_END;
    
    return pFD->IsStatic();
}

#endif // #ifndef DACCESS_COMPILE

Module *EEDbgInterfaceImpl::MethodDescGetModule(MethodDesc *pFD)
{
    CONTRACT(Module *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    RETURN pFD->GetModule();
}

#ifndef DACCESS_COMPILE

COR_ILMETHOD* EEDbgInterfaceImpl::MethodDescGetILHeader(MethodDesc *pFD)
{
    CONTRACT(COR_ILMETHOD *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    if (pFD->IsIL())
    {
        RETURN pFD->GetILHeader();
    }

    RETURN NULL;
}

ULONG EEDbgInterfaceImpl::MethodDescGetRVA(MethodDesc *pFD)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pFD));
    }
    CONTRACTL_END;

    return pFD->GetRVA();
}

MethodDesc *EEDbgInterfaceImpl::FindLoadedMethodRefOrDef(Module* pModule,
                                                          mdToken memberRef)
{
    CONTRACT(MethodDesc *)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    // Must have a MemberRef or a MethodDef
    mdToken tkType = TypeFromToken(memberRef);
    _ASSERTE((tkType == mdtMemberRef) || (tkType == mdtMethodDef));

    if (tkType == mdtMemberRef) 
    {
        RETURN pModule->LookupMemberRefAsMethod(memberRef);
    }

    RETURN pModule->LookupMethodDef(memberRef);
}

MethodDesc *EEDbgInterfaceImpl::LoadMethodDef(Module* pModule,
                                              mdMethodDef methodDef, 
                                              DWORD numGenericArgs, 
                                              TypeHandle *pGenericArgs,
                                              TypeHandle *pOwnerType)
{
    CONTRACT(MethodDesc *)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    _ASSERTE(TypeFromToken(methodDef) == mdtMethodDef);

    // The generic class and method args are sent as one array
    // by the debugger.  We now split this into two by finding out how
    // many generic args are for the class and how many for the 
    // method.  The actual final checks are done in MemberLoader::GetMethodDescFromMethodDefThrowing.

    DWORD numGenericClassArgs = 0;
    TypeHandle *pGenericClassArgs = NULL;
    DWORD nGenericMethodArgs = 0;
    TypeHandle *pGenericMethodArgs = NULL;
    mdTypeDef typeDef = 0;

    if (numGenericArgs != 0)
    {
        HRESULT hr = pModule->GetMDImport()->GetParentToken(methodDef, &typeDef); 
        if (FAILED(hr)) 
            COMPlusThrowHR(E_INVALIDARG);

        TypeHandle thClass = LoadClass(pModule, typeDef);
        if (thClass.GetClass() == NULL)
            COMPlusThrowHR(COR_E_TYPELOAD);

        numGenericClassArgs = thClass.GetNumGenericArgs();
        pGenericClassArgs = (numGenericClassArgs > 0) ? pGenericArgs : NULL;
        nGenericMethodArgs = (numGenericArgs >= numGenericClassArgs) ? (numGenericArgs - numGenericClassArgs) : 0;
        pGenericMethodArgs = (nGenericMethodArgs > 0) ? (pGenericArgs + numGenericClassArgs) : NULL;
 
    }

    MethodDesc *pRes = MemberLoader::GetMethodDescFromMethodDefThrowing(pModule, 
                                                                        methodDef, 
                                                                        numGenericClassArgs, 
                                                                        pGenericClassArgs, 
                                                                        nGenericMethodArgs, 
                                                                        pGenericMethodArgs);

    // The ownerType is extra information that augments the specification of an interface MD.
    // It is only needed if generics code sharing is supported, becuase otherwise MDs are
    // fully self-describing.
    if (pOwnerType != NULL)
    {
        if (pRes != NULL)
        {
            if (numGenericClassArgs != 0)
            {
                *pOwnerType = ClassLoader::LoadGenericInstantiationThrowing(pModule, typeDef, numGenericClassArgs, pGenericClassArgs);
            }
            else
            {
                *pOwnerType = TypeHandle(pRes->GetMethodTable());
            }
        }
        else
        {
            *pOwnerType = TypeHandle();
        }
    }
    RETURN (pRes);

}


TypeHandle EEDbgInterfaceImpl::FindLoadedClass(Module *pModule, 
                                             mdTypeDef classToken)
{
    CONTRACT(TypeHandle)
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACT_END;
    
    RETURN ClassLoader::LookupTypeDefOrRefInModule(pModule, classToken);

}

TypeHandle EEDbgInterfaceImpl::FindLoadedInstantiation(Module *pModule,
                                                       mdTypeDef typeDef,
                                                       DWORD ntypars,
                                                       TypeHandle *inst)
{
    //
    // Lookup operations run the class loader in non-load mode.
    //
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    return ClassLoader::LoadGenericInstantiationNoThrow(pModule, 
                                                        typeDef, 
                                                        ntypars, 
                                                        inst, 
                                                        NULL,
                                                        ClassLoader::DontLoadTypes);
}

TypeHandle EEDbgInterfaceImpl::FindLoadedFnptrType(TypeHandle *inst, 
                                                   DWORD ntypars)
{
    // Lookup operations run the class loader in non-load mode.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    return ClassLoader::LoadFnptrTypeThrowing(0, ntypars, inst, 
                                              // == FailIfNotLoadedOrNotRestored
                                              ClassLoader::DontLoadTypes);
}

TypeHandle EEDbgInterfaceImpl::FindLoadedPointerOrByrefType(CorElementType et, 
                                                            TypeHandle elemtype)
{
    // Lookup operations run the class loader in non-load mode.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    return ClassLoader::LoadPointerOrByrefTypeThrowing(et, elemtype, 
                                                       // == FailIfNotLoadedOrNotRestored
                                                       ClassLoader::DontLoadTypes);
}

TypeHandle EEDbgInterfaceImpl::FindLoadedArrayType(CorElementType et, 
                                                   TypeHandle elemtype, 
                                                   unsigned rank)
{
    // Lookup operations run the class loader in non-load mode.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    if (elemtype.IsNull())
        return TypeHandle();
    else
        return ClassLoader::LoadArrayTypeThrowing(elemtype, et, rank, 
                                                  // == FailIfNotLoadedOrNotRestored
                                                  ClassLoader::DontLoadTypes );
}


TypeHandle EEDbgInterfaceImpl::FindLoadedElementType(CorElementType et) 
{
    // Lookup operations run the class loader in non-load mode.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    MethodTable *m = g_Mscorlib.LookupElementType(et);
    if (m == NULL)
    {
        return TypeHandle();
    }

    return TypeHandle(m);
}
    
TypeHandle EEDbgInterfaceImpl::LoadClass(Module *pModule, 
                                       mdTypeDef classToken)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACT_END;
    
    RETURN ClassLoader::LoadTypeDefOrRefThrowing(pModule, classToken, 
                                                          ClassLoader::ThrowIfNotFound, 
                                                          ClassLoader::PermitUninstDefOrRef);

}

TypeHandle EEDbgInterfaceImpl::LoadInstantiation(Module *pModule,
                                                 mdTypeDef typeDef,
                                                 DWORD ntypars,
                                                 TypeHandle *inst)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACT_END;
    
    RETURN ClassLoader::LoadGenericInstantiationThrowing(pModule, typeDef, ntypars, inst);
}

TypeHandle EEDbgInterfaceImpl::LoadArrayType(CorElementType et,
                                             TypeHandle elemtype, 
                                             unsigned rank)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;
    
    if (elemtype.IsNull())
        RETURN TypeHandle();
    else
        RETURN ClassLoader::LoadArrayTypeThrowing(elemtype, et, rank);
}

TypeHandle EEDbgInterfaceImpl::LoadPointerOrByrefType(CorElementType et, 
                                                      TypeHandle elemtype)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    return ClassLoader::LoadPointerOrByrefTypeThrowing(et, elemtype);
}

TypeHandle EEDbgInterfaceImpl::LoadFnptrType(TypeHandle *inst, 
                                             DWORD ntypars)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    /* @TODO akenn: CALLCONV? */
    return ClassLoader::LoadFnptrTypeThrowing(0, ntypars, inst);
}

TypeHandle EEDbgInterfaceImpl::LoadElementType(CorElementType et) 
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    MethodTable *m = g_Mscorlib.GetElementType(et);
    
    if (m == NULL)
    {
        return TypeHandle();
    }

    return TypeHandle(m);
}


HRESULT EEDbgInterfaceImpl::GetMethodImplProps(Module *pModule, 
                                               mdToken tk,
                                               DWORD *pRVA, 
                                               DWORD *pImplFlags)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;
    
    pModule->GetMDImport()->GetMethodImplProps(tk, pRVA, pImplFlags);
    return S_OK;
}

HRESULT EEDbgInterfaceImpl::GetParentToken(Module *pModule, 
                                           mdToken tk, 
                                           mdToken *pParentToken)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;
    
    return pModule->GetMDImport()->GetParentToken(tk, pParentToken);
}
    
void EEDbgInterfaceImpl::MarkDebuggerAttached(void)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    g_CORDebuggerControlFlags |= DBCF_ATTACHED;
}

void EEDbgInterfaceImpl::MarkDebuggerUnattached(void)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    g_CORDebuggerControlFlags &= ~DBCF_ATTACHED;
}

HRESULT EEDbgInterfaceImpl::IterateThreadsForAttach(BOOL *fEventSent, 
                                                    BOOL fAttaching)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(fEventSent));
    }
    CONTRACTL_END;
    
    LOG((LF_CORDB, LL_INFO10000, "EEDII:ITFA: Entered function IterateThreadsForAttach()\n"));

    HRESULT hr = E_FAIL;

    Thread *pThread = NULL;

    while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
    {
        // Does the thread belong to an app domain to which we are attaching?


        Thread::ThreadState ts = pThread->GetSnapshotState();

        // Don't send up events for dead or unstarted
        // threads. There is no race between unstarted threads
        // and missing a thread create event, since we setup a
        // DebuggerThreadStarter right after we remove
        // TS_Unstarted.
        if (!((ts & Thread::TS_Dead) || (ts & Thread::TS_Unstarted)))
        {
            LOG((LF_CORDB, LL_INFO10000, "EEDII:ITFA: g_pDebugInterface->ThreadStarted() for [0x%x] "
                "Thread dead : %s / Thread unstarted: %s (0x%08x) / AD: %(0x%08x)\n",
                pThread->GetThreadId(),
                (ts & Thread::TS_Dead)?"TRUE":"FALSE",
                (ts & Thread::TS_Unstarted)?"TRUE":"FALSE",
                ts,
                pThread->GetDomain()->GetDebuggerAttached()));

            g_pDebugInterface->ThreadStarted(pThread, fAttaching);
            *fEventSent = TRUE;
            hr = S_OK;
        }
        else
        {
            LOG((LF_CORDB, LL_INFO10000, "EEDII:ITFA: g_pDebugInterface->ThreadStarted() not called for [0x%x] "
                "Thread dead : %s / Thread unstarted: %s (0x%08x) / AD: %(0x%08x)\n",
                pThread->GetThreadId(),
                (ts & Thread::TS_Dead)?"TRUE":"FALSE",
                (ts & Thread::TS_Unstarted)?"TRUE":"FALSE",
                ts,
                pThread->GetDomain()->GetDebuggerAttached()));
        }

    }

    if (SUCCEEDED(hr))
    {
        hr = CCLRDebugManager::IterateConnectionForAttach(fAttaching);
    }

    return hr;
}


bool EEDbgInterfaceImpl::CrawlFrameIsGcSafe(CrawlFrame *pCF)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pCF));
    }
    CONTRACTL_END;
    
    return pCF->IsGcSafe();
}

bool EEDbgInterfaceImpl::IsStub(const BYTE *ip)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // IsStub will catch any exceptions and return false.
    return StubManager::IsStub((TADDR) ip) != FALSE;
}

#endif // #ifndef DACCESS_COMPILE

// static
bool EEDbgInterfaceImpl::DetectHandleILStubs(Thread *thread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return thread->DetectHandleILStubsForDebugger();
}

bool EEDbgInterfaceImpl::TraceStub(const BYTE *ip, 
                                   TraceDestination *trace)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    return StubManager::TraceStub((TADDR) ip, trace) != FALSE;
#else
    DacNotImpl();
    return false;
#endif // #ifndef DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE

bool EEDbgInterfaceImpl::FollowTrace(TraceDestination *trace)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    return StubManager::FollowTrace(trace) != FALSE;
}

bool EEDbgInterfaceImpl::TraceFrame(Thread *thread, 
                                    Frame *frame, 
                                    BOOL fromPatch, 
                                    TraceDestination *trace, 
                                    REGDISPLAY *regs)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        DISABLED(GC_TRIGGERS);  // This is not a bug - the debugger can call this on an un-managed thread.
        PRECONDITION(CheckPointer(frame));
    }
    CONTRACTL_END;

    bool fResult = frame->TraceFrame(thread, fromPatch, trace, regs) != FALSE;

#ifdef _DEBUG
    StubManager::DbgWriteLog("Doing TraceFrame on frame=0x%p (fromPatch=%d), yeilds:\n", frame, fromPatch);
    if (fResult)
    {
        SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;    
        FAULT_NOT_FATAL();
        SString buffer;
        StubManager::DbgWriteLog("  td=%S\n", trace->DbgToString(buffer));
    }
    else
    {
        StubManager::DbgWriteLog("  false (this frame does not expect to call managed code).\n");
    }
#endif        
    return fResult;
}

bool EEDbgInterfaceImpl::TraceManager(Thread *thread, 
                                      StubManager *stubManager,
                                      TraceDestination *trace, 
                                      CONTEXT *context,
                                      BYTE **pRetAddr)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(stubManager));
    }
    CONTRACTL_END;

    bool fResult =  stubManager->TraceManager(thread, trace, context, pRetAddr) != FALSE;

#ifdef _DEBUG
    StubManager::DbgWriteLog("Doing TraceManager on %s (0x%p) for IP=0x%p, yields:\n", stubManager->DbgGetName(), stubManager, GetIP(context));
    if (fResult)
    {
        // Should never be on helper thread
        FAULT_NOT_FATAL();
        SString buffer;
        StubManager::DbgWriteLog("  td=%S\n", trace->DbgToString(buffer));
    }
    else
    {
        StubManager::DbgWriteLog("  false (this stub does not expect to call managed code).\n");
    }
#endif    
    return fResult;
}

void EEDbgInterfaceImpl::EnableTraceCall(Thread *thread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;
    
    thread->IncrementTraceCallCount();
}

bool EEDbgInterfaceImpl::GetVirtualTraceCallTargets(TADDR** ppTargets, DWORD* pdwNumTargets)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return StubManager::GetVirtualTraceCallTargets(ppTargets, pdwNumTargets);
}

void EEDbgInterfaceImpl::DisableTraceCall(Thread *thread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(thread));
    }
    CONTRACTL_END;
    
    thread->DecrementTraceCallCount();
}

void EEDbgInterfaceImpl::GetRuntimeOffsets(SIZE_T *pTLSIndex,
                                           SIZE_T *pTLSIsSpecialIndex,
                                           SIZE_T *pTLSCantStopIndex,
                                           SIZE_T* pTLSIndexOfPredefs,
                                           SIZE_T *pEEThreadStateOffset,
                                           SIZE_T *pEEThreadStateNCOffset,
                                           SIZE_T *pEEThreadPGCDisabledOffset,
                                           DWORD  *pEEThreadPGCDisabledValue,
                                           SIZE_T *pEEThreadDebuggerWordOffset,
                                           SIZE_T *pEEThreadFrameOffset,
                                           SIZE_T *pEEThreadMaxNeededSize,
                                           DWORD  *pEEThreadSteppingStateMask,
                                           DWORD  *pEEMaxFrameValue,
                                           SIZE_T *pEEThreadDebuggerFilterContextOffset,
                                           SIZE_T *pEEThreadCantStopOffset,
                                           SIZE_T *pEEFrameNextOffset,
                                           DWORD  *pEEIsManagedExceptionStateMask)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pTLSIndex));
        PRECONDITION(CheckPointer(pTLSIsSpecialIndex));
        PRECONDITION(CheckPointer(pEEThreadStateOffset));
        PRECONDITION(CheckPointer(pTLSIndexOfPredefs));
        PRECONDITION(CheckPointer(pEEThreadStateNCOffset));
        PRECONDITION(CheckPointer(pEEThreadPGCDisabledOffset));
        PRECONDITION(CheckPointer(pEEThreadPGCDisabledValue));
        PRECONDITION(CheckPointer(pEEThreadDebuggerWordOffset));
        PRECONDITION(CheckPointer(pEEThreadFrameOffset));
        PRECONDITION(CheckPointer(pEEThreadMaxNeededSize));
        PRECONDITION(CheckPointer(pEEThreadSteppingStateMask));
        PRECONDITION(CheckPointer(pEEMaxFrameValue));
        PRECONDITION(CheckPointer(pEEThreadDebuggerFilterContextOffset));
        PRECONDITION(CheckPointer(pEEThreadCantStopOffset));
        PRECONDITION(CheckPointer(pEEFrameNextOffset));
        PRECONDITION(CheckPointer(pEEIsManagedExceptionStateMask));
    }
    CONTRACTL_END;
    
    *pTLSIndex = GetThreadTLSIndex();
    *pTLSIsSpecialIndex = TlsIdx_ThreadType;
    *pTLSCantStopIndex = TlsIdx_CantStopCount;
    *pTLSIndexOfPredefs = CExecutionEngine::TlsIndex;
    *pEEThreadStateOffset = Thread::GetOffsetOfState();
    *pEEThreadStateNCOffset = Thread::GetOffsetOfStateNC();
    *pEEThreadPGCDisabledOffset = Thread::GetOffsetOfGCFlag();
    *pEEThreadPGCDisabledValue = 1; // A little obvious, but just in case...
    *pEEThreadDebuggerWordOffset = Thread::GetOffsetOfDebuggerWord();
    *pEEThreadFrameOffset = Thread::GetOffsetOfCurrentFrame();
    *pEEThreadMaxNeededSize = sizeof(Thread);
    *pEEThreadDebuggerFilterContextOffset = Thread::GetOffsetOfDebuggerFilterContext();
    *pEEThreadCantStopOffset = Thread::GetOffsetOfCantStop();
    *pEEThreadSteppingStateMask = Thread::TSNC_DebuggerIsStepping;
    *pEEMaxFrameValue = (DWORD)(size_t)FRAME_TOP;
    *pEEFrameNextOffset = Frame::GetOffsetOfNextLink();
    *pEEIsManagedExceptionStateMask = Thread::TSNC_DebuggerIsManagedException;
}

void EEDbgInterfaceImpl::DebuggerModifyingLogSwitch (int iNewLevel, 
                                                     const WCHAR *pLogSwitchName)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    Log::DebuggerModifyingLogSwitch (iNewLevel, pLogSwitchName);
}


HRESULT EEDbgInterfaceImpl::SetIPFromSrcToDst(Thread *pThread,
                                              IJitManager* pIJM,
                                              METHODTOKEN MethodToken,
                                              SLOT addrStart,      
                                              DWORD offFrom,        
                                              DWORD offTo,          
                                              bool fCanSetIPOnly,   
                                              PREGDISPLAY pReg,
                                              PCONTEXT pCtx,
                                              DWORD methodSize,
                                              void *firstExceptionHandler,
                                              void *pDji,
                                              EHRangeTree *pEHRT)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    return ::SetIPFromSrcToDst(pThread,
                               pIJM,
                               MethodToken,
                               addrStart,      
                               offFrom,        
                               offTo,          
                               fCanSetIPOnly,   
                               pReg,
                               pCtx,
                               methodSize,
                               firstExceptionHandler,
                               pDji,
                               pEHRT);

}

void EEDbgInterfaceImpl::SetDebugState(Thread *pThread, 
                                       CorDebugThreadState state)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;
    
    _ASSERTE(state == THREAD_SUSPEND || state == THREAD_RUN);

    LOG((LF_CORDB,LL_INFO10000,"EEDbg:Setting thread 0x%x (ID:0x%x) to 0x%x\n", pThread, pThread->GetThreadId(), state));

    if (state == THREAD_SUSPEND)
    {
        pThread->SetThreadStateNC(Thread::TSNC_DebuggerUserSuspend);
    }
    else
    {
        pThread->ResetThreadStateNC(Thread::TSNC_DebuggerUserSuspend);
    }
}

void EEDbgInterfaceImpl::SetAllDebugState(Thread *et, 
                                          CorDebugThreadState state)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    Thread *pThread = NULL;

    while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
    {
        if (pThread != et)
        {
            SetDebugState(pThread, state);
        }
    }
}

// This is pretty much copied from VM\COMSynchronizable's
// INT32 __stdcall ThreadNative::GetThreadState, so propogate changes
// to both functions
// This just gets the user state from the EE's perspective (hence "partial").
CorDebugUserState EEDbgInterfaceImpl::GetPartialUserState(Thread *pThread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pThread));
    }
    CONTRACTL_END;
    
    Thread::ThreadState ts = pThread->GetSnapshotState();
    unsigned ret = 0;

    if (ts & Thread::TS_Background)
    {
        ret |= (unsigned)USER_BACKGROUND;
    }

    if (ts & Thread::TS_Unstarted)
    {
        ret |= (unsigned)USER_UNSTARTED;            
    }

    // Don't report a StopRequested if the thread has actually stopped.
    if (ts & Thread::TS_Dead)
    {
        ret |= (unsigned)USER_STOPPED;           
    }

    if (ts & Thread::TS_Interruptible)
    {
        ret |= (unsigned)USER_WAIT_SLEEP_JOIN;          
    }

    // Don't report a SuspendRequested if the thread has actually Suspended.
    if ( ((ts & Thread::TS_UserSuspendPending) && (ts & Thread::TS_SyncSuspended)))
    {
        ret |= (unsigned)USER_SUSPENDED;
    }
    else if (ts & Thread::TS_UserSuspendPending)
    {
        ret |= (unsigned)USER_SUSPEND_REQUESTED;
    }

    LOG((LF_CORDB,LL_INFO1000, "EEDbgII::GUS: thread 0x%x (id:0x%x)"
        " userThreadState is 0x%x\n", pThread, pThread->GetThreadId(), ret));

    return (CorDebugUserState)ret;
}

HRESULT EEDbgInterfaceImpl::FilterEnCBreakpointsByEH(DebuggerILToNativeMap   *m_sequenceMap,
                                                     unsigned int             m_sequenceMapCount,
                                                     COR_ILMETHOD_DECODER    *pMethodDecoderOld,
                                                     CORDB_ADDRESS            addrOfCode,
                                                     METHODTOKEN              methodToken,
                                                     DWORD                    methodSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    return S_OK;

    //
    //  At this point, we'd want to go in, and detect if an EnC will change
    //  a method so that the EH structure had changed illegally. The way 
    //  I was planning on doing this was to create an EHRangeTree for the old &
    //  new versions, then make sure that the structure hadn't changed, then
    //  make sure that the location within the EH tree of an old IL offset, when
    //  mapped through the old to new IL map, ends up in the corresponding location
    //  in the new tree.  All the sequence points that satisfy these constraints are
    //  marked "ok", everything else is marked "bad", and we don't set EnC BPs
    //  at the "bad" ones.  We'll also have to change DispatchPatchOrSingleStep
    //  so we don't short-circuit when we're not supposed to.
    //  This prevent us from allowing an EnC that changes the EH layout (the next
    //  time the user invokes the function things'll go fine).
    //
    //        return ::FilterEnCBreakpointsByEH(m_sequenceMap,
    //                                          m_sequenceMapCount,
    //                                          pMethodDecoderOld,
    //                                          pAddrOfCode,
    //                                          methodToken,
    //                                          methodSize);
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
EEDbgInterfaceImpl::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_VTHIS();
}

#endif

unsigned EEDbgInterfaceImpl::GetSizeForCorElementType(CorElementType etyp)
{
    WRAPPER_CONTRACT;

    return (::GetSizeForCorElementType(etyp));
}


#ifndef DACCESS_COMPILE
/*
 * ObjIsInstanceOf
 *
 * This method supplies the internal VM implementation of this method to the 
 * debugger left-side.
 *
 */
BOOL EEDbgInterfaceImpl::ObjIsInstanceOf(Object *pElement, TypeHandle toTypeHnd)
{
    WRAPPER_CONTRACT;

    return (::ObjIsInstanceOf(pElement, toTypeHnd));
}
#endif
    
/*
 * ClearAllDebugInterfaceReferences
 *
 * This method is called by the debugging part of the runtime to notify
 * that the debugger resources are no longer valid and any internal references
 * to it must be null'ed out.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   None.
 *
 */
void EEDbgInterfaceImpl::ClearAllDebugInterfaceReferences()
{
    LEAF_CONTRACT;

    g_pDebugInterface = NULL;
}

#ifndef DACCESS_COMPILE
#ifdef _DEBUG
/*
 * ObjectRefFlush
 *
 * Flushes all debug tracking information for object referencing.
 *
 * Parameters:
 *   pThread - The target thread to flush object references of.
 *
 * Returns:
 *   None.
 *
 */
void EEDbgInterfaceImpl::ObjectRefFlush(Thread *pThread)
{
    WRAPPER_CONTRACT;

    Thread::ObjectRefFlush(pThread);
}
#endif
#endif

#endif // DEBUGGING_SUPPORTED
