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
//*****************************************************************************
//*****************************************************************************
#include "stdafx.h"
#include "eetoprofinterfaceimpl.h"
#include "contract.h"

//********** Globals. *********************************************************

// Note that for generic code ClassIDs and FunctionIDs are always uninstantiated.
// Thus we call GetTypicalTypeDefinition and GetTypicalMethodDefinition.
ClassID TypeIDToClassID(TypeID typeID)
{
    return reinterpret_cast< ClassID >(typeID);
}
FunctionID CodeIDToFunctionID(CodeID codeId)
{
    return reinterpret_cast< FunctionID >(codeId);
}




class EnsureGCPreemptiveHolder
{
public:
    FORCEINLINE EnsureGCPreemptiveHolder(ThreadID threadId)
    {
        if (threadId != 0)
        {
            m_threadId= threadId;
            m_bIsCoopMode = g_pProfToEEInterface->PreemptiveGCDisabled(m_threadId);
            if (m_bIsCoopMode)
                g_pProfToEEInterface->EnablePreemptiveGC(m_threadId);
        }
        else
            m_threadId = 0;
    }
    FORCEINLINE ~EnsureGCPreemptiveHolder()
    {
        if (m_threadId != 0 && m_bIsCoopMode)
            g_pProfToEEInterface->DisablePreemptiveGC(m_threadId);
    }
private:
    ThreadID    m_threadId;
    BOOL        m_bIsCoopMode;
};

class EnsureGCCooperativeHolder
{
public:
    FORCEINLINE EnsureGCCooperativeHolder(ThreadID threadId)
    {
        if (threadId != 0)
        {
            m_threadId= threadId;
            m_bIsCoopMode = g_pProfToEEInterface->PreemptiveGCDisabled(m_threadId);
            if (!m_bIsCoopMode)
                g_pProfToEEInterface->DisablePreemptiveGC(m_threadId);
        }
        else
            m_threadId = 0;
    }
    FORCEINLINE ~EnsureGCCooperativeHolder()
    {
        if (m_threadId != 0 && !m_bIsCoopMode)
            g_pProfToEEInterface->EnablePreemptiveGC(m_threadId);
    }
private:
    ThreadID    m_threadId;
    BOOL        m_bIsCoopMode;
};

// When we call into profiler code, we push one of these babies onto the stack to
// remember on the Thread how the profiler was called.  If the profiler calls back into us,
// we use the flags that this set to authorize.
//
// ProfToEEInterfaceImpl.cpp has something similar to this that it uses for
// the profiler enter, leave, tailcall, FunctionIDMapper hooks (this one is
// for all the ICorProfilerCallback(2) methods).  Since we're outside the VM DLL
// we're forced to use our lifeline to mscorwks.dll (g_pProfToEEInterface) to do
// our dirty work for us, which is why we have a separate implementation of this mini-holder.
class SetCallbackStateFlagsHolder
{
public:
    FORCEINLINE SetCallbackStateFlagsHolder(DWORD dwFlags)
    {
        m_threadId = 0;
        m_dwOriginalFullState = g_pProfToEEInterface->SetProfilerCallbackStateFlags(dwFlags, &m_threadId);
    }
    
    FORCEINLINE ~SetCallbackStateFlagsHolder()
    {
        if (m_threadId != 0)
        {
            g_pProfToEEInterface->SetProfilerCallbackFullState(m_dwOriginalFullState, m_threadId);
        }
    }
private:
    ThreadID    m_threadId;
    DWORD      m_dwOriginalFullState;
};

//********** Code. ************************************************************

EEToProfInterfaceImpl::t_AllocByClassData *EEToProfInterfaceImpl::m_pSavedAllocDataBlock = NULL;

EEToProfInterfaceImpl::EEToProfInterfaceImpl() :
    m_GUID(k_guidZero),
    m_lGUIDCount(0),
    m_pMovedRefDataFreeList(NULL),
    m_pRootRefDataFreeList(NULL)
{
}

HRESULT EEToProfInterfaceImpl::Init()
{
    // Used to initialize the WinWrap so that WszXXX works
    OnUnicodeSystem();
    return (S_OK);
}

void EEToProfInterfaceImpl::Terminate(BOOL fProcessDetach)
{
    g_pProfToEEInterface->Terminate();
    g_pProfToEEInterface = NULL;

    // Delete the structs associated with GC moved references
    while (m_pMovedRefDataFreeList)
    {
        t_MovedReferencesData *pDel = m_pMovedRefDataFreeList;
        m_pMovedRefDataFreeList = m_pMovedRefDataFreeList->pNext;
        delete pDel;
    }

    // Delete the structs associated with root references
    while (m_pRootRefDataFreeList)
    {
        t_RootReferencesData *pDel = m_pRootRefDataFreeList;
        m_pRootRefDataFreeList = m_pRootRefDataFreeList->pNext;
        delete pDel;
    }

    if (m_pSavedAllocDataBlock)
    {
        _ASSERTE((UINT)m_pSavedAllocDataBlock != 0xFFFFFFFF);

        _ASSERTE(m_pSavedAllocDataBlock->pHashTable != NULL);
        // Get rid of the hash table
        if (m_pSavedAllocDataBlock->pHashTable)
            delete m_pSavedAllocDataBlock->pHashTable;

        // Get rid of the two arrays used to hold class<->numinstance info
        if (m_pSavedAllocDataBlock->cLength != 0)
        {
            _ASSERTE(m_pSavedAllocDataBlock->arrClsId != NULL);
            _ASSERTE(m_pSavedAllocDataBlock->arrcObjects != NULL);

            delete [] m_pSavedAllocDataBlock->arrClsId;
            delete [] m_pSavedAllocDataBlock->arrcObjects;
        }

        // Get rid of the hash array used by the hash table
        if (m_pSavedAllocDataBlock->arrHash)
        {
            delete [] m_pSavedAllocDataBlock->arrHash;
        }
    }

    m_GUID = k_guidZero;

    // If we're in process detach, then do nothing related
    // to cleaning up the profiler DLL.
    if (g_pCallback && !fProcessDetach)
    {
        g_pCallback->Release();
        g_pCallback = NULL;
    }

    if (g_pEverettCallback_DELETE_ME && !fProcessDetach)
    {
        g_pEverettCallback_DELETE_ME->Release();
        g_pEverettCallback_DELETE_ME = NULL;
    }

    // The runtime can't free me, cause I'm in a separate DLL with my own
    // memory management.
    delete this;
}

