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
/*============================================================
**
** Header: COMThreadPool.cpp
**
**                                                  
**
** Purpose: Native methods on System.ThreadPool
**          and its inner classes
**
** Date:  August, 1999
**
===========================================================*/

/********************************************************************************************************************/
#include "common.h"
#include "comdelegate.h"
#include "comthreadpool.h"
#include "win32threadpool.h"
#include "class.h"
#include "object.h"
#include "field.h"
#include "excep.h"
#include "security.h"
#include "eeconfig.h"
#include "corhost.h"
#include "nativeoverlapped.h"
#include "crossdomaincalls.h"
#include "appdomain.inl"
/*****************************************************************************************************/
#ifdef _DEBUG
void LogCall(MethodDesc* pMD, LPCUTF8 api)
{
    LEAF_CONTRACT;
    
    LPCUTF8 cls  = pMD->GetClass() ? pMD->GetClass()->GetDebugClassName()
                                   : "GlobalFunction";
    LPCUTF8 name = pMD->GetName();

    LOG((LF_THREADPOOL,LL_INFO1000,"%s: ", api));
    LOG((LF_THREADPOOL, LL_INFO1000,
         " calling %s.%s\n", cls, name));
}
#else
#define LogCall(pMd,api)
#endif

/*****************************************************************************************************/
// Caller has to GC protect Objectrefs being passed in
DelegateInfo *DelegateInfo::MakeDelegateInfo(AppDomain *pAppDomain,
                                             OBJECTREF *state,
                                             OBJECTREF *waitEvent,
                                             OBJECTREF *registeredWaitHandle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsProtectedByGCFrame(state));
        PRECONDITION(waitEvent == NULL || IsProtectedByGCFrame(waitEvent));
        PRECONDITION(registeredWaitHandle == NULL || IsProtectedByGCFrame(registeredWaitHandle));
        PRECONDITION(CheckPointer(pAppDomain));
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    // If there were any DelegateInfos waiting to be released, they'll get flushed now
    ThreadpoolMgr::FlushQueueOfTimerInfos();
    
    DelegateInfo* delegateInfo = (DelegateInfo*) ThreadpoolMgr::GetRecycledMemory(ThreadpoolMgr::MEMTYPE_DelegateInfo);
    delegateInfo->m_appDomainId = pAppDomain->GetId();

    delegateInfo->m_stateHandle = pAppDomain->CreateHandle(*state);

    if (waitEvent != NULL)
    {
        delegateInfo->m_eventHandle = pAppDomain->CreateHandle(*waitEvent);
    }
    else
        delegateInfo->m_eventHandle = NULL;

    if (registeredWaitHandle != NULL)
    {
        delegateInfo->m_registeredWaitHandle = pAppDomain->CreateHandle(*registeredWaitHandle);
    }
    else
    	 delegateInfo->m_registeredWaitHandle = NULL;

    delegateInfo->m_overridesCount = 0;
    delegateInfo->m_hasSecurityInfo = FALSE;

    return delegateInfo;
}

/*****************************************************************************************************/
FCIMPL2(FC_BOOL_RET, ThreadPoolNative::CorSetMaxThreads,DWORD workerThreads, DWORD completionPortThreads)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    BOOL bRet = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_0(); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW

    bRet = ThreadpoolMgr::SetMaxThreads(workerThreads,completionPortThreads);
    HELPER_METHOD_FRAME_END();    
    FC_RETURN_BOOL(bRet);
}
FCIMPLEND

/*****************************************************************************************************/
FCIMPL2(VOID, ThreadPoolNative::CorGetMaxThreads,DWORD* workerThreads, DWORD* completionPortThreads)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT; 
    
    ThreadpoolMgr::GetMaxThreads(workerThreads,completionPortThreads);
    return;
}
FCIMPLEND

/*****************************************************************************************************/
FCIMPL2(FC_BOOL_RET, ThreadPoolNative::CorSetMinThreads,DWORD workerThreads, DWORD completionPortThreads)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    BOOL bRet = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_0(); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW

    bRet = ThreadpoolMgr::SetMinThreads(workerThreads,completionPortThreads);
    HELPER_METHOD_FRAME_END();    
    FC_RETURN_BOOL(bRet);
}
FCIMPLEND

/*****************************************************************************************************/
FCIMPL2(VOID, ThreadPoolNative::CorGetMinThreads,DWORD* workerThreads, DWORD* completionPortThreads)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    ThreadpoolMgr::GetMinThreads(workerThreads,completionPortThreads);
    return;
}
FCIMPLEND

/*****************************************************************************************************/
FCIMPL2(VOID, ThreadPoolNative::CorGetAvailableThreads,DWORD* workerThreads, DWORD* completionPortThreads)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    ThreadpoolMgr::GetAvailableThreads(workerThreads,completionPortThreads);
    return;
}
FCIMPLEND

/*****************************************************************************************************/

struct RegisterWaitForSingleObjectCallback_Args
{
    DelegateInfo *delegateInfo;
    BOOL TimerOrWaitFired;
};

static VOID
RegisterWaitForSingleObjectCallback_Worker(LPVOID ptr)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    OBJECTREF orState = NULL;

    GCPROTECT_BEGIN( orState );

    RegisterWaitForSingleObjectCallback_Args *args = (RegisterWaitForSingleObjectCallback_Args *) ptr;
    orState = ObjectFromHandle(((DelegateInfo*) args->delegateInfo)->m_stateHandle);

#ifdef _DEBUG
    MethodDesc *pMeth = g_Mscorlib.GetMethod(METHOD__TPWAITORTIMER_HELPER__PERFORM_WAITORTIMER_CALLBACK);
    LogCall(pMeth,"RWSOCallback");
#endif

    // Caution: the args are not protected, we have to garantee there's no GC from here till
    // the managed call happens.
    PREPARE_NONVIRTUAL_CALLSITE(METHOD__TPWAITORTIMER_HELPER__PERFORM_WAITORTIMER_CALLBACK);
    DECLARE_ARGHOLDER_ARRAY(arg, 2);
    arg[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(orState);
    arg[ARGNUM_1]  = DWORD_TO_ARGHOLDER(args->TimerOrWaitFired);

    // Call the method...
    CALL_MANAGED_METHOD_NORET(arg);

    GCPROTECT_END();
}


void ResetThreadSecurityState(Thread* pThread)
{
    CONTRACTL 
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;
    
    if (pThread)
    {
        pThread->ResetSecurityInfo();
    }
}

// this holder resets our thread's security state
typedef Holder<Thread*, DoNothing<Thread*>, ResetThreadSecurityState> ThreadSecurityStateHolder;

VOID WINAPI RegisterWaitForSingleObjectCallback(PVOID delegateInfo, BOOL TimerOrWaitFired)
{
    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread);
        if (pThread == NULL) {
            return;
        }
    }

    CONTRACTL
    {
        MODE_PREEMPTIVE;    // Worker thread will be in preempt mode. We switch to coop below.
        THROWS;
        GC_TRIGGERS;

        PRECONDITION(CheckPointer(delegateInfo));
    }
    CONTRACTL_END;

    // This thread should not have any locks held at entry point.
    _ASSERTE(pThread->m_dwLockCount == 0);

    GCX_COOP();

    // this holder resets our thread's security state when exiting this scope
    ThreadSecurityStateHolder  secState(pThread);

    RegisterWaitForSingleObjectCallback_Args args = { ((DelegateInfo*) delegateInfo), TimerOrWaitFired };

    ManagedThreadBase::ThreadPool(((DelegateInfo*) delegateInfo)->m_appDomainId, RegisterWaitForSingleObjectCallback_Worker, &args);

    // We should have released all locks.
    _ASSERTE(g_fEEShutDown || pThread->m_dwLockCount == 0 || pThread->m_fRudeAborted);
    return;
}

void ThreadPoolNative::Init()
{

}


VOID
AcquireDelegateInfo(DelegateInfo *pDelInfo)
{
    LEAF_CONTRACT;
}

VOID
ReleaseDelegateInfo(DelegateInfo *pDelInfo)
{
    CONTRACTL 
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;
    
    // The release methods of holders can be called with preemptive GC enabled. Ensure we're in cooperative mode
    // before calling pDelInfo->Release(), since that requires coop mode.
    GCX_COOP();

    pDelInfo->Release();
    ThreadpoolMgr::RecycleMemory( pDelInfo, ThreadpoolMgr::MEMTYPE_DelegateInfo );
}

typedef Holder<DelegateInfo *, AcquireDelegateInfo, ReleaseDelegateInfo> DelegateInfoHolder;

FCIMPL7(LPVOID, ThreadPoolNative::CorRegisterWaitForSingleObject,
                                        Object* waitObjectUNSAFE,
                                        Object* stateUNSAFE,
                                        UINT32 timeout,
                                        CLR_BOOL executeOnlyOnce,
                                        Object* registeredWaitObjectUNSAFE,
                                        StackCrawlMark* stackMark,
                                        CLR_BOOL compressStack)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;
    
    HANDLE handle = 0;
    struct _gc
    {
        WAITHANDLEREF waitObject;
        OBJECTREF state;
        OBJECTREF registeredWaitObject;
    } gc;
    gc.waitObject = (WAITHANDLEREF) ObjectToOBJECTREF(waitObjectUNSAFE);
    gc.state = (OBJECTREF) stateUNSAFE;
    gc.registeredWaitObject = (OBJECTREF) registeredWaitObjectUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc);  // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    if(gc.waitObject == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Obj");

    _ASSERTE(gc.registeredWaitObject != NULL);

    ULONG flag = executeOnlyOnce ? WAIT_SINGLE_EXECUTION | WAIT_FREE_CONTEXT : WAIT_FREE_CONTEXT;

    HANDLE hWaitHandle = gc.waitObject->GetWaitHandle();
    _ASSERTE(hWaitHandle);

    Thread* pCurThread = GetThread();
    _ASSERTE( pCurThread);

    AppDomain* appDomain = pCurThread->GetDomain();
    _ASSERTE(appDomain);

    DelegateInfoHolder delInfoHolder;
    DelegateInfo* delegateInfo = DelegateInfo::MakeDelegateInfo(appDomain,
                                                                &gc.state,
                                                                (OBJECTREF *)&gc.waitObject,
                                                                &gc.registeredWaitObject);
    delInfoHolder.Assign(delegateInfo);

    if (Security::IsSecurityOn() && compressStack)
    {
        delegateInfo->SetThreadSecurityInfo( pCurThread, stackMark );
    }



    if (!(ThreadpoolMgr::RegisterWaitForSingleObject(&handle,
                                          hWaitHandle,
                                          RegisterWaitForSingleObjectCallback,
                                          (PVOID) delegateInfo,
                                          (ULONG) timeout,
                                          flag)))

    {
        _ASSERTE(GetLastError() != ERROR_CALL_NOT_IMPLEMENTED);

        COMPlusThrowWin32();
    }

    delInfoHolder.SuppressRelease();
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return (LPVOID) handle;
}
FCIMPLEND


/********************************************************************************************************************/

static VOID
QueueUserWorkItemCallback_Worker(PVOID pArg)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // This thread should not have any locks held at entry point.
    _ASSERTE(GetThread()->m_dwLockCount == 0);

    OBJECTREF orState = NULL;

    GCPROTECT_BEGIN( orState );

    DelegateInfoHolder* pDelInfoHolder = (DelegateInfoHolder*)pArg;

    orState = ObjectFromHandle( pDelInfoHolder->GetValue()->m_stateHandle );

    pDelInfoHolder->Release();

#ifdef _DEBUG
    MethodDesc *pMeth = g_Mscorlib.GetMethod(METHOD__TPWAIT_HELPER__PERFORM_WAIT_CALLBACK);
    LogCall(pMeth,"QUWICallback");
#endif

    // Caution: the args are not protected, we have to garantee there's no GC from here till
    // the managed call happens.
    PREPARE_NONVIRTUAL_CALLSITE(METHOD__TPWAIT_HELPER__PERFORM_WAIT_CALLBACK);
    DECLARE_ARGHOLDER_ARRAY(arg, 1);
    arg[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(orState);

    // Call the method...
    CALL_MANAGED_METHOD_NORET(arg);

    GCPROTECT_END();
}

DWORD WINAPI QueueUserWorkItemCallback(PVOID pArg)
{
    HRESULT hr;
    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread, &hr);
        if (pThread == NULL) {
            return hr;
        }
    }

    CONTRACTL
    {
        MODE_PREEMPTIVE;
        THROWS; 
        GC_TRIGGERS;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pArg));
    }
    CONTRACTL_END;

    GCX_COOP();
    BEGIN_SO_INTOLERANT_CODE(pThread);

    DelegateInfo*      delegateInfo = (DelegateInfo*)pArg;
    DelegateInfoHolder delInfoHolder( delegateInfo );
    
    //
    // NOTE: there is a potential race between the time we retrieve the app domain pointer,
    // and the time which this thread enters the domain.
    //
    // To solve the race, we rely on the fact that there is a thread sync (via GC)
    // between releasing an app domain's handle, and destroying the app domain.  Thus
    // it is important that we not go into preemptive gc mode in that window.
    //
    {
        // this holder resets our thread's security state when exiting this scope, 
        ThreadSecurityStateHolder  secState(pThread);
            
        ManagedThreadBase::ThreadPool( delegateInfo->m_appDomainId, QueueUserWorkItemCallback_Worker, &delInfoHolder );
    }

    // We should have released all locks.
    _ASSERTE(g_fEEShutDown || pThread->m_dwLockCount == 0 || pThread->m_fRudeAborted);
    
    END_SO_INTOLERANT_CODE;
    return ERROR_SUCCESS;
}


FCIMPL3(FC_BOOL_RET, ThreadPoolNative::CorQueueUserWorkItem, Object* stateUNSAFE, StackCrawlMark* stackMark, CLR_BOOL compressStack)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    OBJECTREF state = (OBJECTREF) stateUNSAFE;
    BOOL res = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(state); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    Thread* pCurThread = GetThread();
    _ASSERTE( pCurThread);

    AppDomain* appDomain = pCurThread->GetDomain();
    _ASSERTE(appDomain);

    DelegateInfo* delegateInfo = DelegateInfo::MakeDelegateInfo(appDomain,
                                                                &state,
                                                                NULL,
                                                                NULL);
    DelegateInfoHolder delInfoHolder( delegateInfo );

    if (Security::IsSecurityOn() && compressStack)
    {
        delegateInfo->SetThreadSecurityInfo( pCurThread, stackMark );
    }

    res = ThreadpoolMgr::QueueUserWorkItem(QueueUserWorkItemCallback,
                                      (PVOID) delegateInfo,
                                                0);
    if (!res)
    {
        if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            COMPlusThrow(kNotSupportedException);
        else
            COMPlusThrowWin32();
    }
    else
        delInfoHolder.SuppressRelease();

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(res);
}
FCIMPLEND

/********************************************************************************************************************/

FCIMPL2(FC_BOOL_RET, ThreadPoolNative::CorUnregisterWait, LPVOID WaitHandle, Object* objectToNotify)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = false;
    SAFEHANDLEREF refSH = (SAFEHANDLEREF) ObjectToOBJECTREF(objectToNotify);
    HELPER_METHOD_FRAME_BEGIN_RET_1(refSH); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    HANDLE hWait = (HANDLE) WaitHandle;
    HANDLE hObjectToNotify = NULL;

    ThreadpoolMgr::WaitInfo *pWaitInfo = (ThreadpoolMgr::WaitInfo *)hWait;
    _ASSERTE(pWaitInfo != NULL);

    ThreadpoolMgr::WaitInfoHolder   wiHolder(NULL);

    if (refSH != NULL)
    {
        // Create a GCHandle in the WaitInfo, so that it can hold on to the safe handle
        pWaitInfo->ExternalEventSafeHandle = GetAppDomain()->CreateHandle(NULL);
        pWaitInfo->handleOwningAD = GetAppDomain()->GetId();

        // Holder will now release objecthandle in face of exceptions
        wiHolder.Assign(pWaitInfo);
        
        // Once CreateHandle succeeds, AddRef the safehandle. It'll be 
        // released when the unregister is complete or in the case of any error
        refSH->AddRef();

        // Store SafeHandle in object handle. Holder will now release both safehandle and objecthandle
        // in case of exceptions
        StoreObjectInHandle(pWaitInfo->ExternalEventSafeHandle, refSH);

        if (refSH->OwnsHandle())
            pWaitInfo->bReleaseEventIfADUnloaded = TRUE;

        hObjectToNotify = (HANDLE) refSH->GetHandle();

        if (hObjectToNotify == (HANDLE) -1)
        {
            // We do not need the ObjectHandle, refcount on the safehandle etc
            wiHolder.Release();
            _ASSERTE(pWaitInfo->ExternalEventSafeHandle == NULL);
        }
    }
    
    retVal = ThreadpoolMgr::UnregisterWaitEx(hWait, hObjectToNotify);

    if (retVal)
        wiHolder.SuppressRelease();
    
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

/********************************************************************************************************************/
FCIMPL1(void, ThreadPoolNative::CorWaitHandleCleanupNative, LPVOID WaitHandle)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0(); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    HANDLE hWait = (HANDLE)WaitHandle;
    ThreadpoolMgr::WaitHandleCleanup(hWait);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

/********************************************************************************************************************/

/********************************************************************************************************************/

struct BindIoCompletion_Args
{
    DWORD ErrorCode;
    DWORD numBytesTransferred;
    LPOVERLAPPED lpOverlapped;
    BOOL *pfProcessed;
};

VOID BindIoCompletionCallBack_Worker(LPVOID args)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_SO_INTOLERANT;
    
    DWORD        ErrorCode = ((BindIoCompletion_Args *)args)->ErrorCode;
    DWORD        numBytesTransferred = ((BindIoCompletion_Args *)args)->numBytesTransferred;
    LPOVERLAPPED lpOverlapped = ((BindIoCompletion_Args *)args)->lpOverlapped;
    bool         exceptionOccurred = true;
    
    OVERLAPPEDDATAREF overlapped = ObjectToOVERLAPPEDDATAREF(OverlappedDataObject::GetOverlapped(lpOverlapped));

    GCPROTECT_BEGIN(overlapped);
    *(((BindIoCompletion_Args *)args)->pfProcessed) = TRUE;
    // we set processed to TRUE, now it's our responsibility to guarantee proper cleanup

    EX_TRY_FOR_FINALLY
    {

#ifdef _DEBUG
        MethodDesc *pMeth = g_Mscorlib.GetMethod(METHOD__IOCB_HELPER__PERFORM_IOCOMPLETION_CALLBACK);
        LogCall(pMeth,"IOCallback");
#endif

    if (overlapped->m_iocb != NULL)
    {
        // Caution: the args are not protected, we have to garantee there's no GC from here till
        // the managed call happens.
        PREPARE_NONVIRTUAL_CALLSITE(METHOD__IOCB_HELPER__PERFORM_IOCOMPLETION_CALLBACK);        
        DECLARE_ARGHOLDER_ARRAY(arg, 3);
        arg[ARGNUM_0]  = DWORD_TO_ARGHOLDER(ErrorCode);    
        arg[ARGNUM_1]  = DWORD_TO_ARGHOLDER(numBytesTransferred);       
        arg[ARGNUM_2]  = PTR_TO_ARGHOLDER(lpOverlapped);

        // Call the method...
        CALL_MANAGED_METHOD_NORET(arg);
    }
    else
    { // no user delegate to callback
        _ASSERTE((overlapped->m_iocbHelper == NULL) || !"This is benign, but should be optimized");

    }
    exceptionOccurred = false;
    }
    EX_FINALLY
    {
        if (exceptionOccurred)
        {
           OBJECTHANDLE pinSelf = overlapped->m_pinSelf;   
           if (pinSelf && !GetAppDomain()->CanThreadEnter(GetThread()))
                overlapped->FreeAsyncPinHandles();
        }
    }
    EX_END_FINALLY;
    GCPROTECT_END();
}


void __stdcall BindIoCompletionCallbackStubEx(DWORD ErrorCode,
                                              DWORD numBytesTransferred,
                                              LPOVERLAPPED lpOverlapped,
                                              BOOL setStack)
{
    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread);
        if (pThread == NULL) {
            return;
        }
    }

    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    // This thread should not have any locks held at entry point.
    _ASSERTE(pThread->m_dwLockCount == 0);

    LOG((LF_INTEROP, LL_INFO10000, "In IO_CallBackStub thread 0x%x retCode 0x%x, overlap 0x%x\n",  pThread, ErrorCode, lpOverlapped));

    GCX_COOP();

    // NOTE: there is a potential race between the time we retrieve the app domain pointer,
    // and the time which this thread enters the domain.
    //
    // To solve the race, we rely on the fact that there is a thread sync (via GC)
    // between releasing an app domain's handle, and destroying the app domain.  Thus
    // it is important that we not go into preemptive gc mode in that window.
    //

    //IMPORTANT - do not gc protect overlapped here - it belongs to another appdomain
    //so if it stops being pinned it should be able to go away
    OVERLAPPEDDATAREF overlapped = ObjectToOVERLAPPEDDATAREF(OverlappedDataObject::GetOverlapped(lpOverlapped));
    AppDomainFromIDHolder appDomain(ADID(overlapped->GetAppDomainId()), TRUE);
    BOOL fProcessed = FALSE;
    if (!appDomain.IsUnloaded())
    {
            // this holder resets our thread's security state when exiting this scope, 
            // but only if setStack is TRUE.
            Thread* pHolderThread = NULL;
            if (setStack)
            {
                pHolderThread = pThread; 
            }
            
            ThreadSecurityStateHolder  secState(pHolderThread);
            
        BindIoCompletion_Args args = {ErrorCode, numBytesTransferred, lpOverlapped, &fProcessed};
        appDomain.Release();
        ManagedThreadBase::ThreadPool(ADID(overlapped->GetAppDomainId()), BindIoCompletionCallBack_Worker, &args);
    }

 


    LOG((LF_INTEROP, LL_INFO10000, "Leaving IO_CallBackStub thread 0x%x retCode 0x%x, overlap 0x%x\n",  pThread, ErrorCode, lpOverlapped));
        // We should have released all locks.
    _ASSERTE(g_fEEShutDown || pThread->m_dwLockCount == 0 || pThread->m_fRudeAborted);
    return;
}

void WINAPI BindIoCompletionCallbackStub(DWORD ErrorCode,
                                            DWORD numBytesTransferred,
                                            LPOVERLAPPED lpOverlapped)
{
    WRAPPER_CONTRACT;
    BindIoCompletionCallbackStubEx(ErrorCode, numBytesTransferred, lpOverlapped, TRUE);
}

FCIMPL1(FC_BOOL_RET, ThreadPoolNative::CorBindIoCompletionCallback, HANDLE fileHandle)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_0(); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    HANDLE hFile = (HANDLE) fileHandle;
    DWORD errCode = 0;

    retVal = ThreadpoolMgr::BindIoCompletionCallback(hFile,
                                           BindIoCompletionCallbackStub,
                                           0,     // reserved, must be 0
                                           OUT errCode);
    if (!retVal)
    {
        if (errCode == ERROR_CALL_NOT_IMPLEMENTED)
            COMPlusThrow(kPlatformNotSupportedException);
        else
        {
            SetLastError(errCode);
            COMPlusThrowWin32();
        }
    }

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, ThreadPoolNative::CorPostQueuedCompletionStatus, LPOVERLAPPED lpOverlapped)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    OVERLAPPEDDATAREF   overlapped = ObjectToOVERLAPPEDDATAREF(OverlappedDataObject::GetOverlapped(lpOverlapped));

    BOOL res = FALSE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(overlapped); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    // OS doesn't signal handle, so do it here
    overlapped->Internal = 0;

    res = ThreadpoolMgr::PostQueuedCompletionStatus(lpOverlapped,
                                   BindIoCompletionCallbackStub);

    if (!res)
    {
        if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            COMPlusThrow(kPlatformNotSupportedException);
        else
            COMPlusThrowWin32();
    }

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(res);
}
FCIMPLEND


/********************************************************************************************************************/


/******************************************************************************************/
/*                                                                                        */
/*                              Timer Functions                                           */
/*                                                                                        */
/******************************************************************************************/

struct AddTimerCallback_Args
{
    PVOID delegateInfo;
    BOOL TimerOrWaitFired;
};

void AddTimerCallback_Worker(LPVOID ptr)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    OBJECTREF orState = NULL;

    GCPROTECT_BEGIN( orState );

    AddTimerCallback_Args *args = (AddTimerCallback_Args *) ptr;
    orState = ObjectFromHandle(((DelegateInfo*) args->delegateInfo)->m_stateHandle);            

