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
** Header: COMNativeOverlapped.h
**
**                                                  
**
** Purpose: Native methods for allocating and freeing NativeOverlapped
**
** Date:  January, 2000
** 
===========================================================*/
#include "common.h"
#include "fcall.h"
#include "nativeoverlapped.h"
#include "corhost.h"
#include "win32threadpool.h"
#include "mdaassistants.h"

LONG OverlappedDataObject::s_CleanupRequestCount = 0;
BOOL OverlappedDataObject::s_CleanupInProgress = FALSE;
BOOL OverlappedDataObject::s_GCDetectsCleanup = FALSE;
BOOL OverlappedDataObject::s_CleanupFreeHandle = FALSE;

FCIMPL1(void*, AllocateNativeOverlapped, OverlappedDataObject* overlappedUNSAFE)
{
    CONTRACTL 
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } 
    CONTRACTL_END;

    OVERLAPPEDDATAREF   overlapped   = ObjectToOVERLAPPEDDATAREF(overlappedUNSAFE);
    OBJECTREF       userObject = overlapped->m_userObject;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_NONE, overlapped, userObject);

    AsyncPinningHandleHolder handle;

    if (g_pOverlappedDataClass == NULL)
    {
        g_pOverlappedDataClass = g_Mscorlib.FetchClass(CLASS__OVERLAPPEDDATA);
        // We have optimization to avoid creating event if IO is in default domain.  This depends on default domain 
        // can not be unloaded.
        _ASSERTE(!SystemDomain::System()->DefaultDomain()->CanUnload());
        _ASSERTE(SystemDomain::System()->DefaultDomain()->GetId().m_dwId == DefaultADID);
    }

    CONSISTENCY_CHECK(overlappedUNSAFE->GetMethodTable() == g_pOverlappedDataClass);

    overlapped->m_AppDomainId = GetAppDomain()->GetId().m_dwId;

    if (userObject != NULL)
    {
        if (overlapped->m_isArray == 1)
        {
            BASEARRAYREF asArray = (BASEARRAYREF) userObject;
            OBJECTREF *pObj = (OBJECTREF*)(asArray->GetDataPtr());
            DWORD num = asArray->GetNumComponents();
            DWORD i;
            for (i = 0; i < num; i ++)
            {
                GCHandleValidatePinnedObject(pObj[i]);
            }
            for (i = 0; i < num; i ++)
            {
                asArray = (BASEARRAYREF) userObject;
                AddMTForPinHandle(pObj[i]);
            }
        }
        else
        {
            GCHandleValidatePinnedObject(userObject);
            AddMTForPinHandle(userObject);
        }
        
    }

    handle = GetAppDomain()->CreateTypedHandle(overlapped, HNDTYPE_ASYNCPINNED);
    if (CLRIoCompletionHosted()) 
    {
        _ASSERTE(CorHost2::GetHostIoCompletionManager());
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = CorHost2::GetHostIoCompletionManager()->InitializeHostOverlapped(&overlapped->Internal);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr)) 
        {
            COMPlusThrowHR(hr);
        }
    }

    handle.SuppressRelease();
    overlapped->m_pinSelf = handle;

    HELPER_METHOD_FRAME_END();
    LOG((LF_INTEROP, LL_INFO10000, "In AllocNativeOperlapped thread 0x%x\n", GetThread()));

    return &overlapped->Internal;
}
FCIMPLEND

FCIMPL1(void, FreeNativeOverlapped, LPOVERLAPPED lpOverlapped)
{
    CONTRACTL 
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        ENTRY_POINT;
    } 
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    OVERLAPPEDDATAREF overlapped = ObjectToOVERLAPPEDDATAREF(OverlappedDataObject::GetOverlapped(lpOverlapped));
    CONSISTENCY_CHECK(g_pOverlappedDataClass && (overlapped->GetMethodTable() == g_pOverlappedDataClass));



    overlapped->FreeAsyncPinHandles();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(OverlappedDataObject*, GetOverlappedFromNative, LPOVERLAPPED lpOverlapped)
{
    CONTRACTL 
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } 
    CONTRACTL_END;

    CONSISTENCY_CHECK(g_pOverlappedDataClass && (OverlappedDataObject::GetOverlapped(lpOverlapped)->GetMethodTable() == g_pOverlappedDataClass));

    return OverlappedDataObject::GetOverlapped(lpOverlapped);
}
FCIMPLEND

void OverlappedDataObject::FreeAsyncPinHandles()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // This cannot throw or return error, and cannot force SO because it is called
    // from CCLRIoCompletionManager::OnComplete which probes.
    CONTRACT_VIOLATION(SOToleranceViolation);

    CONSISTENCY_CHECK(g_pOverlappedDataClass && (this->GetMethodTable() == g_pOverlappedDataClass));

    _ASSERTE(GetThread() != NULL);
#ifdef CHECK_APP_DOMAIN_LEAKS
    if (m_pinSelf)
    {
        ADIndex id = HndGetHandleADIndex(m_pinSelf);
        _ASSERTE (id == ::GetAppDomain()->GetIndex());
    }
#endif

    if (m_pinSelf)
    {
        OBJECTHANDLE h = m_pinSelf;
        if (FastInterlockCompareExchangePointer((void**)&m_pinSelf, NULL, h) == h)
        {
            DestroyAsyncPinningHandle(h);
        }
    }

    EventHandle = 0;

}


void OverlappedDataObject::StartCleanup()
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) {MODE_COOPERATIVE;} else {DISABLED(MODE_COOPERATIVE);}
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (s_CleanupRequestCount == 0)
    {
        return;
    }

    LONG curCount = s_CleanupRequestCount;
    if (FastInterlockExchange((LONG*)&s_CleanupInProgress, TRUE) == FALSE)
    {
        {
            BOOL HasJob = Ref_HandleAsyncPinHandles();
            if (!HasJob)
            {
                s_CleanupInProgress = FALSE;
                FastInterlockExchangeAdd (&s_CleanupRequestCount, -curCount);
                return;
            }
        }

        if (!ThreadpoolMgr::DrainCompletionPortQueue())
        {
            s_CleanupInProgress = FALSE;
        }
        else
        {
            FastInterlockExchangeAdd (&s_CleanupRequestCount, -curCount);
        }
    }
}


void OverlappedDataObject::FinishCleanup()
{
    WRAPPER_CONTRACT;

    Thread *pThread = GetThread();
    BOOL fToggle = (pThread && !pThread->PreemptiveGCDisabled());
    if (fToggle)
    {
        pThread->DisablePreemptiveGC();
    }
    s_CleanupFreeHandle = TRUE;
    Ref_HandleAsyncPinHandles();

    s_CleanupInProgress = FALSE;
    if (s_CleanupRequestCount > 0)
    {
        StartCleanup();
    }
    if (fToggle)
    {
        pThread->EnablePreemptiveGC();
    }
}


void OverlappedDataObject::HandleAsyncPinHandle()
{
    WRAPPER_CONTRACT;

    _ASSERTE (s_CleanupInProgress);
    if (m_toBeCleaned || !ThreadpoolMgr::IsCompletionPortInitialized())
    {
        OBJECTHANDLE h = m_pinSelf;
        if (h)
        {
            if (FastInterlockCompareExchangePointer((void**)&m_pinSelf, NULL, h) == h)
            {
                DestroyAsyncPinningHandle(h);
            }
        }
    }
    else if (!s_CleanupFreeHandle)
    {
        m_toBeCleaned = 1;
    }
}