// This is called by the EE if the profiling bit is set.
HRESULT EEToProfInterfaceImpl::CreateProfiler( __in LPWSTR wszCLSID, __in LPWSTR wszProfileDLL )
{
    CONTRACT_VIOLATION(ThrowsViolation);
    
    DWORD dwSupportEverett = REGUTIL::GetConfigDWORD(L"TempAllowEverettProfilers", 0);

    // Try and CoCreate the registered profiler
    ICorProfilerCallback *pCallback;

    HRESULT hr = CoCreateProfiler(wszCLSID, wszProfileDLL, &pCallback, (BOOL) dwSupportEverett);
    
    // If profiler was CoCreated, initialize it.
    if (SUCCEEDED(hr))
    {

        _ASSERTE(pCallback != NULL);

        //
        // Query to see if this profiler supports V2.0 callbacks, otherwise fail to load
        // the profiler.
        //
        ICorProfilerCallback2 *pCallback2;

        hr = pCallback->QueryInterface(IID_ICorProfilerCallback2, (void **)&pCallback2);

        if (FAILED(hr))
        {
            g_fSupportWhidbeyOnly_DELETE_ME = (dwSupportEverett == 0);

            if (g_fSupportWhidbeyOnly_DELETE_ME)
            {
                //
                // Indicate that we failed because the V2.0 callbacks are not supported.
                //
                RELEASE(pCallback);
                return E_NOINTERFACE;
            }
            else
            {
                g_pCallback = NULL;
                g_pEverettCallback_DELETE_ME = pCallback;
                g_pEverettCallback_DELETE_ME->AddRef();
            }
        }
        else
        {
            //
            // Save this off into our global
            //
            g_pCallback = pCallback2;
        }

        RELEASE(pCallback);

        // Create an ICorProfilerInfo object for the initialize
        CorProfInfo *pInfo = new CorProfInfo();
        _ASSERTE(pInfo != NULL);

        if (pInfo != NULL)
        {
            // Now call the initialize method on the profiler.

            g_pInfo = pInfo;

            pInfo->AddRef();

            if (g_pCallback == NULL)
            {
                _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

                hr = g_pEverettCallback_DELETE_ME->Initialize((IUnknown *)(ICorProfilerInfo *)pInfo);
            }
            else
            {
                hr = g_pCallback->Initialize((IUnknown *)(ICorProfilerInfo *)pInfo);
            }

            // If initialize failed, then they will not have addref'd the object
            // and it will die here.  If initialize succeeded, then if they want
            // the info interface they will have addref'd it and this will just
            // decrement the addref counter.
            pInfo->Release();
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }


        if (FAILED(hr))
        {
            if (g_pCallback != NULL)
            {
                RELEASE(g_pCallback);
                g_pCallback = NULL;
            }

            if (g_pEverettCallback_DELETE_ME != NULL)
            {
                RELEASE(g_pEverettCallback_DELETE_ME);
                g_pEverettCallback_DELETE_ME = NULL;
            }


            hr = E_OUTOFMEMORY;
        }
    }

    return (hr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// THREAD EVENTS
//


HRESULT EEToProfInterfaceImpl::ThreadCreated(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: Notifying profiler of created thread. ThreadId: 0x%p.\n", threadId));


    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        // Notify the profiler of the newly created thread.
        return (g_pEverettCallback_DELETE_ME->ThreadCreated(threadId));
    }
    else
    {
        _ASSERTE(g_pCallback != NULL);

        // Notify the profiler of the newly created thread.
        return (g_pCallback->ThreadCreated(threadId));
    }
}

HRESULT EEToProfInterfaceImpl::ThreadDestroyed(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: Notifying profiler of destroyed thread. ThreadId: 0x%p.\n", threadId));
    
    g_pProfToEEInterface->SetProfilerCallbacksAllowed(threadId, FALSE);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        // Notify the profiler of the destroyed thread
        return (g_pEverettCallback_DELETE_ME->ThreadDestroyed(threadId));
    }
    else
    {
        _ASSERTE(g_pCallback != NULL);

        // Notify the profiler of the destroyed thread
        return (g_pCallback->ThreadDestroyed(threadId));
    }
}

HRESULT EEToProfInterfaceImpl::ThreadAssignedToOSThread(ThreadID managedThreadId,
                                                              DWORD osThreadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(managedThreadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: Notifying profiler of thread assignment.  ThreadId: 0x%p, OSThreadId: 0x%08x\n", 
                managedThreadId, osThreadId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        // Notify the profiler of the destroyed thread
        return (g_pEverettCallback_DELETE_ME->ThreadAssignedToOSThread(managedThreadId, osThreadId));
    }
    else
    {
        _ASSERTE(g_pCallback != NULL);

        // Notify the profiler of the destroyed thread
        return (g_pCallback->ThreadAssignedToOSThread(managedThreadId, osThreadId));
    }

}

HRESULT EEToProfInterfaceImpl::ThreadNameChanged(ThreadID managedThreadId,
                                                 ULONG cchName,
                                                 WCHAR name[])
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(managedThreadId));

    //
    // Only exists on Callback2
    //
    if (g_pCallback != NULL)
    {
        _ASSERTE(g_pCallback != NULL);

        LOG((LF_CORPROF, LL_INFO100, "**PROF: Notifying profiler of thread name change.\n"));

        SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);
        return g_pCallback->ThreadNameChanged(managedThreadId, cchName, name);
    }
    else
    {
        return S_OK;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE STARTUP/SHUTDOWN EVENTS
//

HRESULT EEToProfInterfaceImpl::Shutdown(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    
    LOG((LF_CORPROF, LL_INFO10, "**PROF: Notifying profiler that "
         "shutdown is beginning.\n"));

    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->Shutdown());
    }
    else
    {
        _ASSERTE(g_pCallback != NULL);

        return (g_pCallback->Shutdown());
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// JIT/FUNCTION EVENTS
//

HRESULT EEToProfInterfaceImpl::FunctionUnloadStarted(ThreadID threadId, CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(codeId);
    FunctionID functionId = CodeIDToFunctionID(codeId);


    LOG((LF_CORPROF, LL_INFO100, "**PROF: FunctionUnloadStarted 0x%p.\n", functionId));

    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->FunctionUnloadStarted(functionId));
    }
    else
    {
        return (g_pCallback->FunctionUnloadStarted(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::JITCompilationFinished(ThreadID threadId, CodeID codeId,
                                                      HRESULT hrStatus, BOOL fIsSafeToBlock)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(codeId);
    FunctionID functionId = CodeIDToFunctionID(codeId);

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: JITCompilationFinished 0x%p, hr=0x%08x.\n", functionId, hrStatus));

    EnsureGCPreemptiveHolder *pgc = NULL;

    if (fIsSafeToBlock)
    {
        pgc = (EnsureGCPreemptiveHolder *)_alloca(sizeof(EnsureGCPreemptiveHolder));
        pgc = new ((void *)pgc) EnsureGCPreemptiveHolder(threadId);
    }

    HRESULT hr;

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        hr = g_pEverettCallback_DELETE_ME->JITCompilationFinished(functionId, hrStatus, fIsSafeToBlock);
    }
    else
    {
        hr = g_pCallback->JITCompilationFinished(functionId, hrStatus, fIsSafeToBlock);
    }

    if (fIsSafeToBlock)
    {
        pgc->~EnsureGCPreemptiveHolder();
    }

    return (hr);
}


HRESULT EEToProfInterfaceImpl::JITCompilationStarted(ThreadID threadId, CodeID codeId,
                                                     BOOL fIsSafeToBlock)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(codeId);
    FunctionID functionId = CodeIDToFunctionID(codeId);

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: JITCompilationStarted 0x%p.\n", functionId));

    EnsureGCPreemptiveHolder *pgc = NULL;

    if (fIsSafeToBlock)
    {
        pgc = (EnsureGCPreemptiveHolder *)_alloca(sizeof(EnsureGCPreemptiveHolder));
        pgc = new ((void *)pgc) EnsureGCPreemptiveHolder(threadId);
    }

    HRESULT hr;

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        hr = g_pEverettCallback_DELETE_ME->JITCompilationStarted(functionId, fIsSafeToBlock);
    }
    else
    {
        hr = g_pCallback->JITCompilationStarted(functionId, fIsSafeToBlock);
    }

    if (fIsSafeToBlock)
    {
        pgc->~EnsureGCPreemptiveHolder();
    }

    return (hr);
}

HRESULT EEToProfInterfaceImpl::JITCachedFunctionSearchStarted(
                                    /* [in] */    ThreadID   threadId,
                                    /* [in] */  CodeID codeId,
                                    /* [out] */ BOOL       *pbUseCachedFunction)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(codeId);
    FunctionID functionId = CodeIDToFunctionID(codeId);
    _ASSERTE(pbUseCachedFunction != NULL);

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: JITCachedFunctionSearchStarted 0x%p.\n", functionId));

    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->JITCachedFunctionSearchStarted(functionId, pbUseCachedFunction));
    }
    else
    {
        return (g_pCallback->JITCachedFunctionSearchStarted(functionId, pbUseCachedFunction));
    }
}

HRESULT EEToProfInterfaceImpl::JITCachedFunctionSearchFinished(
                                    /* [in] */    ThreadID threadId,
                                    /* [in] */  CodeID codeId,
                                    /* [in] */  COR_PRF_JIT_CACHE result)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(codeId);
    FunctionID functionId = CodeIDToFunctionID(codeId);

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: JITCachedFunctionSearchFinished 0x%p, %s.\n", functionId,
        (result == COR_PRF_CACHED_FUNCTION_FOUND ? "Cached function found" : "Cached function not found")));

    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->JITCachedFunctionSearchFinished(functionId, result));
    }
    else
    {
        return (g_pCallback->JITCachedFunctionSearchFinished(functionId, result));
    }
}


HRESULT EEToProfInterfaceImpl::JITFunctionPitched(ThreadID threadId, CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(codeId);
    FunctionID functionId = CodeIDToFunctionID(codeId);

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: JITFunctionPitched 0x%p.\n", functionId));

    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->JITFunctionPitched(functionId));
    }
    else
    {
        return (g_pCallback->JITFunctionPitched(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::JITInlining(
    /* [in] */  ThreadID      threadId,
    /* [in] */  CodeID     eeCallerId,
    /* [in] */  CodeID     eeCalleeId,
    /* [out] */ BOOL         *pfShouldInline)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(eeCallerId);
    _ASSERTE(eeCalleeId);
    FunctionID callerId = CodeIDToFunctionID(eeCallerId);
    FunctionID calleeId = CodeIDToFunctionID(eeCalleeId);

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: JITInlining caller: 0x%p, callee: 0x%p.\n", callerId, calleeId));

    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->JITInlining(callerId, calleeId, pfShouldInline));
    }
    else
    {
        return (g_pCallback->JITInlining(callerId, calleeId, pfShouldInline));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MODULE EVENTS
//

HRESULT EEToProfInterfaceImpl::ModuleLoadStarted(ThreadID threadId, ModuleID moduleId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(moduleId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: ModuleLoadStarted 0x%p.\n", moduleId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ModuleLoadStarted(moduleId));
    }
    else
    {
        return (g_pCallback->ModuleLoadStarted(moduleId));
    }
}


HRESULT EEToProfInterfaceImpl::ModuleLoadFinished(
    ThreadID    threadId,
    ModuleID    moduleId,
    HRESULT        hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(moduleId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: ModuleLoadFinished 0x%p.\n", moduleId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ModuleLoadFinished(moduleId, hrStatus));
    }
    else
    {
        return (g_pCallback->ModuleLoadFinished(moduleId, hrStatus));
    }
}



HRESULT EEToProfInterfaceImpl::ModuleUnloadStarted(
    ThreadID    threadId,
    ModuleID    moduleId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(moduleId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: ModuleUnloadStarted 0x%p.\n", moduleId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ModuleUnloadStarted(moduleId));
    }
    else
    {
        return (g_pCallback->ModuleUnloadStarted(moduleId));
    }
}


HRESULT EEToProfInterfaceImpl::ModuleUnloadFinished(
    ThreadID    threadId,
    ModuleID    moduleId,
    HRESULT        hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(moduleId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: ModuleUnloadFinished 0x%p.\n", moduleId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ModuleUnloadFinished(moduleId, hrStatus));
    }
    else
    {
        return (g_pCallback->ModuleUnloadFinished(moduleId, hrStatus));
    }
}


HRESULT EEToProfInterfaceImpl::ModuleAttachedToAssembly(
    ThreadID    threadId,
    ModuleID    moduleId,
    AssemblyID  AssemblyId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(moduleId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: ModuleAttachedToAssembly 0x%p, 0x%p.\n", moduleId, AssemblyId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ModuleAttachedToAssembly(moduleId, AssemblyId));
    }
    else
    {
        return (g_pCallback->ModuleAttachedToAssembly(moduleId, AssemblyId));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS EVENTS
//

HRESULT EEToProfInterfaceImpl::ClassLoadStarted(
    ThreadID    threadId,
    TypeID        typeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(typeId);
    ClassID classId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO100, "**PROF: ClassLoadStarted 0x%p.\n", classId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ClassLoadStarted(classId));
    }
    else
    {
        return (g_pCallback->ClassLoadStarted(classId));
    }
}


HRESULT EEToProfInterfaceImpl::ClassLoadFinished(
    ThreadID    threadId,
    TypeID        typeId,
    HRESULT        hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(typeId);
    ClassID classId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO100, "**PROF: ClassLoadFinished 0x%p, 0x%08x.\n", classId, hrStatus));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ClassLoadFinished(classId, hrStatus));
    }
    else
    {
        return (g_pCallback->ClassLoadFinished(classId, hrStatus));
    }
}


HRESULT EEToProfInterfaceImpl::ClassUnloadStarted(
    ThreadID    threadId,
    TypeID typeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(typeId);
    ClassID classId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO100, "**PROF: ClassUnloadStarted 0x%p.\n", classId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ClassUnloadStarted(classId));
    }
    else
    {
        return (g_pCallback->ClassUnloadStarted(classId));
    }
}


HRESULT EEToProfInterfaceImpl::ClassUnloadFinished(
    ThreadID    threadId,
    TypeID typeId,
    HRESULT     hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(typeId);
    ClassID classId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO100, "**PROF: ClassUnloadFinished 0x%p, 0x%08x.\n", classId, hrStatus));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ClassUnloadFinished(classId, hrStatus));
    }
    else
    {
        return (g_pCallback->ClassUnloadFinished(classId, hrStatus));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// APPDOMAIN EVENTS
//

HRESULT EEToProfInterfaceImpl::AppDomainCreationStarted(
    ThreadID    threadId,
    AppDomainID appDomainId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(appDomainId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AppDomainCreationStarted 0x%p.\n", appDomainId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AppDomainCreationStarted(appDomainId));
    }
    else
    {
        return (g_pCallback->AppDomainCreationStarted(appDomainId));
    }
}


HRESULT EEToProfInterfaceImpl::AppDomainCreationFinished(
    ThreadID    threadId,
    AppDomainID appDomainId,
    HRESULT     hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(appDomainId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AppDomainCreationFinished 0x%p, 0x%08x.\n", appDomainId, hrStatus));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AppDomainCreationFinished(appDomainId, hrStatus));
    }
    else
    {
        return (g_pCallback->AppDomainCreationFinished(appDomainId, hrStatus));
    }
}

HRESULT EEToProfInterfaceImpl::AppDomainShutdownStarted(
    ThreadID    threadId,
    AppDomainID appDomainId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(appDomainId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AppDomainShutdownStarted 0x%p.\n", appDomainId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AppDomainShutdownStarted(appDomainId));
    }
    else
    {
        return (g_pCallback->AppDomainShutdownStarted(appDomainId));
    }
}

HRESULT EEToProfInterfaceImpl::AppDomainShutdownFinished(
    ThreadID    threadId,
    AppDomainID appDomainId,
    HRESULT     hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(appDomainId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AppDomainShutdownFinished 0x%p, 0x%08x.\n", appDomainId, hrStatus));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AppDomainShutdownFinished(appDomainId, hrStatus));
    }
    else
    {
        return (g_pCallback->AppDomainShutdownFinished(appDomainId, hrStatus));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ASSEMBLY EVENTS
//

HRESULT EEToProfInterfaceImpl::AssemblyLoadStarted(
    ThreadID    threadId,
    AssemblyID  assemblyId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(assemblyId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AssemblyLoadStarted 0x%p.\n", assemblyId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AssemblyLoadStarted(assemblyId));
    }
    else
    {
        return (g_pCallback->AssemblyLoadStarted(assemblyId));
    }
}

HRESULT EEToProfInterfaceImpl::AssemblyLoadFinished(
    ThreadID    threadId,
    AssemblyID  assemblyId,
    HRESULT     hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(assemblyId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AssemblyLoadFinished 0x%p, 0x%08x.\n", assemblyId, hrStatus));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AssemblyLoadFinished(assemblyId, hrStatus));
    }
    else
    {
        return (g_pCallback->AssemblyLoadFinished(assemblyId, hrStatus));
    }
}

HRESULT EEToProfInterfaceImpl::AssemblyUnloadStarted(
    ThreadID    threadId,
    AssemblyID  assemblyId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(assemblyId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AssemblyUnloadStarted 0x%p.\n", assemblyId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AssemblyUnloadStarted(assemblyId));
    }
    else
    {
        return (g_pCallback->AssemblyUnloadStarted(assemblyId));
    }
}

HRESULT EEToProfInterfaceImpl::AssemblyUnloadFinished(
    ThreadID    threadId,
    AssemblyID  assemblyId,
    HRESULT     hrStatus)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    _ASSERTE(assemblyId != 0);
    LOG((LF_CORPROF, LL_INFO10, "**PROF: AssemblyUnloadFinished 0x%p, 0x%08x.\n", assemblyId, hrStatus));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->AssemblyUnloadFinished(assemblyId, hrStatus));
    }
    else
    {
        return (g_pCallback->AssemblyUnloadFinished(assemblyId, hrStatus));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TRANSITION EVENTS
//

HRESULT EEToProfInterfaceImpl::UnmanagedToManagedTransition(
    CodeID codeId,
    COR_PRF_TRANSITION_REASON reason)
{
    FunctionID functionId = CodeIDToFunctionID(codeId);
    _ASSERTE(reason == COR_PRF_TRANSITION_CALL || reason == COR_PRF_TRANSITION_RETURN);

    LOG((LF_CORPROF, LL_INFO10000, "**PROF: UnmanagedToManagedTransition 0x%p.\n", functionId));
    // Unnecessary to toggle GC, as it is guaranteed that preemptive GC is enabled for this call
    //EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return(g_pEverettCallback_DELETE_ME->UnmanagedToManagedTransition(functionId, reason));
    }
    else
    {
        return(g_pCallback->UnmanagedToManagedTransition(functionId, reason));
    }
}

HRESULT EEToProfInterfaceImpl::ManagedToUnmanagedTransition(
    CodeID codeId,
    COR_PRF_TRANSITION_REASON reason)
{
    FunctionID functionId = CodeIDToFunctionID(codeId);
    _ASSERTE(reason == COR_PRF_TRANSITION_CALL || reason == COR_PRF_TRANSITION_RETURN);

    LOG((LF_CORPROF, LL_INFO10000, "**PROF: NotifyManagedToUnanagedTransition 0x%p.\n", functionId));
    // Unnecessary to toggle GC, as it is guaranteed that preemptive GC is enabled for this call
    //EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ManagedToUnmanagedTransition(functionId, reason));
    }
    else
    {
        return (g_pCallback->ManagedToUnmanagedTransition(functionId, reason));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXCEPTION EVENTS
//

HRESULT EEToProfInterfaceImpl::ExceptionThrown(
    ThreadID threadId,
    ObjectID thrownObjectId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionThrown. ObjectID: 0x%p. ThreadID: 0x%p\n",
         thrownObjectId, threadId));

    _ASSERTE(g_pInfo != NULL);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionThrown(thrownObjectId));
    }
    else
    {
        return (g_pCallback->ExceptionThrown(thrownObjectId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionSearchFunctionEnter(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionSearchFunctionEnter. ThreadID: 0x%p, CodeID: 0x%p\n", threadId, codeId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionSearchFunctionEnter(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionSearchFunctionEnter(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionSearchFunctionLeave(
    ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionSearchFunctionLeave. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionSearchFunctionLeave());
    }
    else
    {
        return (g_pCallback->ExceptionSearchFunctionLeave());
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionSearchFilterEnter(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionSearchFilterEnter. ThreadID: 0x%p, CodeID: 0x%p\n", threadId, codeId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionSearchFilterEnter(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionSearchFilterEnter(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionSearchFilterLeave(
    ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionFilterLeave. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionSearchFilterLeave());
    }
    else
    {
        return (g_pCallback->ExceptionSearchFilterLeave());
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionSearchCatcherFound(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionSearchCatcherFound.  ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionSearchCatcherFound(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionSearchCatcherFound(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionOSHandlerEnter(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionOSHandlerEnter. ThreadID: 0x%p, CodeID: 0x%p\n", threadId, codeId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionOSHandlerEnter(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionOSHandlerEnter(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionOSHandlerLeave(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionOSHandlerLeave. ThreadID: 0x%p\n", threadId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionOSHandlerLeave(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionOSHandlerLeave(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionUnwindFunctionEnter(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionUnwindFunctionEnter. ThreadID: 0x%p, CodeID: 0x%p\n", threadId, codeId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionUnwindFunctionEnter(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionUnwindFunctionEnter(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionUnwindFunctionLeave(
    ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionUnwindFunctionLeave. ThreadID: 0x%p\n", threadId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionUnwindFunctionLeave());
    }
    else
    {
        return (g_pCallback->ExceptionUnwindFunctionLeave());
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionUnwindFinallyEnter(
    ThreadID threadId,
    CodeID codeId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionUnwindFinallyEnter. ThreadID: 0x%p, CodeID: 0x%p\n", threadId, codeId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionUnwindFinallyEnter(functionId));
    }
    else
    {
        return (g_pCallback->ExceptionUnwindFinallyEnter(functionId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionUnwindFinallyLeave(
    ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionUnwindFinallyLeave. ThreadID: 0x%p\n", threadId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionUnwindFinallyLeave());
    }
    else
    {
        return (g_pCallback->ExceptionUnwindFinallyLeave());
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionCatcherEnter(
    ThreadID threadId,
    CodeID codeId,
    ObjectID objectId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    FunctionID functionId = CodeIDToFunctionID(codeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionCatcherEnter.        ThreadID: 0x%p, CodeID: 0x%p\n", threadId, codeId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionCatcherEnter(functionId, objectId));
    }
    else
    {
        return (g_pCallback->ExceptionCatcherEnter(functionId, objectId));
    }
}

HRESULT EEToProfInterfaceImpl::ExceptionCatcherLeave(
    ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ExceptionCatcherLeave.        ThreadID: 0x%p\n", threadId));

    // NOTE: Cannot enable preemptive GC here, since the stack may not be in a GC-friendly state.
    //       Thus, the profiler cannot block on this call.

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ExceptionCatcherLeave());
    }
    else
    {
        return (g_pCallback->ExceptionCatcherLeave());
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCW EVENTS
//
HRESULT EEToProfInterfaceImpl::COMClassicVTableCreated(
    /* [in] */ TypeID typeId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void *pVTable,
    /* [in] */ ULONG cSlots,
    /* [in] */ ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    ClassID wrappedClassId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO100, "**PROF: COMClassicWrapperCreated %#x %#08x... %#x %d.\n",
         wrappedClassId, implementedIID.Data1, pVTable, cSlots));

    // Someone's doing a forbid GC that prevents this
    // EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->COMClassicVTableCreated(wrappedClassId, implementedIID, pVTable, cSlots));
    }
    else
    {
        return (g_pCallback->COMClassicVTableCreated(wrappedClassId, implementedIID, pVTable, cSlots));
    }
}

HRESULT EEToProfInterfaceImpl::COMClassicVTableDestroyed(
    /* [in] */ TypeID typeId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void *pVTable,
    /* [in] */ ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    ClassID wrappedClassId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO100, "**PROF: COMClassicWrapperDestroyed %#x %#08x... %#x.\n",
         wrappedClassId, implementedIID.Data1, pVTable));

    // Someone's doing a forbid GC that prevents this
    // EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->COMClassicVTableDestroyed(wrappedClassId, implementedIID, pVTable));
    }
    else
    {
        return (g_pCallback->COMClassicVTableDestroyed(wrappedClassId, implementedIID, pVTable));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GC EVENTS
//

HRESULT EEToProfInterfaceImpl::RuntimeSuspendStarted(
    COR_PRF_SUSPEND_REASON suspendReason, ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: RuntimeSuspendStarted. ThreadID 0x%p.\n",
         threadId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeSuspendStarted(suspendReason));
    }
    else
    {
        return (g_pCallback->RuntimeSuspendStarted(suspendReason));
    }
}

HRESULT EEToProfInterfaceImpl::RuntimeSuspendFinished(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: RuntimeSuspendFinished. ThreadID 0x%p.\n",
         threadId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeSuspendFinished());
    }
    else
    {
        return (g_pCallback->RuntimeSuspendFinished());
    }
}

HRESULT EEToProfInterfaceImpl::RuntimeSuspendAborted(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: RuntimeSuspendAborted. ThreadID 0x%p.\n",
         threadId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeSuspendAborted());
    }
    else
    {
        return (g_pCallback->RuntimeSuspendAborted());
    }
}

HRESULT EEToProfInterfaceImpl::RuntimeResumeStarted(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: RuntimeResumeStarted. ThreadID 0x%p.\n",
         threadId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeResumeStarted());
    }
    else
    {
        return (g_pCallback->RuntimeResumeStarted());
    }
}

HRESULT EEToProfInterfaceImpl::RuntimeResumeFinished(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO100, "**PROF: RuntimeResumeFinished. ThreadID 0x%p.\n",
         threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeResumeFinished());
    }
    else
    {
        return (g_pCallback->RuntimeResumeFinished());
    }
}

HRESULT EEToProfInterfaceImpl::RuntimeThreadSuspended(ThreadID suspendedThreadId,
                                                      ThreadID threadId)
{
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RuntimeThreadSuspended. ThreadID 0x%p.\n",
         suspendedThreadId));
    
    //
    // We may have already indicated to the profiler that this thread has died, but 
    // the runtime may continue to suspend this thread during the process of destroying
    // the thread, so we do not want to indicate to the profiler these suspensions.
    //
    if(!g_pProfToEEInterface->ProfilerCallbacksAllowed(suspendedThreadId))
    {
        return S_OK;
    }

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeThreadSuspended(suspendedThreadId));
    }
    else
    {
        return (g_pCallback->RuntimeThreadSuspended(suspendedThreadId));
    }
}

HRESULT EEToProfInterfaceImpl::RuntimeThreadResumed(ThreadID resumedThreadId,
                                                    ThreadID threadId)
{
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RuntimeThreadResumed. ThreadID 0x%p.\n",
         resumedThreadId));

    //
    // We may have already indicated to the profiler that this thread has died, but 
    // the runtime may resume this thread during the process of destroying
    // the thread, so we do not want to indicate to the profiler these resumes.
    //
    if(!g_pProfToEEInterface->ProfilerCallbacksAllowed(resumedThreadId))
    {
        return S_OK;
    }

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RuntimeThreadResumed(resumedThreadId));
    }
    else
    {
        return (g_pCallback->RuntimeThreadResumed(resumedThreadId));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// REMOTING
//

HRESULT EEToProfInterfaceImpl::RemotingClientInvocationStarted(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingClientInvocationStarted. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingClientInvocationStarted());
    }
    else
    {
        return (g_pCallback->RemotingClientInvocationStarted());
    }
}

HRESULT EEToProfInterfaceImpl::RemotingClientSendingMessage(ThreadID threadId, GUID *pCookie,
                                                            BOOL fIsAsync)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingClientSendingMessage. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingClientSendingMessage(pCookie, fIsAsync));
    }
    else
    {
        return (g_pCallback->RemotingClientSendingMessage(pCookie, fIsAsync));
    }
}

HRESULT EEToProfInterfaceImpl::RemotingClientReceivingReply(ThreadID threadId, GUID *pCookie,
                                                            BOOL fIsAsync)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingClientReceivingReply. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingClientReceivingReply(pCookie, fIsAsync));
    }
    else
    {
        return (g_pCallback->RemotingClientReceivingReply(pCookie, fIsAsync));
    }
}

HRESULT EEToProfInterfaceImpl::RemotingClientInvocationFinished(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingClientInvocationFinished. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingClientInvocationFinished());
    }
    else
    {
        return (g_pCallback->RemotingClientInvocationFinished());
    }
}

HRESULT EEToProfInterfaceImpl::RemotingServerReceivingMessage(ThreadID threadId, GUID *pCookie,
                                                              BOOL fIsAsync)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingServerReceivingMessage. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingServerReceivingMessage(pCookie, fIsAsync));
    }
    else
    {
        return (g_pCallback->RemotingServerReceivingMessage(pCookie, fIsAsync));
    }
}

HRESULT EEToProfInterfaceImpl::RemotingServerInvocationStarted(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingServerInvocationStarted. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingServerInvocationStarted());
    }
    else
    {
        return (g_pCallback->RemotingServerInvocationStarted());
    }
}

HRESULT EEToProfInterfaceImpl::RemotingServerInvocationReturned(ThreadID threadId)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingServerInvocationReturned. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingServerInvocationReturned());
    }
    else
    {
        return (g_pCallback->RemotingServerInvocationReturned());
    }
}

HRESULT EEToProfInterfaceImpl::RemotingServerSendingReply(ThreadID threadId, GUID *pCookie,
                                                          BOOL fIsAsync)
{
    _ASSERTE(g_pProfToEEInterface->ProfilerCallbacksAllowed(threadId));
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: RemotingServerSendingReply. ThreadID: 0x%p\n", threadId));
    EnsureGCPreemptiveHolder gc(threadId);

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->RemotingServerSendingReply(pCookie, fIsAsync));
    }
    else
    {
        return (g_pCallback->RemotingServerSendingReply(pCookie, fIsAsync));
    }
}

HRESULT EEToProfInterfaceImpl::InitGUID()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (IsEqualGUID(m_GUID, k_guidZero))
    {
        return (CoCreateGuid(&m_GUID));
    }

    return (S_OK);
}

void EEToProfInterfaceImpl::GetGUID(GUID *pGUID)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    // the member GUID and the argument should both be valid
    _ASSERTE(!(IsEqualGUID(m_GUID, k_guidZero)));
    _ASSERTE(pGUID); 

    // Copy the contents of the template GUID
    memcpy(pGUID, &m_GUID, sizeof(GUID));

    // Adjust the last two bytes
    pGUID->Data4[6] = (BYTE) GetCurrentThreadId();
    pGUID->Data4[7] = (BYTE) InterlockedIncrement((LPLONG)&m_lGUIDCount);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GC EVENTS
//

HRESULT EEToProfInterfaceImpl::ObjectAllocated(
    /* [in] */ ObjectID objectId,
    /* [in] */ TypeID typeId)
{
    ClassID classId = TypeIDToClassID(typeId);
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ObjectAllocated. ObjectID: 0x%p.  ClassID: 0x%p\n", objectId, classId));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ObjectAllocated(objectId, classId));
    }
    else
    {
        return (g_pCallback->ObjectAllocated(objectId, classId));
    }
}


EEToProfInterfaceImpl::t_MovedReferencesData *EEToProfInterfaceImpl::AllocateMovedReferencesData()
{
    // Lock for access to the free list
    m_critSecMovedRefsFL.Lock();

    t_MovedReferencesData *pData;
    if (m_pMovedRefDataFreeList == NULL)
    {
        // Unlock immediately, since we have no use for the free list and
        // we don't want to block anyone else.
        m_critSecMovedRefsFL.UnLock();

        // Allocate struct
        pData = new (nothrow) t_MovedReferencesData;
        if (!pData)
            return NULL;

    }

    // Otherwise, grab one from the list of free blocks
    else
    {
        // Get the first element from the free list
        pData = m_pMovedRefDataFreeList;
        m_pMovedRefDataFreeList = m_pMovedRefDataFreeList->pNext;

        // Done, let others in.
        m_critSecMovedRefsFL.UnLock();
    }

    // Now init the new block

    // Set our index to the beginning
    pData->curIdx = 0;
    pData->compactingCount = 0;

    return pData;
}

void EEToProfInterfaceImpl::FreeMovedReferencesData(struct t_MovedReferencesData *pData)
{
    m_critSecMovedRefsFL.Lock();
    pData->pNext = m_pMovedRefDataFreeList;
    m_pMovedRefDataFreeList = pData;
    m_critSecMovedRefsFL.UnLock();
}

HRESULT EEToProfInterfaceImpl::MovedReference(BYTE *pbMemBlockStart,
                                              BYTE *pbMemBlockEnd,
                                              ptrdiff_t cbRelocDistance,
                                              void *pHeapId,
                                              BOOL compacting)
{
    _ASSERTE(pHeapId);
    _ASSERTE(*((size_t *)pHeapId) != (size_t)(-1));

    // Get a pointer to the data for this heap
    t_MovedReferencesData *pData = (t_MovedReferencesData *)(*((size_t *)pHeapId));

    // If this is the first notification of a moved reference for this heap
    // in this particular gc activation, then we need to get a ref data block
    // from the free list of blocks, or if that's empty then we need to
    // allocate a new one.
    if (pData == NULL)
    {
        pData = AllocateMovedReferencesData();
        if (pData == NULL)
            return (E_OUTOFMEMORY);

        // Set the cookie so that we will be provided it on subsequent
        // callbacks
        ((*((size_t *)pHeapId))) = (size_t)pData;
    }

    _ASSERTE(pData->curIdx >= 0 && pData->curIdx <= MAX_REFERENCES);

    // If the struct has been filled, then we need to notify the profiler of
    // these moved references and clear the struct for the next load of
    // moved references
    if (pData->curIdx == MAX_REFERENCES)
    {
        MovedReferences(pData);
        pData->curIdx = 0;
        pData->compactingCount = 0;
    }

    // Now save the information in the struct
    pData->arrpbMemBlockStartOld[pData->curIdx] = pbMemBlockStart;
    pData->arrpbMemBlockStartNew[pData->curIdx] = pbMemBlockStart + cbRelocDistance;
    pData->arrMemBlockSize[pData->curIdx] = (ULONG)(pbMemBlockEnd - pbMemBlockStart);

    // Make sure the size of the block is not larger than 4 GB.  The maximum
    // size of a single block of memory relocated by the GC is the segment size,
    // which is 64 MB.  If the size gets larger than 4 GB, we either have to update
    // the API or split up the block so that it takes up more than one element in the
    // array.
    _ASSERTE((pbMemBlockEnd - pbMemBlockStart) == (ptrdiff_t)(pData->arrMemBlockSize[pData->curIdx]));

    // Increment the index into the parallel arrays
    pData->curIdx += 1;

    // Keep track of whether this is a compacting collection
    if (compacting)
    {
        pData->compactingCount += 1;
        // The gc is supposed to make up its mind whether this is a compacting collection or not
        // Thus if this one is compacting, everything so far had to say compacting
        _ASSERTE(pData->compactingCount == pData->curIdx);
    }
    else
    {
        // The gc is supposed to make up its mind whether this is a compacting collection or not
        // Thus if this one is non-compacting, everything so far had to say non-compacting
        _ASSERTE(pData->compactingCount == 0 && cbRelocDistance == 0);
    }
    return (S_OK);
}

HRESULT EEToProfInterfaceImpl::EndMovedReferences(void *pHeapId)
{
    _ASSERTE(pHeapId);
    _ASSERTE((*((size_t *)pHeapId)) != (size_t)(-1));

    HRESULT hr = S_OK;

    // Get a pointer to the data for this heap
    t_MovedReferencesData *pData = (t_MovedReferencesData *)(*((size_t *)pHeapId));

    // If there were no moved references, profiler doesn't need to know
    if (!pData)
        return (S_OK);

    // Communicate the moved references to the profiler
    _ASSERTE(pData->curIdx> 0);
    hr = MovedReferences(pData);

    // Now we're done with the data block, we can shove it onto the free list
    m_critSecMovedRefsFL.Lock();
    pData->pNext = m_pMovedRefDataFreeList;
    m_pMovedRefDataFreeList = pData;
    m_critSecMovedRefsFL.UnLock();

#ifdef _DEBUG
    // Set the cookie to an invalid number
    (*((size_t *)pHeapId)) = (size_t)(-1);
#endif // _DEBUG

    return (hr);
}

HRESULT EEToProfInterfaceImpl::MovedReferences(t_MovedReferencesData *pData)
{
    LOG((LF_CORPROF, LL_INFO10000, "**PROF: MovedReferences.\n"));

    if (pData->curIdx == 0)
        return S_OK;

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->MovedReferences((ULONG)pData->curIdx,
                                                              (ObjectID *)pData->arrpbMemBlockStartOld,
                                                              (ObjectID *)pData->arrpbMemBlockStartNew,
                                                              (ULONG *)pData->arrMemBlockSize));
    }
    else
    {
        if (pData->compactingCount != 0)
        {
            _ASSERTE(pData->curIdx == pData->compactingCount);
            return (g_pCallback->MovedReferences((ULONG)pData->curIdx,
                                                 (ObjectID *)pData->arrpbMemBlockStartOld,
                                                 (ObjectID *)pData->arrpbMemBlockStartNew,
                                                 (ULONG *)pData->arrMemBlockSize));
        }
        else
        {
            return (g_pCallback->SurvivingReferences((ULONG)pData->curIdx,
                                                     (ObjectID *)pData->arrpbMemBlockStartOld,
                                                     (ULONG *)pData->arrMemBlockSize));
        }
    }
}

HRESULT EEToProfInterfaceImpl::RootReference(ObjectID objId, void *pHeapId)
{
    _ASSERTE(pHeapId);
    _ASSERTE((*((size_t *)pHeapId)) != (size_t)(-1));

    // Get a pointer to the data for this heap
    t_RootReferencesData *pData = (t_RootReferencesData *)(*((size_t *)pHeapId));

    // If this is the first notification of a root reference for this heap
    // in this particular gc activation, then we need to get a root data block
    // from the free list of blocks, or if that's empty then we need to
    // allocate a new one.
    if (pData == NULL)
    {
        // Lock for access to the free list
        m_critSecRootRefsFL.Lock();

        if (m_pRootRefDataFreeList == NULL)
        {
            // Unlock immediately, since we have no use for the free list and
            // we don't want to block anyone else.
            m_critSecRootRefsFL.UnLock();

            // Allocate struct
            pData = new (nothrow) t_RootReferencesData;
            if (!pData)
                return (E_OUTOFMEMORY);

        }

        // Otherwise, grab one from the list of free blocks
        else
        {
            // Get the first element from the free list
            pData = m_pRootRefDataFreeList;
            m_pRootRefDataFreeList = m_pRootRefDataFreeList->pNext;

            // Done, let others in.
            m_critSecRootRefsFL.UnLock();
        }

        // Now init the new block

        // Set our index to the beginning
        pData->curIdx = 0;

        // Set the cookie so that we will be provided it on subsequent
        // callbacks
        *((size_t *)pHeapId) = (size_t)pData;
    }

    _ASSERTE(pData->curIdx >= 0 && pData->curIdx <= MAX_ROOTS);

    // If the struct has been filled, then we need to notify the profiler of
    // these root references and clear the struct for the next load of
    // root references
    if (pData->curIdx == MAX_ROOTS)
    {
        RootReferences(pData);
        pData->curIdx = 0;
    }

    // Now save the information in the struct
    pData->arrRoot[pData->curIdx++] = objId;

    return (S_OK);
}

HRESULT EEToProfInterfaceImpl::EndRootReferences(void *pHeapId)
{
    _ASSERTE(pHeapId);
    _ASSERTE((*((size_t *)pHeapId)) != (size_t)(-1));

    // Get a pointer to the data for this heap
    t_RootReferencesData *pData = (t_RootReferencesData *)(*((size_t *)pHeapId));

    // Notify the profiler
    HRESULT hr = RootReferences(pData);

    if (pData)
    {
        // Now we're done with the data block, we can shove it onto the free list
        m_critSecRootRefsFL.Lock();
        pData->pNext = m_pRootRefDataFreeList;
        m_pRootRefDataFreeList = pData;
        m_critSecRootRefsFL.UnLock();
    }

#ifdef _DEBUG
    // Set the cookie to an invalid number
    (*((size_t *)pHeapId)) = (size_t)(-1);
#endif // _DEBUG

    return (hr);
}

HRESULT EEToProfInterfaceImpl::RootReferences(t_RootReferencesData *pData)
{
    LOG((LF_CORPROF, LL_INFO10000, "**PROF: RootReferences.\n"));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        if (pData)
            return (g_pEverettCallback_DELETE_ME->RootReferences((ULONG)pData->curIdx, (ObjectID *)pData->arrRoot));
        else
            return (g_pEverettCallback_DELETE_ME->RootReferences(0, NULL));
    }
    else
    {
        if (pData)
            return (g_pCallback->RootReferences((ULONG)pData->curIdx, (ObjectID *)pData->arrRoot));
        else
            return (g_pCallback->RootReferences(0, NULL));
    }
}

#define HASH_ARRAY_SIZE_INITIAL 1024
#define HASH_ARRAY_SIZE_INC     256
#define HASH_NUM_BUCKETS        32
#define HASH(x)       ( (ULONG) ((SIZE_T)x) )  // A simple hash function
HRESULT EEToProfInterfaceImpl::AllocByClass(ObjectID objId, TypeID typeId, void* pHeapId)
{
    ClassID clsId = TypeIDToClassID(typeId);

#ifdef _DEBUG
    // This is a slight attempt to make sure that this is never called in a multi-threaded
    // manner.  This heap walk should be done by one thread at a time only.
    static DWORD dwProcId = 0xFFFFFFFF;
#endif

    _ASSERTE(pHeapId != NULL);
    _ASSERTE((*((size_t *)pHeapId)) != (size_t)(-1));

    // The heapId they pass in is really a t_AllocByClassData struct ptr.
    t_AllocByClassData *pData = (t_AllocByClassData *)(*((size_t *)pHeapId));

    // If it's null, need to allocate one
    if (pData == NULL)
    {
#ifdef _DEBUG
        // This is a slight attempt to make sure that this is never called in a multi-threaded
        // manner.  This heap walk should be done by one thread at a time only.
        dwProcId = GetCurrentProcessId();
#endif

        // See if we've saved a data block from a previous GC
        if (m_pSavedAllocDataBlock != NULL)
            pData = m_pSavedAllocDataBlock;

        // This means we need to allocate all the memory to keep track of the info
        else
        {
            // Get a new alloc data block
            pData = new (nothrow) t_AllocByClassData;
            if (pData == NULL)
                return (E_OUTOFMEMORY);

            // Create a new hash table
            pData->pHashTable = new (nothrow) CHashTableImpl(HASH_NUM_BUCKETS);
            if (!pData->pHashTable)
            {
                delete pData;
                return (E_OUTOFMEMORY);
            }

            // Get the memory for the array that the hash table is going to use
            pData->arrHash = new (nothrow) CLASSHASHENTRY[HASH_ARRAY_SIZE_INITIAL];
            if (pData->arrHash == NULL)
            {
                delete pData->pHashTable;
                delete pData;
                return (E_OUTOFMEMORY);
            }

            // Save the number of elements in the array
            pData->cHash = HASH_ARRAY_SIZE_INITIAL;

            // Now initialize the hash table
            HRESULT hr = pData->pHashTable->NewInit((BYTE *)pData->arrHash, sizeof(CLASSHASHENTRY));
            if (hr == E_OUTOFMEMORY)
            {
                delete [] pData->arrHash;
                delete pData->pHashTable;
                delete pData;
                return (E_OUTOFMEMORY);
            }
            _ASSERTE(pData->pHashTable->IsInited());

            // Null some entries
            pData->arrClsId = NULL;
            pData->arrcObjects = NULL;
            pData->cLength = 0;

            // Hold on to the structure
            m_pSavedAllocDataBlock = pData;
        }

        // Got some memory and hash table to store entries, yay!
        *((size_t *)pHeapId) = (size_t)pData;

        // Initialize the data
        pData->iHash = 0;
        pData->pHashTable->Clear();
    }

    _ASSERTE(pData->iHash <= pData->cHash);
    _ASSERTE(dwProcId == GetCurrentProcessId());

    // Lookup to see if this class already has an entry
    CLASSHASHENTRY *pEntry = (CLASSHASHENTRY *)pData->pHashTable->Find(HASH(clsId), (BYTE *)clsId);

    // If this class has already been encountered, just increment the counter.
    if (pEntry)
        pEntry->m_count++;

    // Otherwise, need to add this one as a new entry in the hash table
    else
    {
        // If we're full, we need to realloc
        if (pData->iHash == pData->cHash)
        {
            // Try to realloc the memory
            CLASSHASHENTRY     *tmp = new (nothrow) CLASSHASHENTRY[pData->cHash + HASH_ARRAY_SIZE_INC];
            if (!tmp)
            {
                return (E_OUTOFMEMORY);
            }

            _ASSERTE (pData->arrHash);
            memcpy (tmp, pData->arrHash, pData->cHash*sizeof(CLASSHASHENTRY));
            delete [] pData->arrHash;
            pData->arrHash = tmp;
            // Tell the hash table that the memory location of the array has changed
            pData->pHashTable->SetTable((BYTE *)pData->arrHash);

            // Save the new size of the array
            pData->cHash += HASH_ARRAY_SIZE_INC;
        }

        // Now add the new entry
        CLASSHASHENTRY *pNewEntry = (CLASSHASHENTRY *) pData->pHashTable->Add(HASH(clsId), pData->iHash++);

        pNewEntry->m_clsId = clsId;
        pNewEntry->m_count = 1;
    }

    // Indicate success
    return (S_OK);
}

HRESULT EEToProfInterfaceImpl::EndAllocByClass(void *pHeapId)
{
    _ASSERTE(pHeapId != NULL);
    _ASSERTE((*((size_t *)pHeapId)) != (size_t)(-1));

    HRESULT hr = S_OK;

    t_AllocByClassData *pData = (t_AllocByClassData *)(*((size_t *)pHeapId));

    // Notify the profiler if there are elements to notify it of
    if (pData != NULL)
        hr = NotifyAllocByClass(pData);

#ifdef _DEBUG
    (*((size_t *)pHeapId)) = (size_t)(-1);
#endif // _DEBUG

    return (hr);
}


HRESULT EEToProfInterfaceImpl::NotifyAllocByClass(t_AllocByClassData *pData)
{
    _ASSERTE(pData != NULL);
    _ASSERTE(pData->iHash > 0);

    // If the arrays are not long enough, get rid of them.
    if (pData->cLength != 0 && pData->iHash > pData->cLength)
    {
        _ASSERTE(pData->arrClsId != NULL && pData->arrcObjects != NULL);
        delete [] pData->arrClsId;
        delete [] pData->arrcObjects;
        pData->cLength = 0;
    }

    // If there are no arrays, must allocate them.
    if (pData->cLength == 0)
    {
        pData->arrClsId = new (nothrow) ClassID[pData->iHash];
        if (pData->arrClsId == NULL)
            return (E_OUTOFMEMORY);

        pData->arrcObjects = new (nothrow) ULONG[pData->iHash];
        if (pData->arrcObjects == NULL)
        {
            delete [] pData->arrClsId;
            pData->arrClsId= NULL;

            return (E_OUTOFMEMORY);
        }

        // Indicate that the memory was successfully allocated
        pData->cLength = pData->iHash;
    }

    // Now copy all the data
    HASHFIND hFind;
    CLASSHASHENTRY *pCur = (CLASSHASHENTRY *) pData->pHashTable->FindFirstEntry(&hFind);
    size_t iCur = 0;    // current index for arrays

    while (pCur != NULL)
    {
        _ASSERTE(iCur < pData->iHash);

        pData->arrClsId[iCur] = pCur->m_clsId;
        pData->arrcObjects[iCur] = (DWORD) pCur->m_count;

        // Move to the next entry
        iCur++;
        pCur = (CLASSHASHENTRY *) pData->pHashTable->FindNextEntry(&hFind);
    }

    _ASSERTE(iCur == pData->iHash);

    LOG((LF_CORPROF, LL_INFO10000, "**PROF: RootReferences.\n"));

    // Now communicate the results to the profiler

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return (g_pEverettCallback_DELETE_ME->ObjectsAllocatedByClass((ULONG)pData->iHash, pData->arrClsId, pData->arrcObjects));
    }
    else
    {
        return (g_pCallback->ObjectsAllocatedByClass((ULONG)pData->iHash, pData->arrClsId, pData->arrcObjects));
    }
}

HRESULT EEToProfInterfaceImpl::ObjectReference(ObjectID objId,
                                               TypeID typeId,
                                               ULONG cNumRefs,
                                               ObjectID *arrObjRef)
{
    ClassID classId = TypeIDToClassID(typeId);
    // Notify the profiler of the object ref
    LOG((LF_CORPROF, LL_INFO100000, "**PROF: ObjectReferences.\n"));

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback == NULL)
    {
        _ASSERTE(!g_fSupportWhidbeyOnly_DELETE_ME);

        return g_pEverettCallback_DELETE_ME->ObjectReferences(objId, classId, cNumRefs, arrObjRef);
    }
    else
    {
        return g_pCallback->ObjectReferences(objId, classId, cNumRefs, arrObjRef);
    }
}


HRESULT EEToProfInterfaceImpl::FinalizeableObjectQueued(BOOL isCritical, ObjectID objectID)
{
    if (g_pCallback != NULL)
    {
        LOG((LF_CORPROF, LL_INFO100, "**PROF: Notifying profiler of finalizeable object.\n"));

        SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

        return g_pCallback->FinalizeableObjectQueued(isCritical ? COR_PRF_FINALIZER_CRITICAL : 0, objectID);
    }

    return S_OK;
}


HRESULT EEToProfInterfaceImpl::RootReference2(BYTE *objectId, COR_PRF_GC_ROOT_KIND rootKind, COR_PRF_GC_ROOT_FLAGS rootFlags, void *rootID, void *pHeapId)
{
    _ASSERTE(pHeapId);
   _ASSERTE(*((size_t *)pHeapId) != (size_t)(-1));

    LOG((LF_CORPROF, LL_INFO100000, "**PROF: Root Reference. "
            "ObjectID:0x%p rootKind:0x%x rootFlags:0x%x rootId:0x%p HeadId:0x%p\n",
            objectId, rootKind, rootFlags, rootID, pHeapId));

    // Get a pointer to the data for this heap
    t_MovedReferencesData *pData = (t_MovedReferencesData *)(*((size_t *)pHeapId));

    // If this is the first notification of an extended root reference for this heap
    // in this particular gc activation, then we need to get a ref data block
    // from the free list of blocks, or if that's empty then we need to
    // allocate a new one.
    if (pData == NULL)
    {
        pData = AllocateMovedReferencesData();
        if (pData == NULL)
            return (E_OUTOFMEMORY);

        // Set the cookie so that we will be provided it on subsequent
        // callbacks
        ((*((size_t *)pHeapId))) = (size_t)pData;
    }

    _ASSERTE(pData->curIdx >= 0 && pData->curIdx <= MAX_REFERENCES);

    // If the struct has been filled, then we need to notify the profiler of
    // these root references and clear the struct for the next load of
    // root references
    if (pData->curIdx == MAX_REFERENCES)
    {
        RootReferences2(pData);
        pData->curIdx = 0;
    }

    // Now save the information in the struct
    pData->arrpbMemBlockStartOld[pData->curIdx] = objectId;
    pData->arrpbMemBlockStartNew[pData->curIdx] = (BYTE *)rootID;

    // assert that rootKind and rootFlags both fit in 16 bits, so we can
    // pack both into a 32-bit word
    _ASSERTE((rootKind & 0xffff) == rootKind && (rootFlags & 0xffff) == rootFlags);

    pData->arrMemBlockSize[pData->curIdx] = (rootKind << 16) | rootFlags;

    // Increment the index into the parallel arrays
    pData->curIdx += 1;

    return S_OK;
}

HRESULT EEToProfInterfaceImpl::RootReferences2(t_MovedReferencesData *pData)
{
    HRESULT hr = S_OK;

    SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    if (g_pCallback != NULL)
    {
        LOG((LF_CORPROF, LL_INFO10000, "**PROF: RootReferencesEx.\n"));

        COR_PRF_GC_ROOT_FLAGS flags[MAX_REFERENCES];

        _ASSERTE(pData->curIdx <= MAX_REFERENCES);
        for (ULONG i = 0; i < pData->curIdx; i++)
        {
            flags[i] = (COR_PRF_GC_ROOT_FLAGS)(pData->arrMemBlockSize[i] & 0xffff);
            pData->arrMemBlockSize[i] >>= 16;
        }
        hr = g_pCallback->RootReferences2( (ULONG)pData->curIdx,
                                             (ObjectID *)pData->arrpbMemBlockStartOld,
                                             (COR_PRF_GC_ROOT_KIND *)pData->arrMemBlockSize,
                                             flags,
                                             (ObjectID *)pData->arrpbMemBlockStartNew);

        if (SUCCEEDED(hr))
        {
            hr = g_pCallback->RootReferences((ULONG)pData->curIdx, (ObjectID *)pData->arrpbMemBlockStartOld);
        }
    }
    else
    {
        hr = g_pEverettCallback_DELETE_ME->RootReferences((ULONG)pData->curIdx, (ObjectID *)pData->arrpbMemBlockStartOld);
    }
    return hr;
}

HRESULT EEToProfInterfaceImpl::EndRootReferences2(void *pHeapId)
{
    _ASSERTE(pHeapId);
    _ASSERTE((*((size_t *)pHeapId)) != (size_t)(-1));

    HRESULT hr = S_OK;

    // Get a pointer to the data for this heap
    t_MovedReferencesData *pData = (t_MovedReferencesData *)(*((size_t *)pHeapId));

    // If there were no moved references, profiler doesn't need to know
    if (!pData)
        return (S_OK);

    // Communicate the moved references to the profiler
    _ASSERTE(pData->curIdx> 0);
    hr = RootReferences2(pData);

    // Now we're done with the data block, we can shove it onto the free list
    FreeMovedReferencesData(pData);

#ifdef _DEBUG
    // Set the cookie to an invalid number
    (*((size_t *)pHeapId)) = (size_t)(-1);
#endif // _DEBUG

    return (hr);
}

HRESULT EEToProfInterfaceImpl::HandleCreated(UINT_PTR handleId, ObjectID initialObjectId)
{
    if (g_pCallback != NULL)
    {
        LOG((LF_CORPROF, LL_INFO10000, "**PROF: HandleCreated.\n"));

        SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

        return g_pCallback->HandleCreated(handleId, initialObjectId);
    }

    return S_OK;
}

HRESULT EEToProfInterfaceImpl::HandleDestroyed(UINT_PTR handleId)
{
    if (g_pCallback != NULL)
    {
        LOG((LF_CORPROF, LL_INFO10000, "**PROF: HandleDestroyed.\n"));

        SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

        return g_pCallback->HandleDestroyed(handleId);
    }

    return S_OK;
}

HRESULT EEToProfInterfaceImpl::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
    if (g_pCallback != NULL)
    {
        LOG((LF_CORPROF, LL_INFO10000, "**PROF: GarbageCollectionStarted.\n"));

        SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

        return g_pCallback->GarbageCollectionStarted(cGenerations, generationCollected, reason);
    }

    return S_OK;
}

HRESULT EEToProfInterfaceImpl::GarbageCollectionFinished()
{
    if (g_pCallback != NULL)
    {
        LOG((LF_CORPROF, LL_INFO10000, "**PROF: GarbageCollectionFinished.\n"));

        SetCallbackStateFlagsHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

        return g_pCallback->GarbageCollectionFinished();
    }

    return S_OK;
}

// eof