#ifdef _DEBUG
    MethodDesc *pMeth = g_Mscorlib.GetMethod(METHOD__TPTIMER_HELPER__PERFORM_TIMER_CALLBACK);
    LogCall(pMeth,"TimerCallback");
#endif

    // Caution: the args are not protected, we have to garantee there's no GC from here till
    // the managed call happens.
    PREPARE_NONVIRTUAL_CALLSITE(METHOD__TPTIMER_HELPER__PERFORM_TIMER_CALLBACK);
    DECLARE_ARGHOLDER_ARRAY(arg, 1);
    arg[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(orState);

    // Call the method...
    CALL_MANAGED_METHOD_NORET(arg);

    GCPROTECT_END();
}

VOID WINAPI AddTimerCallbackEx(PVOID delegateInfo, BOOL TimerOrWaitFired, BOOL setStack)
{
    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread);
        if (pThread == NULL) {
            return;
        }
    }

    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        SO_INTOLERANT;
        
        PRECONDITION(CheckPointer(delegateInfo));
    }
    CONTRACTL_END;

    // This thread should not have any locks held at entry point.
    _ASSERTE(pThread->m_dwLockCount == 0);

    GCX_COOP();

    // NOTE: there is a potential race between the time we retrieve the app domain pointer,
    // and the time which this thread enters the domain.
    //
    // To solve the race, we rely on the fact that there is a thread sync (via GC)
    // between releasing an app domain's handle, and destroying the app domain.  Thus
    // it is important that we not go into preemptive gc mode in that window.
    //
    {
            // this holder resets our thread's security state when exiting this scope, 
            // but only if setStack is TRUE.
            Thread* pHolderThread = NULL;
            if (setStack)
            {
                pHolderThread = pThread; 
            }
            
            ThreadSecurityStateHolder  secState(pHolderThread);

        AddTimerCallback_Args args = { delegateInfo, TimerOrWaitFired };
    
        ManagedThreadBase::ThreadPool(((DelegateInfo*) delegateInfo)->m_appDomainId, AddTimerCallback_Worker, &args);
    }
    
    // We should have released all locks.
    _ASSERTE(g_fEEShutDown || pThread->m_dwLockCount == 0 || pThread->m_fRudeAborted);
}

VOID WINAPI AddTimerCallback(PVOID delegateInfo, BOOL TimerOrWaitFired)
{
    WRAPPER_CONTRACT;

    AddTimerCallbackEx(delegateInfo, TimerOrWaitFired, TRUE);
}

FCIMPL5(VOID, TimerNative::CorCreateTimer, TimerBaseNative* pThisUNSAFE, Object* stateUNSAFE, INT32 dueTime, INT32 period, StackCrawlMark* stackMark)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    struct _gc
    {
        TIMERREF pThis;
        OBJECTREF state;
    } gc;
    gc.pThis = (TIMERREF) pThisUNSAFE;
    gc.state = (OBJECTREF) stateUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    Thread* pCurThread = GetThread();
    _ASSERTE( pCurThread);

    AppDomain* appDomain = pCurThread->GetDomain();
    _ASSERTE(appDomain);

    DelegateInfoHolder delInfoHolder;
    DelegateInfo* delegateInfo = DelegateInfo::MakeDelegateInfo(appDomain,
                                                                &gc.state,
                                                                NULL,
                                                                NULL);
    delInfoHolder.Assign(delegateInfo);

    if (Security::IsSecurityOn())
    {
        delegateInfo->SetThreadSecurityInfo( pCurThread, stackMark );
    }

    HANDLE hNewTimer;
    BOOL res = ThreadpoolMgr::CreateTimerQueueTimer(&hNewTimer,
                                     (WAITORTIMERCALLBACK) AddTimerCallback ,
                                     (PVOID) delegateInfo,
                                     (ULONG) dueTime,
                                     (ULONG) period,
                                     0
                                     );
    if (!res)
    {
        if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            COMPlusThrow(kNotSupportedException);
        else
            COMPlusThrowWin32();
    }
    else
        delInfoHolder.SuppressRelease();

    gc.pThis->SetDelegateInfo(delegateInfo);
    gc.pThis->SetTimerHandle(hNewTimer);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

/******************************************************************************************/

FCIMPL2(FC_BOOL_RET, TimerNative::CorDeleteTimer, TimerBaseNative* pThisUNSAFE, Object* notifyObject)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = FALSE;
    TIMERREF pThis = (TIMERREF) pThisUNSAFE;
    SAFEHANDLEREF   refNotifyObject = (SAFEHANDLEREF) ObjectToOBJECTREF(notifyObject);
    HELPER_METHOD_FRAME_BEGIN_RET_2(pThis, refNotifyObject); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    LONG deleted = 0;
    BOOL res1;
    DWORD errorCode = 0;
    Thread *pThread = NULL;
    HANDLE notifyHandle = NULL;

    ThreadpoolMgr::TimerInfoHolder tiHolder(NULL);
    
    HANDLE timerHandle = pThis->GetTimerHandle();
    ThreadpoolMgr::TimerInfo *timerInfo = (ThreadpoolMgr::TimerInfo *)timerHandle;
    if (timerInfo == NULL)        // this can happen if an exception is thrown in the timer constructor
    {                               // and the finalizer thread calls this through dispose
        goto lExit;
    }

    deleted = InterlockedExchange(pThis->GetAddressTimerDeleted(),TRUE);
    if (deleted)   // someone beat us to it
    {
        goto lExit;   // an application error, so return false.
    }

    if (refNotifyObject != NULL)
    {
        // Create a GCHandle in the WaitInfo, so that it can hold on to the safe handle
        timerInfo->ExternalEventSafeHandle = GetAppDomain()->CreateHandle(NULL);
        timerInfo->handleOwningAD = GetAppDomain()->GetId();

        // Holder will now release objecthandle in face of exceptions
        tiHolder.Assign(timerInfo);
        
        // Once CreateHandle succeeds, AddRef the safehandle. It'll be 
        // released when the unregister is complete or in the case of any error
        refNotifyObject->AddRef();

        // Store SafeHandle in object handle. Holder will now release both safehandle and objecthandle
        // in case of exceptions
        StoreObjectInHandle(timerInfo->ExternalEventSafeHandle, refNotifyObject);

        notifyHandle = (HANDLE) refNotifyObject->GetHandle();

        if (refNotifyObject->OwnsHandle())
            timerInfo->bReleaseEventIfADUnloaded = TRUE;

        if (notifyHandle == (HANDLE) -1)
        {
            // We do not need the ObjectHandle, refcount on the safehandle etc
            tiHolder.Release();
            _ASSERTE(timerInfo->ExternalEventSafeHandle == NULL);
        }
    }
    
    pThread = GetThread();

    {
        GCX_PREEMP_THREAD_EXISTS(pThread);

        res1 = ThreadpoolMgr::DeleteTimerQueueTimer(timerHandle, notifyHandle);

        if (res1)
            tiHolder.SuppressRelease();
        else
        {
            errorCode = ::GetLastError();   // capture the error code so we can throw the right exception
            if (errorCode != ERROR_IO_PENDING)
            {
                COMPlusThrowWin32(HRESULT_FROM_WIN32(errorCode));
            }
        }
    }

    retVal = TRUE;

lExit: ;
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND



/******************************************************************************************/

FCIMPL3(FC_BOOL_RET, TimerNative::CorChangeTimer, TimerBaseNative* pThisUNSAFE, INT32 dueTime, INT32 period)
{
    CONTRACTL 
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = FALSE;
    TIMERREF pThis = (TIMERREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(pThis); // Eventually calls BEGIN_SO_INTOLERANT_CODE_NOTHROW
    //-[autocvtpro]-------------------------------------------------------

    BOOL status = FALSE;

    if (pThis->IsTimerDeleted())
    {
        goto lExit;
    }
    status = ThreadpoolMgr::ChangeTimerQueueTimer(
                                            pThis->GetTimerHandle(),
                                            (ULONG)dueTime,
                                            (ULONG)period);

    if (!status)
    {
        COMPlusThrowWin32();
    }

    retVal = TRUE;

lExit: ;
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND





