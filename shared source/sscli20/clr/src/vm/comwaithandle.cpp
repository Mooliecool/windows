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
** COMWaitHandle.cpp
**
**                                                  
**
** Purpose: Native methods on System.WaitHandle
**
** Date:  August, 1999
**
===========================================================*/
#include "common.h"
#include "object.h"
#include "field.h"
#include "excep.h"
#include "comwaithandle.h"


//-----------------------------------------------------------------------------
// ObjArrayHolder : ideal for holding a managed array of items.  Will run 
// the ACQUIRE method sequentially on each item.  Assume the ACQUIRE method 
// may possibly fail.  If it does, only release the ones we've acquired.
// Note: If a GC occurs during the ACQUIRE or RELEASE methods, you'll have to
// explicitly gc protect the objectref.
//-----------------------------------------------------------------------------
template <typename TYPE, void (*ACQUIRE)(TYPE), void (*RELEASEF)(TYPE)>
class ObjArrayHolder
{

public:
    ObjArrayHolder() {
        LEAF_CONTRACT;
        m_numAcquired = 0;
        m_pValues = NULL;
    }

    // Assuming ACQUIRE can throw an exception, we must put this logic 
    // somewhere outside of the constructor.  In C++, the destructor won't be
    // run if the constructor didn't complete.
    void Initialize(const unsigned int numElements, PTRARRAYREF* pValues) {
        WRAPPER_CONTRACT;
        _ASSERTE(m_numAcquired == 0);
        m_numElements = numElements;
        m_pValues = pValues;
        for (unsigned int i=0; i<m_numElements; i++) {
            TYPE value = (TYPE) (*m_pValues)->GetAt(i);
            ACQUIRE(value);
            m_numAcquired++;
        }
    }
        
    ~ObjArrayHolder() {
        WRAPPER_CONTRACT;
        GCX_COOP();
        for (unsigned int i=0; i<m_numAcquired; i++) {
            TYPE value = (TYPE) (*m_pValues)->GetAt(i);
            RELEASEF(value);
        }
    }

private:
    unsigned int m_numElements;
    unsigned int m_numAcquired;
    PTRARRAYREF* m_pValues;

    FORCEINLINE ObjArrayHolder<TYPE, ACQUIRE, RELEASEF> &operator=(const ObjArrayHolder<TYPE, ACQUIRE, RELEASEF> &holder)
    {
        _ASSERTE(!"No assignment allowed");
        return NULL;
    }

    FORCEINLINE ObjArrayHolder(const ObjArrayHolder<TYPE, ACQUIRE, RELEASEF> &holder)
    {
        _ASSERTE(!"No copy construction allowed");
    }
};

void AcquireSafeHandleFromWaitHandle(WAITHANDLEREF wh)
{
    WRAPPER_CONTRACT;
    
    GCX_COOP();
    _ASSERTE(wh != NULL);
    SAFEHANDLEREF sh = wh->GetSafeHandle();
    sh->AddRef();
}

void ReleaseSafeHandleFromWaitHandle(WAITHANDLEREF wh)
{
    WRAPPER_CONTRACT;
    
    GCX_COOP();
    _ASSERTE(wh != NULL);
    SAFEHANDLEREF sh = wh->GetSafeHandle();
    sh->Release();
}

typedef ObjArrayHolder<WAITHANDLEREF, AcquireSafeHandleFromWaitHandle, ReleaseSafeHandleFromWaitHandle> WaitHandleArrayHolder;


FCIMPL4(INT32, WaitHandleNative::CorWaitOneNative, SafeHandle* safeWaitHandleUNSAFE, INT32 timeout, CLR_BOOL hasThreadAffinity, CLR_BOOL exitContext)
{
    CONTRACTL {
        WRAPPER(GC_TRIGGERS);
        WRAPPER(THROWS);
        SO_TOLERANT;
    } CONTRACTL_END;

    INT32 retVal = 0;
    SAFEHANDLEREF sh(safeWaitHandleUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_RET_1(sh);
    //-[autocvtpro]-------------------------------------------------------

    _ASSERTE(sh != NULL);

    Thread* pThread = GetThread();

    DWORD res = (DWORD) -1;

    Context* targetContext = pThread->GetContext();
    _ASSERTE(targetContext);
    Context* defaultContext = pThread->GetDomain()->GetDefaultContext();
    _ASSERTE(defaultContext);
    // DoAppropriateWait calls LeaveRuntime/EnterRuntime which may cause the current
    // fiber to be re-scheduled.
    ThreadAffinityAndCriticalRegionHolder affinityAndCriticalRegionHolder(hasThreadAffinity);
    SafeHandleHolder shh(&sh);
    // Note that SafeHandle is a GC object, and RequestCallback and 
    // DoAppropriateWait work on an array of handles.  Don't pass the address
    // of the handle field - that's a GC hole.  Instead, pass this temp
    // array.
    HANDLE handles[1];
    handles[0] = sh->GetHandle();

    if (exitContext != NULL &&
        targetContext != defaultContext)
    {
        Context::WaitArgs waitOneArgs = {1, handles, TRUE, timeout, TRUE, &res};
        Context::CallBackInfo callBackInfo = {Context::Wait_callback, (void*) &waitOneArgs};
        Context::RequestCallBack(CURRENT_APPDOMAIN_ID,defaultContext, &callBackInfo);
    }
    else
    {
        res = pThread->DoAppropriateWait(1,handles,TRUE,timeout, WaitMode_Alertable /*alertable*/);
    }

    retVal = res;

    if (res == WAIT_OBJECT_0 && hasThreadAffinity) {
        affinityAndCriticalRegionHolder.SuppressRelease();
    }
    else if(res == WAIT_ABANDONED_0) {
        // WAIT_ABANDONED means the specified object is a mutex object that was not released by the thread 
        // that owned the mutex object before the owning thread terminated. 
        // Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled.    
        _ASSERTE(hasThreadAffinity);
        affinityAndCriticalRegionHolder.SuppressRelease();    
    }

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

FCIMPL4(INT32, WaitHandleNative::CorWaitMultipleNative, Object* waitObjectsUNSAFE, INT32 timeout, CLR_BOOL exitContext, CLR_BOOL waitForAll)
{
    INT32 retVal = 0;
    OBJECTREF waitObjects = (OBJECTREF) waitObjectsUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(waitObjects);
    //-[autocvtpro]-------------------------------------------------------
    
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        SO_TOLERANT;
    } CONTRACTL_END;

    _ASSERTE(waitObjects);

    Thread* pThread = GetThread();

    PTRARRAYREF pWaitObjects = (PTRARRAYREF)waitObjects;  // array of objects on which to wait
    int numWaiters = pWaitObjects->GetNumComponents();


    WaitHandleArrayHolder arrayHolder;
    arrayHolder.Initialize(numWaiters, (PTRARRAYREF*) &waitObjects);
    
    pWaitObjects = (PTRARRAYREF)waitObjects;  // array of objects on which to wait
    HANDLE* internalHandles = (HANDLE*) _alloca(numWaiters*sizeof(HANDLE));
    BOOL *hasThreadAffinity = (BOOL*) _alloca(numWaiters*sizeof(BOOL));

    BOOL mayRequireThreadAffinity = FALSE;
    for (int i=0;i<numWaiters;i++)
    {
        WAITHANDLEREF waitObject = (WAITHANDLEREF) pWaitObjects->m_Array[i];
        if (waitObject == NULL)
            COMPlusThrow(kNullReferenceException);

        MethodTable *pMT = waitObject->GetMethodTable();
        if (pMT->IsTransparentProxyType())
            COMPlusThrow(kInvalidOperationException,L"InvalidOperation_WaitOnTransparentProxy");
        internalHandles[i] = waitObject->m_handle;

        // m_hasThreadAffinity is set for Mutex only 
        hasThreadAffinity[i] = waitObject->m_hasThreadAffinity;
        if (hasThreadAffinity[i]) {
            mayRequireThreadAffinity = TRUE;
        }
    }

    DWORD res = (DWORD) -1;
    ThreadAffinityHolder affinityHolder(mayRequireThreadAffinity);
    Context* targetContext = pThread->GetContext();
    _ASSERTE(targetContext);
    Context* defaultContext = pThread->GetDomain()->GetDefaultContext();
    _ASSERTE(defaultContext);
    if (exitContext != NULL &&
        targetContext != defaultContext)
    {
        Context::WaitArgs waitMultipleArgs = {numWaiters, internalHandles, waitForAll, timeout, TRUE, &res};
        Context::CallBackInfo callBackInfo = {Context::Wait_callback, (void*) &waitMultipleArgs};
        Context::RequestCallBack(CURRENT_APPDOMAIN_ID,defaultContext, &callBackInfo);
    }
    else
    {
        res = pThread->DoAppropriateWait(numWaiters, internalHandles, waitForAll, timeout, WaitMode_Alertable /*alertable*/);
    }

    if (mayRequireThreadAffinity) {
        if (waitForAll) {
            if (res >= (DWORD) WAIT_OBJECT_0 && res < (DWORD) WAIT_OBJECT_0 + numWaiters) {
                for (int i = 0; i < numWaiters; i ++) {
                    if (hasThreadAffinity[i]) {
                        Thread::BeginThreadAffinityAndCriticalRegion();
                    }
                }
            }
            // If some mutex is abandoned
            else if (res >= (DWORD) WAIT_ABANDONED_0 && res < (DWORD) WAIT_ABANDONED_0+numWaiters) {
                for (int i = 0; i < numWaiters; i ++) {
                    if (hasThreadAffinity[i])
                    {
                        if (WaitForSingleObject(internalHandles[i],0) == WAIT_OBJECT_0)
                        {
                            BOOL result;
                            result = ReleaseMutex(internalHandles[i]);
                            _ASSERTE (result);
                            Thread::BeginThreadAffinityAndCriticalRegion();
                        }
                    }
                }
            }
        }
        else {
            if ( res >= (DWORD)WAIT_OBJECT_0 && res < (DWORD)WAIT_OBJECT_0 + numWaiters) {
                if (hasThreadAffinity[res - WAIT_OBJECT_0]) {
                    Thread::BeginThreadAffinityAndCriticalRegion();
                }
            }
            else if (res >= (DWORD)WAIT_ABANDONED_0 && res < (DWORD)WAIT_ABANDONED_0 + numWaiters) {
                _ASSERTE (hasThreadAffinity[res - WAIT_ABANDONED_0]);
                Thread::BeginThreadAffinityAndCriticalRegion();                
            }
        }
    }
    retVal = res;

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND



