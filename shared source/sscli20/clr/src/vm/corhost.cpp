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
// CorHost.cpp
//
// Implementation for the meta data dispenser code.
//
//*****************************************************************************
#include "common.h"

#include "mscoree.h"
#include "corhost.h"
#include "excep.h"
#include "threads.h"
#include "jitinterface.h"
#include "comstring.h"
#include "eeconfig.h"
#include "dbginterface.h"
#include "ceemain.h"
#include "rwlock.h"
#include "hosting.h"
#include "eepolicy.h"
#include "clrex.h"
#include "ipcmanagerinterface.h"
#include "comcallablewrapper.h"
#include "hostexecutioncontext.h"
#include "invokeutil.h"
#include "safegetfilesize.h"
#include "appdomain.inl"

#include "win32threadpool.h"



GVAL_IMPL_INIT(DWORD, g_fHostConfig, 0);

GVAL_IMPL_INIT(ECustomDumpFlavor, g_ECustomDumpFlavor, DUMP_FLAVOR_Default);

SVAL_IMPL_INIT(DWORD, CExecutionEngine, TlsIndex, TLS_OUT_OF_INDEXES);

#ifndef DACCESS_COMPILE

extern void STDMETHODCALLTYPE EEShutDown(BOOL fIsDllUnloading);
extern HRESULT STDMETHODCALLTYPE CoInitializeEE(DWORD fFlags);
extern void PrintToStdOutA(const char *pszString);
extern void PrintToStdOutW(const WCHAR *pwzString);

//******************************************************************************
//******************************************************************************

HRESULT STDMETHODCALLTYPE  CorThreadpool::CorRegisterWaitForSingleObject(PHANDLE phNewWaitObject,
                                                                         HANDLE hWaitObject,
                                                                         WAITORTIMERCALLBACK Callback,
                                                                         PVOID Context,
                                                                         ULONG timeout,
                                                                         BOOL  executeOnlyOnce,
                                                                         BOOL* pResult)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;

    BEGIN_ENTRYPOINT_NOTHROW;

    ULONG flag = executeOnlyOnce ? WAIT_SINGLE_EXECUTION : 0;
    *pResult = FALSE;
    EX_TRY
    {
        *pResult = ThreadpoolMgr::RegisterWaitForSingleObject(phNewWaitObject,
                                                  hWaitObject,
                                                  Callback,
                                                  Context,
                                                  timeout,
                                                  flag);

        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;

    return hr;
}


HRESULT STDMETHODCALLTYPE  CorThreadpool::CorBindIoCompletionCallback(HANDLE fileHandle,
                                                                      LPOVERLAPPED_COMPLETION_ROUTINE callback)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    BOOL ret = FALSE;
    DWORD errCode = 0;

    EX_TRY
    {
        ret = ThreadpoolMgr::BindIoCompletionCallback(fileHandle,callback,0, errCode);
        hr = (ret ? S_OK : HRESULT_FROM_WIN32(errCode));
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;

    return hr;
}


HRESULT STDMETHODCALLTYPE  CorThreadpool::CorUnregisterWait(HANDLE hWaitObject,
                                                            HANDLE CompletionEvent,
                                                            BOOL* pResult)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;

    BEGIN_ENTRYPOINT_NOTHROW;

    *pResult = FALSE;
    EX_TRY
    {

        *pResult = ThreadpoolMgr::UnregisterWaitEx(hWaitObject,CompletionEvent);
        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;

    return hr;

}

HRESULT STDMETHODCALLTYPE  CorThreadpool::CorQueueUserWorkItem(LPTHREAD_START_ROUTINE Function,
                                                               PVOID Context,BOOL executeOnlyOnce,
                                                               BOOL* pResult )
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    *pResult = FALSE;
    EX_TRY
    {
        *pResult = ThreadpoolMgr::QueueUserWorkItem(Function,Context,QUEUE_ONLY);
        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

HRESULT STDMETHODCALLTYPE  CorThreadpool::CorCallOrQueueUserWorkItem(LPTHREAD_START_ROUTINE Function,
                                                                     PVOID Context,
                                                                     BOOL* pResult )
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;
    *pResult = FALSE;
    EX_TRY
    {
        *pResult = ThreadpoolMgr::QueueUserWorkItem(Function,Context,CALL_OR_QUEUE);
        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}


HRESULT STDMETHODCALLTYPE  CorThreadpool::CorCreateTimer(PHANDLE phNewTimer,
                                                         WAITORTIMERCALLBACK Callback,
                                                         PVOID Parameter,
                                                         DWORD DueTime,
                                                         DWORD Period,
                                                         BOOL* pResult)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    *pResult = FALSE;
    EX_TRY
    {
        *pResult = ThreadpoolMgr::CreateTimerQueueTimer(phNewTimer,Callback,Parameter,DueTime,Period,0);
        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}


HRESULT STDMETHODCALLTYPE  CorThreadpool::CorDeleteTimer(HANDLE Timer, HANDLE CompletionEvent, BOOL* pResult)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    *pResult = FALSE;
    EX_TRY
    {
        *pResult = ThreadpoolMgr::DeleteTimerQueueTimer(Timer,CompletionEvent);
        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

HRESULT STDMETHODCALLTYPE  CorThreadpool::CorChangeTimer(HANDLE Timer,
                                                         ULONG DueTime,
                                                         ULONG Period,
                                                         BOOL* pResult)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    *pResult = FALSE;
    EX_TRY
    {
        //CONTRACT_VIOLATION(ThrowsViolation);
        *pResult = ThreadpoolMgr::ChangeTimerQueueTimer(Timer,DueTime,Period);
        hr = (*pResult ? S_OK : HRESULT_FROM_GetLastError());
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}


HRESULT STDMETHODCALLTYPE CorThreadpool::CorSetMaxThreads(DWORD MaxWorkerThreads,
                                                          DWORD MaxIOCompletionThreads)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    BOOL result = FALSE;
    EX_TRY
    {
        result = ThreadpoolMgr::SetMaxThreads(MaxWorkerThreads, MaxIOCompletionThreads);
        hr = (result ? S_OK : E_FAIL);
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

HRESULT STDMETHODCALLTYPE CorThreadpool::CorGetMaxThreads(DWORD *MaxWorkerThreads,
                                                          DWORD *MaxIOCompletionThreads)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    BOOL result = FALSE;
    EX_TRY
    {
        result = ThreadpoolMgr::GetMaxThreads(MaxWorkerThreads, MaxIOCompletionThreads);
        hr = (result ? S_OK : E_FAIL);
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

HRESULT STDMETHODCALLTYPE CorThreadpool::CorGetAvailableThreads(DWORD *AvailableWorkerThreads,
                                                                DWORD *AvailableIOCompletionThreads)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    BEGIN_ENTRYPOINT_NOTHROW;

    BOOL result = FALSE;
    EX_TRY
    {
        result = ThreadpoolMgr::GetAvailableThreads(AvailableWorkerThreads, AvailableIOCompletionThreads);
        hr = (result ? S_OK : E_FAIL);
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_ENTRYPOINT_NOTHROW;

    return hr;
}
//***************************************************************************

IGCThreadControl *CorConfiguration::m_CachedGCThreadControl = 0;
IGCHostControl *CorConfiguration::m_CachedGCHostControl = 0;
IDebuggerThreadControl *CorConfiguration::m_CachedDebuggerThreadControl = 0;
DWORD *CorConfiguration::m_DSTArray = 0;
DWORD CorConfiguration::m_DSTCount = 0;
DWORD CorConfiguration::m_DSTArraySize = 0;
ULONG CorRuntimeHostBase::m_Version = 0;

static CCLRDebugManager s_CLRDebugManager;
static CCLRErrorReportingManager s_CLRErrorReportingManager;

// Hash table entry to keep track <connection, name> for SQL fiber support
struct ConnectionNameHashEntry
{

    FREEHASHENTRY   entry;
    CONNID          m_dwConnectionId;
    WCHAR           *m_pwzName;
    ICLRTask        **m_ppCLRTaskArray;
    UINT            m_CLRTaskCount;
};

// Hash table to keep track <connection, name> for SQL fiber support
class ConnectionNameTable : CHashTableAndData<CNewDataNoThrow>
{
    friend class CCLRDebugManager;
public:

    // Key to match is connection ID.
    BOOL Cmp(const BYTE *target, const HASHENTRY *entry)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return (*(CONNID *)target) != ((ConnectionNameHashEntry*)entry)->m_dwConnectionId;
    }

    // Hash function
    ULONG Hash(CONNID dwConnectionId)
    {
        LEAF_CONTRACT;

        return (ULONG)(dwConnectionId);
    }

    // constructor
    ConnectionNameTable(
        ULONG      iBuckets) :
        CHashTableAndData<CNewDataNoThrow>(iBuckets)
    {LEAF_CONTRACT;}

    // destructor
    ~ConnectionNameTable()
    {
        CONTRACTL
        {
            if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
            NOTHROW;
        }
        CONTRACTL_END;
        HASHFIND hashFind;
        ConnectionNameHashEntry *pNameEntry;

        pNameEntry = (ConnectionNameHashEntry *)FindFirstEntry(&hashFind);
        while (pNameEntry != NULL)
        {
            if (pNameEntry->m_pwzName)
            {
                delete pNameEntry->m_pwzName;
                pNameEntry->m_pwzName = NULL;
            }

            if (pNameEntry->m_CLRTaskCount != 0)
            {
                _ASSERTE(pNameEntry->m_ppCLRTaskArray != NULL);
                for (UINT i = 0; i < pNameEntry->m_CLRTaskCount; i++)
                {
                    pNameEntry->m_ppCLRTaskArray[i]->Release();
                }
                delete [] pNameEntry->m_ppCLRTaskArray;
                pNameEntry->m_ppCLRTaskArray = NULL;
                pNameEntry->m_CLRTaskCount = 0;
            }
            pNameEntry = (ConnectionNameHashEntry *)FindNextEntry(&hashFind);
        }
    }

    // Add a new connection into hash table.
    // This function does not throw but return NULL when memory allocation fails.
    ConnectionNameHashEntry *AddConnection(
        CONNID  dwConnectionId,
        __in_z WCHAR   *pwzName)
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            NOTHROW;
        }
        CONTRACTL_END;

        ULONG iHash = Hash(dwConnectionId);

        size_t len = wcslen(pwzName) + 1;
        WCHAR *pConnName = new (nothrow) WCHAR[len];
        if (pConnName == NULL)
            return NULL;

        ConnectionNameHashEntry *pRecord = (ConnectionNameHashEntry *)Add(iHash);
        if (pRecord)
        {
            pRecord->m_dwConnectionId = dwConnectionId;
            pRecord->m_pwzName = pConnName;
            wcsncpy_s(pRecord->m_pwzName, len, pwzName, len);
            pRecord->m_CLRTaskCount = 0;
            pRecord->m_ppCLRTaskArray = NULL;
        }
        else
        {
            if (pConnName)
                delete [] pConnName;
        }

        return pRecord;
    }

    // Delete a hash entry given a connection id
    void DeleteConnection(CONNID dwConnectionId)
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            NOTHROW;
        }
        CONTRACTL_END;

        ULONG  iHash;
        iHash = Hash(dwConnectionId);
        ConnectionNameHashEntry *pRecord = (ConnectionNameHashEntry *)Find(iHash, (BYTE *)&dwConnectionId);
        if (pRecord == NULL)
        {
            return;
        }

        _ASSERTE(pRecord->m_CLRTaskCount == 0 && pRecord->m_ppCLRTaskArray == NULL);
        if (pRecord->m_pwzName)
        {
            delete pRecord->m_pwzName;
            pRecord->m_pwzName = NULL;
        }
        Delete(iHash, (HASHENTRY *)pRecord);
    }

    // return NULL if the given connection id cannot be found.
    ConnectionNameHashEntry *FindConnection(CONNID dwConnectionId)
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            NOTHROW;
        }
        CONTRACTL_END;

        ULONG  iHash;
        iHash = Hash(dwConnectionId);
        return (ConnectionNameHashEntry *) Find(iHash, (BYTE *)&dwConnectionId);
    }

};

// Keep track connection id and name
ConnectionNameTable *CCLRDebugManager::m_pConnectionNameHash = NULL;
CrstStatic CCLRDebugManager::m_lockConnectionNameTable;

//*****************************************************************************
// ICorRuntimeHost
//*****************************************************************************
extern BOOL g_singleVersionHosting;

// *** ICorRuntimeHost methods ***
// Returns an object for configuring the runtime prior to
// it starting. If the runtime has been initialized this
// routine returns an error. See ICorConfiguration.
HRESULT CorHost::GetConfiguration(ICorConfiguration** pConfiguration)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;
    HRESULT hr=E_FAIL;
    BEGIN_ENTRYPOINT_NOTHROW;
    if (CorHost::GetHostVersion() != 1)
    {
        hr=HOST_E_INVALIDOPERATION;
    }
    else
    if (!pConfiguration)
        hr= E_POINTER;
    else
    if (!m_Started)
    {
        *pConfiguration = (ICorConfiguration *) this;
        AddRef();
        hr=S_OK;
    }
    END_ENTRYPOINT_NOTHROW;
    // Cannot obtain configuration after the runtime is started
    return hr;
}

STDMETHODIMP CorHost::Start(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = CorRuntimeHostBase::Start();

    END_ENTRYPOINT_NOTHROW;

    if (hr == S_FALSE)
    {
        // This is to keep v1 behavior.
        hr = S_OK;
    }
    return(hr);
}


// *** ICorRuntimeHost methods ***
// Returns an object for configuring the runtime prior to
// it starting. If the runtime has been initialized this
// routine returns an error. See ICorConfiguration.
HRESULT CorHost2::GetConfiguration(ICorConfiguration** pConfiguration)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if (!pConfiguration)
        return E_POINTER;
    HRESULT hr=E_FAIL;
    BEGIN_ENTRYPOINT_NOTHROW;
    if (!m_Started)
    {
        *pConfiguration = (ICorConfiguration *) this;
        AddRef();
        hr=S_OK;
    }
    END_ENTRYPOINT_NOTHROW;
    // Cannot obtain configuration after the runtime is started
    return hr;
}

extern BOOL g_fWeOwnProcess;

STDMETHODIMP CorHost2::Start()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
    }CONTRACTL_END;


    HRESULT hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    hr = CorRuntimeHostBase::Start();
    if (SUCCEEDED(hr))
    {
        if (FastInterlockIncrement(&m_RefCount) != 1)
        {
        }
        else
        {
            if (g_fWeOwnProcess)
            {
                // Runtime is started by a managed exe.  Bump the ref-count, so that
                // matching Start/Stop does not stop runtime.
                FastInterlockIncrement(&m_RefCount);
            }
        }
    }

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// Starts the runtime. This is equivalent to CoInitializeEE();
HRESULT CorRuntimeHostBase::Start()
{
    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    {
        m_Started = TRUE;
        hr = CoInitializeEE(COINITEE_DEFAULT);
    }
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

HRESULT CorHost::Stop()
{
    CONTRACTL
    {
        NOTHROW;
        ENTRY_POINT;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    // This must remain this way (that is doing nothing) for backwards compat reasons.
    return S_OK;
}

HRESULT CorHost2::Stop()
{
    CONTRACTL
    {
        NOTHROW;
        ENTRY_POINT;    // We're bringing the EE down, so no point in probing
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;
    if (!g_fEEStarted)
    {
        return E_UNEXPECTED;
    }
    HRESULT hr=S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    while (TRUE)
    {
        LONG refCount = m_RefCount;
        if (refCount == 0)
        {
            hr= E_UNEXPECTED;
            break;
        }
        else
        if (FastInterlockCompareExchange(&m_RefCount, refCount - 1, refCount) == refCount)
        {
            if (refCount > 1)
            {
                hr=S_FALSE;
                break;
            }
            else
            {
                break;
            }
        }
    }
    if (hr==S_OK)
    {
        EPolicyAction action = GetEEPolicy()->GetDefaultAction(OPR_ProcessExit, NULL);
        if (action > eExitProcess)
        {
            g_fFastExitProcess = 1;
        }
        EEShutDown(FALSE);
    }
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// Creates a domain in the runtime. The identity array is
// a pointer to an array TYPE containing IIdentity objects defining
// the security identity.
HRESULT CorRuntimeHostBase::CreateDomain(LPCWSTR pwzFriendlyName,
                                         IUnknown* pIdentityArray, // Optional
                                         IUnknown ** pAppDomain)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;

    return CreateDomainEx(pwzFriendlyName,
                          NULL,
                          NULL,
                          pAppDomain);
}


// Returns the default domain.
HRESULT CorRuntimeHostBase::GetDefaultDomain(IUnknown ** pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    } CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    if (!g_fEEStarted)
        return hr;

    if( pAppDomain == NULL)
        return E_POINTER;

    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)

    if (SystemDomain::System()) {
        AppDomain* pCom = SystemDomain::System()->DefaultDomain();
        if(pCom)
            hr = pCom->GetComIPForExposedObject(pAppDomain);
    }

    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

// Returns the default domain.
HRESULT CorRuntimeHostBase::CurrentDomain(IUnknown ** pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    HRESULT hr = E_UNEXPECTED;
    if (!g_fEEStarted)
        return hr;

   if( pAppDomain == NULL) return E_POINTER;

    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)

    AppDomain* pCom = ::GetAppDomain();
    if(pCom)
        hr = pCom->GetComIPForExposedObject(pAppDomain);

    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;

    return hr;
};

HRESULT CorHost2::GetCurrentAppDomainId(DWORD *pdwAppDomainId)
{
    CONTRACTL
    {
        NOTHROW;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if(pdwAppDomainId == NULL)
        return E_POINTER;

    Thread *pThread = GetThread();
    if (!pThread)
        return E_UNEXPECTED;

    BEGIN_ENTRYPOINT_NOTHROW;
    *pdwAppDomainId = SystemDomain::GetCurrentDomain()->GetId().m_dwId;
    END_ENTRYPOINT_NOTHROW;


    return S_OK;
}

HRESULT CorHost2::ExecuteApplication(LPCWSTR   pwzAppFullName,
                                     DWORD     dwManifestPaths,
                                     LPCWSTR   *ppwzManifestPaths,
                                     DWORD     dwActivationData,
                                     LPCWSTR   *ppwzActivationData,
                                     int       *pReturnValue)
{
    // This API should not be called when the EE has already been started.
    HRESULT hr = E_UNEXPECTED;
    if (g_fEEStarted)
        return hr;

    //
    // We will let unhandled exceptions in the activated application
    // propagate all the way up, so that ClickOnce semi-trusted apps
    // can participate in the Dr Watson program, etc...
    //

    CONTRACTL {
        THROWS;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if (!pwzAppFullName)
        IfFailGo(E_POINTER);

    // Set the information about the application to execute.
    CorCommandLine::m_pwszAppFullName = (LPWSTR) pwzAppFullName;
    CorCommandLine::m_dwManifestPaths = dwManifestPaths;
    CorCommandLine::m_ppwszManifestPaths = (LPWSTR*) ppwzManifestPaths;
    CorCommandLine::m_dwActivationData = dwActivationData;
    CorCommandLine::m_ppwszActivationData = (LPWSTR*) ppwzActivationData;

    // Start up the EE.
    IfFailGo(Start());

    Thread *pThread;
    pThread = GetThread();
    if (pThread == NULL)
        pThread = SetupThreadNoThrow(&hr);
    if (pThread == NULL)
        goto ErrExit;

    _ASSERTE (!pThread->PreemptiveGCDisabled());

    hr = S_OK;

    ENTER_DOMAIN_PTR(SystemDomain::System()->DefaultDomain(),ADV_DEFAULTAD)

    SystemDomain::ActivateApplication(pReturnValue);

    END_DOMAIN_TRANSITION;

ErrExit:
    return hr;
}

HRESULT CorHost2::ExecuteInDefaultAppDomain(LPCWSTR pwzAssemblyPath,
                                            LPCWSTR pwzTypeName,
                                            LPCWSTR pwzMethodName,
                                            LPCWSTR pwzArgument,
                                            DWORD   *pReturnValue)
{
    CONTRACTL
    {
        NOTHROW;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(ThrowsViolation); // Work around SCAN bug

    if(! (pwzAssemblyPath && pwzTypeName && pwzMethodName) )
        return E_POINTER;

    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;


    Thread *pThread = GetThread();
    if (pThread == NULL) {
        pThread = SetupThreadNoThrow(&hr);
        if (pThread == NULL)
            goto ErrExit;
    }

    _ASSERTE (!pThread->PreemptiveGCDisabled());

    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    ENTER_DOMAIN_PTR(SystemDomain::System()->DefaultDomain(),ADV_DEFAULTAD)

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    Assembly *pAssembly = AssemblySpec::LoadAssembly(pwzAssemblyPath);

    SString szTypeName(pwzTypeName);
    StackScratchBuffer buff1;
    const char* szTypeNameUTF8 = szTypeName.GetUTF8(buff1);
    MethodTable *pMT = ClassLoader::LoadTypeByNameThrowing(pAssembly,
                                                           NULL,
                                                           szTypeNameUTF8).AsMethodTable();

    SString szMethodName(pwzMethodName);
    StackScratchBuffer buff;
    const char* szMethodNameUTF8 = szMethodName.GetUTF8(buff);
    MethodDesc *pMethodMD = pMT->GetClass()->FindMethod(szMethodNameUTF8, &gsig_SM_Str_RetInt);

    if (!pMethodMD)
    {
        hr = COR_E_MISSINGMETHOD;
    }
    else
    {
        GCX_COOP();

        MethodDescCallSite method(pMethodMD);

        STRINGREF sref = NULL;
        GCPROTECT_BEGIN(sref);

        if (pwzArgument)
            sref = COMString::NewString(pwzArgument);

        ARG_SLOT MethodArgs[] =
        {
            ObjToArgSlot(sref)
        };
        DWORD retval = method.Call_RetI4(MethodArgs);
        if (pReturnValue)
            *pReturnValue = retval;

        GCPROTECT_END();
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    END_DOMAIN_TRANSITION;
    END_EXCEPTION_GLUE;

ErrExit:

    END_ENTRYPOINT_NOTHROW;

    return hr;
}

HRESULT ExecuteInAppDomainHelper(FExecuteInAppDomainCallback pCallback,
                                 void * cookie)
{
    WRAPPER_CONTRACT;

    HRESULT hr = S_OK;

    BEGIN_SO_TOLERANT_CODE(GetThread());
    hr = pCallback(cookie);
    END_SO_TOLERANT_CODE;

    return hr;
}

HRESULT CorHost2::ExecuteInAppDomain(DWORD dwAppDomainId,
                                     FExecuteInAppDomainCallback pCallback,
                                     void * cookie)
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        ENTRY_POINT;  // This is called by a host.
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(ThrowsViolation); // Work around SCAN bug

    if( pCallback == NULL)
        return E_POINTER;

    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr);
    ENTER_DOMAIN_ID(ADID(dwAppDomainId))
    {
        // We are calling an unmanaged function pointer, either an unmanaged function, or a marshaled out delegate.
        // The thread should be in preemptive mode, and SO_Tolerant.
        GCX_PREEMP();
        hr=ExecuteInAppDomainHelper (pCallback, cookie);
    }
    END_DOMAIN_TRANSITION;
    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

// Enumerate currently existing domains.
HRESULT CorRuntimeHostBase::EnumDomains(HDOMAINENUM *hEnum)
{
    if (GetThread() == NULL)
    {
        HRESULT hr;
        Thread *pThread = SetupThreadNoThrow(&hr);
        if (pThread == NULL)
            return hr;
    }

    CONTRACTL
    {
        NOTHROW;
        MODE_PREEMPTIVE;
        WRAPPER(GC_TRIGGERS);
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if(hEnum == NULL) return E_POINTER;

    HRESULT hr = E_OUTOFMEMORY;
    *hEnum = NULL;
    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)

    AppDomainIterator *pEnum = new (nothrow) AppDomainIterator(FALSE);
    if(pEnum) {
        *hEnum = (HDOMAINENUM) pEnum;
        hr = S_OK;
    }
    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

HRESULT NextDomainWorker(AppDomainIterator *pEnum,
                         IUnknown** pAppDomain)
{
    CONTRACTL
    {
        DISABLED(NOTHROW); // nothrow contract's fs:0 handler gets called before the C++ EH fs:0 handler which is pushed in the prolog
        GC_TRIGGERS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    Thread *pThread = GetThread();
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, return COR_E_STACKOVERFLOW);

    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {
        GCX_COOP_THREAD_EXISTS(pThread);

        if (pEnum->Next())
        {
            AppDomain* pDomain = pEnum->GetDomain();
            // Need to enter the AppDomain to synchronize access to the exposed
            // object properly (can't just take the system domain mutex since we
            // might need to run code that uses higher ranking crsts).
            ENTER_DOMAIN_PTR(pDomain,ADV_ITERATOR)
            {

                hr = pDomain->GetComIPForExposedObject(pAppDomain);
            }
            END_DOMAIN_TRANSITION;
        }
        else
        {
            hr = S_FALSE;
        }
    }
    END_EXCEPTION_GLUE

    END_SO_INTOLERANT_CODE;

    return hr;
}

// Returns S_FALSE when there are no more domains. A domain
// is passed out only when S_OK is returned.
HRESULT CorRuntimeHostBase::NextDomain(HDOMAINENUM hEnum,
                                       IUnknown** pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
    }
    CONTRACTL_END;
    if(hEnum == NULL || pAppDomain == NULL)
        return E_POINTER;
    HRESULT hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    AppDomainIterator *pEnum = (AppDomainIterator *) hEnum;

    do
    {
        hr = NextDomainWorker(pEnum, pAppDomain);
    // Might need to look at the next appdomain if we were attempting to get at
    // the exposed appdomain object and were chucked out as the result of an
    // appdomain unload.
    } while (hr == COR_E_APPDOMAINUNLOADED);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// Creates a domain in the runtime. The identity array is
// a pointer to an array TYPE containing IIdentity objects defining
// the security identity.
HRESULT CorRuntimeHostBase::CreateDomainEx(LPCWSTR pwzFriendlyName,
                                           IUnknown* pSetup, // Optional
                                           IUnknown* pEvidence, // Optional
                                           IUnknown ** pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    if(!pwzFriendlyName) return E_POINTER;
    if(pAppDomain == NULL) return E_POINTER;
    if(!g_fEEStarted) return E_FAIL;

    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)

        struct _gc {
            STRINGREF pName;
            OBJECTREF pSetup;
            OBJECTREF pEvidence;
            APPDOMAINREF pDomain;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        if (FAILED(hr = EnsureComStartedNoThrow()))
            goto lDone;

        GCPROTECT_BEGIN(gc);

        gc.pName = COMString::NewString(pwzFriendlyName);

        if(pSetup)
            gc.pSetup = GetObjectRefFromComIP(pSetup);
        if(pEvidence)
            gc.pEvidence = GetObjectRefFromComIP(pEvidence);

        MethodDescCallSite createDomain(METHOD__APP_DOMAIN__CREATE_DOMAIN);

        ARG_SLOT args[3] = {
            ObjToArgSlot(gc.pName),
            ObjToArgSlot(gc.pEvidence),
            ObjToArgSlot(gc.pSetup),
        };

        gc.pDomain = (APPDOMAINREF) createDomain.Call_RetOBJECTREF(args);

        *pAppDomain = GetComIPFromObjectRef((OBJECTREF*) &gc.pDomain);

        GCPROTECT_END();

lDone: ;
    END_EXTERNAL_ENTRYPOINT;

    END_ENTRYPOINT_NOTHROW;

    return hr;
}

// Close the enumeration releasing resources
HRESULT CorRuntimeHostBase::CloseEnum(HDOMAINENUM hEnum)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    if(hEnum) {
        AppDomainIterator* pEnum = (AppDomainIterator*) hEnum;
        delete pEnum;
    }

    END_ENTRYPOINT_NOTHROW;
    return hr;
}


HRESULT CorRuntimeHostBase::CreateDomainSetup(IUnknown **pAppDomainSetup)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    if (!pAppDomainSetup)
        return E_POINTER;

    // Create the domain.
    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)
        struct _gc {
            OBJECTREF pSetup;
        } gc;
        ZeroMemory(&gc, sizeof(gc));
        MethodTable* pMT = NULL;

        hr = EnsureComStartedNoThrow();
        if (FAILED(hr))
            goto lDone;

        pMT = g_Mscorlib.GetClass(CLASS__APPDOMAIN_SETUP);

        GCPROTECT_BEGIN(gc);
        gc.pSetup = AllocateObject(pMT);
        *pAppDomainSetup = GetComIPFromObjectRef((OBJECTREF*) &gc.pSetup);
        GCPROTECT_END();

lDone: ;
    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

HRESULT CorRuntimeHostBase::CreateEvidence(IUnknown **pEvidence)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    if (!pEvidence)
        return E_POINTER;

    // Create the domain.
    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)
        struct _gc {
            OBJECTREF pEvidence;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        MethodTable* pMT = NULL;

        hr = EnsureComStartedNoThrow();
        if (FAILED(hr))
            goto lDone;

        pMT = g_Mscorlib.GetClass(CLASS__EVIDENCE);

        GCPROTECT_BEGIN(gc);
        gc.pEvidence = AllocateObject(pMT);
        *pEvidence = GetComIPFromObjectRef((OBJECTREF*) &gc.pEvidence);
        GCPROTECT_END();

lDone: ;
    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

HRESULT CorRuntimeHostBase::UnloadDomain(IUnknown *pUnkDomain)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        FORBID_FAULT;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if (!pUnkDomain)
        return E_POINTER;

    CONTRACT_VIOLATION(FaultViolation); // This entire function is full of OOM potential: must fix.

    HRESULT hr = S_OK;
    DWORD dwDomainId = 0;
    BEGIN_ENTRYPOINT_NOTHROW;


    {
        SystemDomain::LockHolder lh;

        ComCallWrapper* pWrap = GetCCWFromIUnknown(pUnkDomain);
        if (!pWrap)
        {
            hr = COR_E_APPDOMAINUNLOADED;
        }
        if (SUCCEEDED(hr))
        {
            dwDomainId = pWrap->GetDomainID().m_dwId;
        }
    }
    if (SUCCEEDED(hr))
    {
        hr = UnloadAppDomain(dwDomainId, TRUE);
    }
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

HRESULT CorRuntimeHostBase::UnloadAppDomain(DWORD dwDomainId, BOOL fSync)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        FORBID_FAULT; // Unloading domains cannot fail due to OOM
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    // We do not use BEGIN_EXTERNAL_ENTRYPOINT here because
    // we do not want to setup Thread.  Process may be OOM, and we want Unload
    // to work.
    hr =  AppDomain::UnloadById(ADID(dwDomainId), fSync);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

//*****************************************************************************
// Fiber Methods
//*****************************************************************************

HRESULT CorHost::CreateLogicalThreadState()
{
    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);
        ENTRY_POINT;
    }
    CONTRACTL_END;
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;
    if (CorHost::GetHostVersion() != 1)
    {
        hr=HOST_E_INVALIDOPERATION;
    }
    else
    {

    /* Thread  *thread = */ SetupThreadNoThrow(&hr);
    }
    END_ENTRYPOINT_NOTHROW;
    return hr;
}


HRESULT CorHost::DeleteLogicalThreadState()
{
    if (CorHost::GetHostVersion() != 1)
    {
        return HOST_E_INVALIDOPERATION;
    }

    Thread *pThread = GetThread();
    if (!pThread)
        return E_UNEXPECTED;

    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
    }
    CONTRACTL_END;
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    // We need to reset the TrapReturningThread count that was
    // set when a thread is requested to be aborted.  Otherwise
    // every stub call is going to go through a slow path.
    if (pThread->IsAbortRequested())
        pThread->UnmarkThreadForAbort(Thread::TAR_ALL);

    pThread->OnThreadTerminate(FALSE);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}


HRESULT CorHost::SwitchInLogicalThreadState(DWORD *pFiberCookie)
{
    if (CorHost::GetHostVersion() != 1)
    {
        return HOST_E_INVALIDOPERATION;
    }

    if (!pFiberCookie)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = ((Thread*)pFiberCookie)->SwitchIn(::GetCurrentThread());

    END_ENTRYPOINT_NOTHROW;
    return hr;

}

HRESULT CorHost::SwitchOutLogicalThreadState(DWORD **pFiberCookie)
{
    if (CorHost::GetHostVersion() != 1)
    {
        return HOST_E_INVALIDOPERATION;
    }

    // If the user of this fiber wants to switch it out then we better be in
    // preemptive mode,
     if (!pFiberCookie)
        return E_POINTER;

     Thread *pThread = GetThread();
     if (!pThread)
         return E_UNEXPECTED;

     CONTRACTL
     {
         NOTHROW;
         GC_NOTRIGGER;
        ENTRY_POINT;
     }
     CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = pThread->SwitchOut();
     if (hr == S_OK)
         *pFiberCookie = (DWORD*)pThread;

    END_ENTRYPOINT_NOTHROW;

     return hr;
}

HRESULT CorRuntimeHostBase::LocksHeldByLogicalThread(DWORD *pCount)
{
    if (!pCount)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_NOTHROW;

    Thread* pThread = GetThread();
    if (pThread == NULL)
        *pCount = 0;
    else
        *pCount = pThread->m_dwLockCount;

    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}

//*****************************************************************************
// ICorConfiguration
//*****************************************************************************

// *** ICorConfiguration methods ***


HRESULT CorConfiguration::SetGCThreadControl(IGCThreadControl *pGCThreadControl)
{
    if (!pGCThreadControl)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_NOTHROW;

    if (m_CachedGCThreadControl)
        m_CachedGCThreadControl->Release();

    m_CachedGCThreadControl = pGCThreadControl;

    if (m_CachedGCThreadControl)
        m_CachedGCThreadControl->AddRef();

    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}

HRESULT CorConfiguration::SetGCHostControl(IGCHostControl *pGCHostControl)
{
    if (!pGCHostControl)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_NOTHROW;

    if (m_CachedGCHostControl)
        m_CachedGCHostControl->Release();

    m_CachedGCHostControl = pGCHostControl;

    if (m_CachedGCHostControl)
        m_CachedGCHostControl->AddRef();

    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}

HRESULT CorConfiguration::SetDebuggerThreadControl(IDebuggerThreadControl *pDebuggerThreadControl)
{
    if (!pDebuggerThreadControl)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

#ifdef DEBUGGING_SUPPORTED
    // Can't change the debugger thread control object once its been set.
    if (m_CachedDebuggerThreadControl != NULL)
        IfFailGo(E_INVALIDARG);

    m_CachedDebuggerThreadControl = pDebuggerThreadControl;

    // If debugging is already initialized then provide this interface pointer to it.
    // It will also addref the new one and release the old one.
    if (g_pDebugInterface)
        g_pDebugInterface->SetIDbgThreadControl(pDebuggerThreadControl);

    if (m_CachedDebuggerThreadControl)
        m_CachedDebuggerThreadControl->AddRef();

    hr = S_OK;
#else // !DEBUGGING_SUPPORTED
    hr = E_NOTIMPL;
#endif // !DEBUGGING_SUPPORTED

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;

}


HRESULT CorConfiguration::AddDebuggerSpecialThread(DWORD dwSpecialThreadId)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;    // debugging not hardened for SO
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;


#ifdef DEBUGGING_SUPPORTED
    // If it's already in the list, don't add it again.
    if (IsDebuggerSpecialThread(dwSpecialThreadId))
    {
        hr = S_OK;
        goto ErrExit;
    }
    // Grow the array if necessary.
    if (m_DSTCount >= m_DSTArraySize)
    {
        // There's probably only ever gonna be one or two of these
        // things, so we'll start small.
        DWORD newSize = (m_DSTArraySize == 0) ? 2 : m_DSTArraySize * 2;

        DWORD *newArray = new (nothrow) DWORD[newSize];
        IfNullGo(newArray);

        // If we're growing instead of starting, then copy the old array.
        if (m_DSTArray)
        {
            memcpy(newArray, m_DSTArray, m_DSTArraySize * sizeof(DWORD));
            delete [] m_DSTArray;
        }

        // Update to the new array and size.
        m_DSTArray = newArray;
        m_DSTArraySize = newSize;
    }

    // Save the new thread ID.
    m_DSTArray[m_DSTCount++] = dwSpecialThreadId;

    hr = (RefreshDebuggerSpecialThreadList());
#else // !DEBUGGING_SUPPORTED
    hr = E_NOTIMPL;
#endif // !DEBUGGING_SUPPORTED
ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;

}
// Helper function to update the thread list in the debugger control block
HRESULT CorConfiguration::RefreshDebuggerSpecialThreadList()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef DEBUGGING_SUPPORTED
    HRESULT hr = S_OK;

    if (g_pDebugInterface)
    {
        // Inform the debugger services that this list has changed
        hr = g_pDebugInterface->UpdateSpecialThreadList(
            m_DSTCount, m_DSTArray);

        _ASSERTE(SUCCEEDED(hr));
    }

    return (hr);
#else // !DEBUGGING_SUPPORTED
    return E_NOTIMPL;
#endif // !DEBUGGING_SUPPORTED
}


// Helper func that returns true if the thread is in the debugger special thread list
BOOL CorConfiguration::IsDebuggerSpecialThread(DWORD dwThreadId)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    for (DWORD i = 0; i < m_DSTCount; i++)
    {
        if (m_DSTArray[i] == dwThreadId)
            return (TRUE);
    }

    return (FALSE);
}


// Clean up any debugger thread control object we may be holding, called at shutdown.
void CorConfiguration::CleanupDebuggerThreadControl()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (m_CachedDebuggerThreadControl != NULL)
    {
        // Note: we don't release the IDebuggerThreadControl object if we're cleaning up from
        // our DllMain. The DLL that implements the object may already have been unloaded.
        // Leaking the object is okay... the PDM doesn't care.
        if (!g_fProcessDetach)
            m_CachedDebuggerThreadControl->Release();

        m_CachedDebuggerThreadControl = NULL;
    }
}

//*****************************************************************************
// IUnknown
//*****************************************************************************

ULONG CorRuntimeHostBase::AddRef()
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        SO_TOLERANT;
    }
    CONTRACTL_END;
    return InterlockedIncrement(&m_cRef);
}

ULONG CorHost::Release()
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    ULONG   cRef = InterlockedDecrement(&m_cRef);
    if (!cRef) {
        delete this;
    }

    return (cRef);
}

ULONG CorHost2::Release()
{
    LEAF_CONTRACT;

    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (!cRef) {

        // CorHost2 is allocated before host memory interface is set up.
        if (m_HostMemoryManager == NULL)
            delete this;
    }

    return (cRef);
}

HRESULT CorHost::QueryInterface(REFIID riid, void **ppUnk)
{
    if (!ppUnk)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;    // no global state updates that need guarding.
    }
    CONTRACTL_END;

    if (ppUnk == NULL)
    {
        return E_POINTER;
    }

    *ppUnk = 0;

    // Deliberately do NOT hand out ICorConfiguration.  They must explicitly call
    // GetConfiguration to obtain that interface.
    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *) (ICorRuntimeHost *) this;
    else if (riid == IID_ICorRuntimeHost)
    {
        ULONG version = 1;
        if (m_Version == 0)
            FastInterlockCompareExchange((LONG*)&m_Version, version, 0);

        if (m_Version != version && (g_singleVersionHosting || !g_fEEStarted))
        {
            return HOST_E_INVALIDOPERATION;
        }

        *ppUnk = (ICorRuntimeHost *) this;
    }
    else if (riid == IID_ICorThreadpool)
        *ppUnk = (ICorThreadpool *) this;
    else if (riid == IID_IGCHost)
        *ppUnk = (IGCHost *) this;
    else if (riid == IID_IValidator)
        *ppUnk = (IValidator *) this;
    else if (riid == IID_IDebuggerInfo)
        *ppUnk = (IDebuggerInfo *) this;
    else
        return (E_NOINTERFACE);
    AddRef();
    return (S_OK);
}


HRESULT CorHost2::QueryInterface(REFIID riid, void **ppUnk)
{
    if (!ppUnk)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;    // no global state updates that need guarding.
    }
    CONTRACTL_END;

    if (ppUnk == NULL)
    {
        return E_POINTER;
    }

    *ppUnk = 0;

    // Deliberately do NOT hand out ICorConfiguration.  They must explicitly call
    // GetConfiguration to obtain that interface.
    if (riid == IID_IUnknown)
        *ppUnk = (IUnknown *) (ICLRRuntimeHost *) this;
    else if (riid == IID_ICLRRuntimeHost)
    {
        ULONG version = 2;
        if (m_Version == 0)
            FastInterlockCompareExchange((LONG*)&m_Version, version, 0);

        *ppUnk = (ICLRRuntimeHost *) this;
    }
    else if (riid == IID_ICorThreadpool)
        *ppUnk = (ICorThreadpool *) this;
    else if (riid == IID_IGCHost &&
             GetHostVersion() == 3)
        *ppUnk = (IGCHost *) this;
    else if (riid == IID_ICLRValidator)
        *ppUnk = (ICLRValidator *) this;
    else if (riid == IID_IDebuggerInfo)
        *ppUnk = (IDebuggerInfo *) this;
    else
        return (E_NOINTERFACE);
    AddRef();
    return (S_OK);
}

//*****************************************************************************
// Called by the class factory template to create a new instance of this object.
//*****************************************************************************
HRESULT CorHost::CreateObject(REFIID riid, void **ppUnk)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    CorHost *pCorHost = new (nothrow) CorHost();
    if (!pCorHost)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        hr = pCorHost->QueryInterface(riid, ppUnk);

    if (FAILED(hr))
        delete pCorHost;
    }
    return (hr);
}


HRESULT CorHost2::CreateObject(REFIID riid, void **ppUnk)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW; );
    CorHost2 *pCorHost = new (nothrow) CorHost2();
    if (!pCorHost)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        hr = pCorHost->QueryInterface(riid, ppUnk);
    if (FAILED(hr))
        delete pCorHost;
    else
        g_fHostConfig |= CLRHOSTED;
    }
    END_SO_INTOLERANT_CODE;
    return (hr);
}


//-----------------------------------------------------------------------------
// MapFile - Maps a file into the runtime in a non-standard way
//-----------------------------------------------------------------------------

static PEImage *MapFileHelper(HANDLE hFile)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    HandleHolder hFileMap(WszCreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL));
    if (hFileMap == NULL)
        ThrowLastError();

    CLRMapViewHolder base(CLRMapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0));
    if (base == NULL)
        ThrowLastError();

    DWORD dwSize = SafeGetFileSize(hFile, NULL);
    if (dwSize == 0xffffffff && GetLastError() != NOERROR)
    {
        ThrowLastError();
    }
    PEImageHolder pImage(PEImage::LoadFlat(base, dwSize));
    return pImage.Extract();
}

HRESULT CorRuntimeHostBase::MapFile(HANDLE hFile, HMODULE* phHandle)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr;
    BEGIN_ENTRYPOINT_NOTHROW;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)
    {
        *phHandle = (HMODULE) (MapFileHelper(hFile)->GetLoadedLayout()->GetBase());
    }
    END_EXTERNAL_ENTRYPOINT;
    END_ENTRYPOINT_NOTHROW;


    return hr;
}

///////////////////////////////////////////////////////////////////////////////
// IDebuggerInfo::IsDebuggerAttached
HRESULT CorDebuggerInfo::IsDebuggerAttached(BOOL *pbAttached)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    if (pbAttached == NULL)
        hr = E_INVALIDARG;
    else
#ifdef DEBUGGING_SUPPORTED
    *pbAttached = (CORDebuggerAttached() != 0);
#else
    *pbAttached = FALSE;
#endif

    END_ENTRYPOINT_NOTHROW;

    return hr;
}

LONG CorHost2::m_RefCount = 0;
IHostControl *CorHost2::m_HostControl = NULL;
IHostMemoryManager *CorHost2::m_HostMemoryManager = NULL;
IHostMalloc *CorHost2::m_HostMalloc = NULL;
IHostTaskManager *CorHost2::m_HostTaskManager = NULL;
IHostThreadpoolManager *CorHost2::m_HostThreadpoolManager = NULL;
IHostIoCompletionManager *CorHost2::m_HostIoCompletionManager = NULL;
IHostSyncManager *CorHost2::m_HostSyncManager = NULL;
IHostAssemblyManager *CorHost2::m_HostAssemblyManager = NULL;
IHostGCManager *CorHost2::m_HostGCManager = NULL;
IHostSecurityManager *CorHost2::m_HostSecurityManager = NULL;
IHostPolicyManager *CorHost2::m_HostPolicyManager = NULL;
int CorHost2::m_HostOverlappedExtensionSize = -1;

LONG g_bLowMemoryFromHost = 0;

class CCLRMemoryNotificationCallback: public ICLRMemoryNotificationCallback
{
public:
    virtual HRESULT STDMETHODCALLTYPE OnMemoryNotification(EMemoryAvailable eMemoryAvailable) {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        // We have not started runtime yet.
        if (!g_fEEStarted)
            return S_OK;

        BEGIN_ENTRYPOINT_NOTHROW;

        switch (eMemoryAvailable)
        {
        case eMemoryAvailableLow:
            STRESS_LOG0(LF_GC, LL_INFO100, "Host delivers memory notification: Low\n");
            break;
        case eMemoryAvailableNeutral:
            STRESS_LOG0(LF_GC, LL_INFO100, "Host delivers memory notification: Neutral\n");
            break;
        case eMemoryAvailableHigh:
            STRESS_LOG0(LF_GC, LL_INFO100, "Host delivers memory notification: High\n");
            break;
        }
        static DWORD lastTime = (DWORD)-1;
        if (eMemoryAvailable == eMemoryAvailableLow)
        {
            FastInterlockIncrement (&g_bLowMemoryFromHost);
            DWORD curTime = GetTickCount();
            if (curTime < lastTime || curTime - lastTime >= 0x2000)
            {
                lastTime = curTime;
                GCHeap::GetGCHeap()->EnableFinalization();
            }
        }
        else
        {
            FastInterlockExchange (&g_bLowMemoryFromHost, FALSE);
        }
        END_ENTRYPOINT_NOTHROW;

        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
    {
        LEAF_CONTRACT;
        if (riid != IID_ICLRMemoryNotificationCallback && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppvObject = this;
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LEAF_CONTRACT;
        return 1;
    }
};

static CCLRMemoryNotificationCallback s_MemoryNotification;

class CLRTaskManager : public ICLRTaskManager
{
public:
    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {
        LEAF_CONTRACT;
        if (riid != IID_ICLRTaskManager && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppvObject = this;
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateTask(ICLRTask **pTask)
    {
        CONTRACTL
        {
            NOTHROW;
            DISABLED(GC_NOTRIGGER);
            ENTRY_POINT;
        }
        CONTRACTL_END;

        HRESULT hr = S_OK;
        BEGIN_ENTRYPOINT_NOTHROW;

#ifdef _DEBUG
        _ASSERTE (!CLRTaskHosted() || GetCurrentHostTask());
#endif
        _ASSERTE (GetThread() == NULL);
        Thread *pThread = NULL;
        pThread = SetupThreadNoThrow(&hr);
        *pTask = pThread;

        END_ENTRYPOINT_NOTHROW;

        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE GetCurrentTask(ICLRTask **pTask)
    {
        // This function may be called due SQL SwitchIn/Out.  Contract may
        // force memory allocation which is not allowed during Switch.
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;
        STATIC_CONTRACT_SO_TOLERANT;
        STATIC_CONTRACT_ENTRY_POINT;

        *pTask = GetThread();
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE SetUILocale(LCID lcid)
    {
        Thread *pThread = GetThread();
        if (pThread == NULL)
            return HOST_E_INVALIDOPERATION;

        CONTRACTL
        {
            GC_TRIGGERS;
            NOTHROW;
            MODE_PREEMPTIVE;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        HRESULT hr = S_OK;
        //BEGIN_ENTRYPOINT_NOTHROW;
        BEGIN_EXTERNAL_ENTRYPOINT(&hr)
        {
            pThread->SetCultureId(lcid,TRUE);
        }
        END_EXTERNAL_ENTRYPOINT;
        //END_ENTRYPOINT_NOTHROW;

        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE SetLocale(LCID lcid)
    {
        Thread *pThread = GetThread();
        if (pThread == NULL)
            return HOST_E_INVALIDOPERATION;

        CONTRACTL
        {
            GC_TRIGGERS;
            NOTHROW;
            MODE_PREEMPTIVE;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        HRESULT hr = S_OK;
        //BEGIN_ENTRYPOINT_NOTHROW;

        BEGIN_EXTERNAL_ENTRYPOINT(&hr)
        {
            pThread->SetCultureId(lcid,FALSE);
        }
        END_EXTERNAL_ENTRYPOINT;
        //END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE GetCurrentTaskType(ETaskType *pTaskType)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_ENTRY_POINT;
        BEGIN_ENTRYPOINT_NOTHROW;
        *pTaskType = ::GetCurrentTaskType();
        END_ENTRYPOINT_NOTHROW;

        return S_OK;
    }
};

static CLRTaskManager s_CLRTaskManager;

class CLRSyncManager : public ICLRSyncManager
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetMonitorOwner(SIZE_T Cookie,
                                                      IHostTask **ppOwnerHostTask)
    {
        CONTRACTL
        {
            NOTHROW;
            MODE_PREEMPTIVE;
            GC_NOTRIGGER;
            ENTRY_POINT;;
        }
        CONTRACTL_END;

        BEGIN_ENTRYPOINT_NOTHROW;

        // Cookie is the SyncBlock
        AwareLock* pAwareLock = (AwareLock*)Cookie;
        IHostTask *pTask = NULL;
        Thread *pThread = pAwareLock->GetOwningThread();
        if (pThread)
        {
            ThreadStoreLockHolder tsLock(TRUE);
            pThread = pAwareLock->GetOwningThread();
            if (pThread)
            {
                // See if the lock is orphaned, and the Thread object has been deleted
                Thread *pWalk = NULL;
                while ((pWalk = ThreadStore::GetAllThreadList(pWalk, 0, 0)) != NULL)
                {
                    if (pWalk == pThread)
                    {
                        pTask = pThread->GetHostTaskWithAddRef();
                        break;
                    }
                }
            }
        }

        *ppOwnerHostTask = pTask;

        END_ENTRYPOINT_NOTHROW;


        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE CreateRWLockOwnerIterator(SIZE_T Cookie,
                                                                SIZE_T *pIterator) {
        Thread *pThread = GetThread();

        // We may open a window for GC here.
        // A host should not hijack a coop thread to do deadlock detection.
        if (pThread && pThread->PreemptiveGCDisabled())
            return HOST_E_INVALIDOPERATION;

        CONTRACTL
        {
            NOTHROW;
            MODE_PREEMPTIVE;
            GC_NOTRIGGER;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        HRESULT hr = E_FAIL;

        BEGIN_ENTRYPOINT_NOTHROW;
        ThreadStoreLockHolder tsLock(TRUE);
        // Cookie is a weak handle.  We need to make sure that the object is not moving.
        CRWLock *pRWLock = *(CRWLock **) Cookie;
        *pIterator = NULL;
        if (pRWLock == NULL)
        {
            hr = S_OK;
        }
        else
        {
            hr = pRWLock->CreateOwnerIterator(pIterator);
        }
        END_ENTRYPOINT_NOTHROW;

        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE GetRWLockOwnerNext(SIZE_T Iterator,
                                                         IHostTask **ppOwnerHostTask)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            ENTRY_POINT;
        }
        CONTRACTL_END;
        BEGIN_ENTRYPOINT_NOTHROW;
        CRWLock::GetNextOwner(Iterator,ppOwnerHostTask);
        END_ENTRYPOINT_NOTHROW;

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DeleteRWLockOwnerIterator(SIZE_T Iterator)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            ENTRY_POINT;
        }
        CONTRACTL_END;
        BEGIN_ENTRYPOINT_NOTHROW;
        CRWLock::DeleteOwnerIterator(Iterator);
        END_ENTRYPOINT_NOTHROW;

        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
    {
        LEAF_CONTRACT;
        if (riid != IID_ICLRSyncManager && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppvObject = this;
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LEAF_CONTRACT;
        return 1;
    }
};

static CLRSyncManager s_CLRSyncManager;

extern void HostIOCompletionCallback(DWORD ErrorCode,
                                     DWORD numBytesTransferred,
                                     LPOVERLAPPED lpOverlapped);
class CCLRIoCompletionManager :public ICLRIoCompletionManager
{
public:
    virtual HRESULT STDMETHODCALLTYPE OnComplete(DWORD dwErrorCode,
                                                 DWORD NumberOfBytesTransferred,
                                                 void* pvOverlapped)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_ENTRY_POINT;
        _ASSERTE (!"BindIoCompletionCallbackStub not defined for FEATURE_PAL");
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return 1;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return 1;
    }
    BEGIN_INTERFACE HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        if (riid != IID_ICLRIoCompletionManager && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppvObject = this;
        return S_OK;
    }
};

static CCLRIoCompletionManager s_CLRIoCompletionManager;

#ifdef _DEBUG
extern void ValidateHostInterface();
#endif

// fusion's global copy of host assembly manager stuff
BOOL g_bFusionHosted = FALSE;
ICLRAssemblyReferenceList *g_pHostAsmList = NULL;
IHostAssemblyStore *g_pHostAssemblyStore = NULL;

/*static*/ BOOL CorHost2::IsLoadFromBlocked() // LoadFrom, LoadFile and Load(byte[]) are blocked in certain hosting scenarios
{
    LEAF_CONTRACT;
    return (g_bFusionHosted && (g_pHostAsmList != NULL));
}

///////////////////////////////////////////////////////////////////////////////
// ICLRRuntimeHost::SetHostControl
///////////////////////////////////////////////////////////////////////////////
HRESULT CorHost2::SetHostControl(IHostControl* pHostControl)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if (m_Version < 2)
        // CLR is hosted with v1 hosting interface.  Some part of v2 hosting API are disabled.
        return HOST_E_INVALIDOPERATION;

    if (pHostControl == 0)
        return E_INVALIDARG;

    // If Runtime has been started, do not allow setting HostMemoryManager
    if (g_fEEStarted)
        return E_ACCESSDENIED;

    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    static BOOL fOneOnly = 0;

    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
    while (FastInterlockExchange((LONG*)&fOneOnly, 1) == 1)
    {
        if (m_HostTaskManager != NULL)
        {
            m_HostTaskManager->SwitchToTask(0);
        }
        else
        {
            IHostTaskManager *pHostTaskManager = NULL;
            if (pHostControl->GetHostManager(IID_IHostTaskManager, (void**)&pHostTaskManager) == S_OK &&
                pHostTaskManager != NULL)
            {
                pHostTaskManager->SwitchToTask(0);
                pHostTaskManager->Release();
            }
            else
            {
                __SwitchToThread(0);
            }
        }
    }

#ifdef _DEBUG
    ValidateHostInterface();
#endif

#ifdef _DEBUG
    DWORD dbg_HostManagerConfig = EEConfig::GetConfigDWORD(L"HostManagerConfig",(DWORD) -1);
#endif

    IHostMemoryManager *memoryManager = NULL;
    IHostTaskManager *taskManager = NULL;
    IHostThreadpoolManager *threadpoolManager = NULL;
    IHostIoCompletionManager *ioCompletionManager = NULL;
    IHostSyncManager *syncManager = NULL;
    IHostAssemblyManager *assemblyManager = NULL;
    IHostGCManager *gcManager = NULL;
    IHostSecurityManager *securityManager = NULL;
    IHostPolicyManager *policyManager = NULL;

    if (m_HostMemoryManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRMEMORYHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostMemoryManager,(void**)&memoryManager) == S_OK &&
        memoryManager != NULL) {
        if (m_HostMalloc == NULL)
        {
            hr = memoryManager->CreateMalloc (MALLOC_THREADSAFE, &m_HostMalloc);
            if (hr == S_OK)
            {
                memoryManager->RegisterMemoryNotificationCallback(&s_MemoryNotification);
            }
            else
            {
                memoryManager->Release();
                IfFailGo(E_UNEXPECTED);
            }
        }
        m_HostMemoryManager = memoryManager;
        g_fHostConfig |= CLRMEMORYHOSTED;
    }

    if (m_HostMemoryManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRMEMORYHOSTED) &&
#endif
        memoryManager != NULL) {
        if (m_HostMalloc == NULL)
        {
            hr = m_HostMemoryManager->CreateMalloc (TRUE, &m_HostMalloc);
            if (hr == S_OK)
                m_HostMemoryManager->RegisterMemoryNotificationCallback(&s_MemoryNotification);
            else
            {
                memoryManager->Release();
                IfFailGo(E_UNEXPECTED);
            }
        }
        m_HostMemoryManager = memoryManager;
        g_fHostConfig |= CLRMEMORYHOSTED;
    }

    if (m_HostTaskManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRTASKHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostTaskManager,(void**)&taskManager) == S_OK &&
        taskManager != NULL) {
        m_HostTaskManager = taskManager;
        m_HostTaskManager->SetCLRTaskManager(&s_CLRTaskManager);
        g_fHostConfig |= CLRTASKHOSTED;
    }

    if (m_HostTaskManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRTASKHOSTED) &&
#endif
        taskManager != NULL) {
        m_HostTaskManager = taskManager;
        g_fHostConfig |= CLRTASKHOSTED;
    }

    if (m_HostThreadpoolManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRTHREADPOOLHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostThreadpoolManager,(void**)&threadpoolManager) == S_OK &&
        threadpoolManager != NULL) {
        m_HostThreadpoolManager = threadpoolManager;
        g_fHostConfig |= CLRTHREADPOOLHOSTED;
    }

    if (m_HostIoCompletionManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRIOCOMPLETIONHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostIoCompletionManager,(void**)&ioCompletionManager) == S_OK &&
        ioCompletionManager != NULL) {
        DWORD hostSize;
        hr = ioCompletionManager->GetHostOverlappedSize(&hostSize);
        if (FAILED(hr))
        {
            ioCompletionManager->Release();
            IfFailGo(E_UNEXPECTED);
        }
        m_HostOverlappedExtensionSize = (int)hostSize;
        m_HostIoCompletionManager = ioCompletionManager;
        m_HostIoCompletionManager->SetCLRIoCompletionManager(&s_CLRIoCompletionManager);
        g_fHostConfig |= CLRIOCOMPLETIONHOSTED;
    }

    if (m_HostSyncManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRSYNCHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostSyncManager,(void**)&syncManager) == S_OK &&
        syncManager != NULL) {
        m_HostSyncManager = syncManager;
        m_HostSyncManager->SetCLRSyncManager(&s_CLRSyncManager);
        g_fHostConfig |= CLRSYNCHOSTED;
    }

    if (m_HostAssemblyManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRASSEMBLYHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostAssemblyManager,(void**)&assemblyManager) == S_OK &&
        assemblyManager != NULL) {

        assemblyManager->GetAssemblyStore(&g_pHostAssemblyStore);

        hr = assemblyManager->GetNonHostStoreAssemblies(&g_pHostAsmList);
        if (FAILED(hr))
        {
            assemblyManager->Release();
            IfFailGo(hr);
        }

        if (g_pHostAssemblyStore || g_pHostAsmList)
            g_bFusionHosted = TRUE;
        m_HostAssemblyManager = assemblyManager;
        g_fHostConfig |= CLRASSEMBLYHOSTED;
    }

    if (m_HostGCManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRGCHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostGCManager,
                                     (void**)&gcManager) == S_OK &&
        gcManager != NULL) {
        m_HostGCManager = gcManager;
        g_fHostConfig |= CLRGCHOSTED;
    }

    if (m_HostSecurityManager == NULL &&
#ifdef _DEBUG
        (dbg_HostManagerConfig & CLRSECURITYHOSTED) &&
#endif
        pHostControl->GetHostManager(IID_IHostSecurityManager,
                                     (void**)&securityManager) == S_OK &&
        securityManager != NULL) {
        g_fHostConfig |= CLRSECURITYHOSTED;
        m_HostSecurityManager = securityManager;
        HostExecutionContextManager::InitializeRestrictedContext();
    }

    if (m_HostPolicyManager == NULL &&
        pHostControl->GetHostManager(IID_IHostPolicyManager,
                                     (void**)&policyManager) == S_OK &&
        policyManager != NULL) {
        m_HostPolicyManager = policyManager;
    }

    if (m_HostControl == NULL)
    {
        m_HostControl = pHostControl;
        m_HostControl->AddRef();
    }

ErrExit:
    fOneOnly = 0;

    END_SO_TOLERANT_CODE_CALLING_HOST;

    END_ENTRYPOINT_NOTHROW;

    return hr;
}

class CCLRPolicyManager: public ICLRPolicyManager
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetDefaultAction(EClrOperation operation,
                                                       EPolicyAction action)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_ENTRY_POINT;
        HRESULT hr;
        BEGIN_ENTRYPOINT_NOTHROW;
        hr = GetEEPolicy()->SetDefaultAction(operation, action);
        END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE SetTimeout(EClrOperation operation,
                                                 DWORD dwMilliseconds)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_ENTRY_POINT;
        HRESULT hr;
        BEGIN_ENTRYPOINT_NOTHROW;
        hr = GetEEPolicy()->SetTimeout(operation,dwMilliseconds);
        END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE SetActionOnTimeout(EClrOperation operation,
                                                         EPolicyAction action)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_ENTRY_POINT;
        HRESULT hr;
        BEGIN_ENTRYPOINT_NOTHROW;
        hr = GetEEPolicy()->SetActionOnTimeout(operation,action);
        END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE SetTimeoutAndAction(EClrOperation operation, DWORD dwMilliseconds,
                                                          EPolicyAction action)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return GetEEPolicy()->SetTimeoutAndAction(operation,dwMilliseconds,action);
    }

    virtual HRESULT STDMETHODCALLTYPE SetActionOnFailure(EClrFailure failure,
                                                         EPolicyAction action)
    {
        STATIC_CONTRACT_ENTRY_POINT;
        WRAPPER_CONTRACT;
        HRESULT hr;
        BEGIN_ENTRYPOINT_NOTHROW;
        hr = GetEEPolicy()->SetActionOnFailure(failure,action);
        END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE SetUnhandledExceptionPolicy(EClrUnhandledException policy)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_ENTRY_POINT;

        BEGIN_ENTRYPOINT_NOTHROW;

        _ASSERTE(GetEEPolicy() != NULL);
        GetEEPolicy()->SetUnhandledExceptionPolicy(policy);
        END_ENTRYPOINT_NOTHROW;

        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return 1;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return 1;
    }

    BEGIN_INTERFACE HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                                             void **ppvObject)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        if (riid != IID_ICLRPolicyManager && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppvObject = this;
        return S_OK;
    }
};

static CCLRPolicyManager s_PolicyManager;

class CCLROnEventManager: public ICLROnEventManager
{
public:
    virtual HRESULT STDMETHODCALLTYPE RegisterActionOnEvent(EClrEvent event,
                                                            IActionOnCLREvent *pAction)
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            NOTHROW;
            ENTRY_POINT;

            // This function is always called from outside the Runtime. So, we assert that we either don't have a
            // managed thread, or if we do, that we're in preemptive GC mode.
            PRECONDITION((GetThread() == NULL) || !GetThread()->PreemptiveGCDisabled());
        }
        CONTRACTL_END;

        if (event >= MaxClrEvent || pAction == NULL || event < (EClrEvent)0)
            return E_INVALIDARG;

        HRESULT hr = S_OK;
        BEGIN_ENTRYPOINT_NOTHROW;

        // Note: its only safe to use a straight ReleaseHolder from within the VM directory when we know we're
        // called from outside the Runtime. We assert that above, just to be sure.
        ReleaseHolder<IActionOnCLREvent>  actionHolder(pAction);
        pAction->AddRef();

        CrstHolder ch(m_pLock);
        while (m_ProcessEvent != 0)
        {
            ch.Release();
            __SwitchToThread(0);
            ch.Acquire();
        }

        if (m_pAction[event] == NULL)
        {
            m_pAction[event] = new (nothrow)ActionNode;
            if (m_pAction[event] == NULL)
                hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            ActionNode *walk = m_pAction[event];
            while (TRUE)
            {
                int n = 0;
                for ( ; n < ActionNode::ActionArraySize; n ++)
                {
                    if (walk->pAction[n] == NULL)
                    {
                        walk->pAction[n] = pAction;
                        actionHolder.SuppressRelease();
                        hr = S_OK;
                        break;
                    }
                }
                if (n < ActionNode::ActionArraySize)
                {
                    break;
                }
                if (walk->pNext == NULL)
                {
                    walk->pNext = new (nothrow) ActionNode;
                    if (walk->pNext == NULL)
                    {
                        hr = E_OUTOFMEMORY;
                        break;
                    }
                }
                walk = walk->pNext;
            }
        }

        END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE UnregisterActionOnEvent(EClrEvent event,
                                                              IActionOnCLREvent *pAction)
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            NOTHROW;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        if (event == Event_StackOverflow)
        {
            // We don't want to take a lock when we process StackOverflow event, because we may
            // not have enough stack to do it.
            // So we do not release our cache of the callback in order to avoid race.
            return HOST_E_INVALIDOPERATION;
        }

        HRESULT hr = S_OK;

        ActionNode *walk = NULL;
        ActionNode *prev = NULL;


        BEGIN_ENTRYPOINT_NOTHROW;

        CrstHolder ch(m_pLock);

        while (m_ProcessEvent != 0)
        {
            ch.Release();
            __SwitchToThread(0);
            ch.Acquire();
        }

        if (m_pAction[event] == NULL)
            IfFailGo(HOST_E_INVALIDOPERATION);

        walk = m_pAction[event];
        while (walk)
        {
            BOOL fInUse = FALSE;
            for (int n = 0; n < ActionNode::ActionArraySize; n ++)
            {
                if (prev && !fInUse && walk->pAction[n])
                    fInUse = TRUE;
                if (walk->pAction[n] == pAction)
                {
                    walk->pAction[n] = NULL;
                    ch.Release();
                    pAction->Release();
                    hr = S_OK;
                    goto ErrExit;
                }
            }
            if (prev && !fInUse)
            {
                prev->pNext = walk->pNext;
                delete walk;
                walk = prev;
            }
            prev = walk;
            walk = walk->pNext;
        }
        hr = HOST_E_INVALIDOPERATION;
ErrExit:
        END_ENTRYPOINT_NOTHROW;

        return hr;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return 1;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppUnk)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        if (riid != IID_ICLROnEventManager && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppUnk = this;
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return 1;
    }

    void ProcessEvent(EClrEvent event, void *data)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        WRAPPER_CONTRACT;

        if (m_pLock == NULL)
            return;

        if (event == Event_StackOverflow)
        {
            ActionNode *walk = m_pAction[event];

            while (walk)
            {
                for (int n = 0; n < ActionNode::ActionArraySize; n ++)
                {
                    if (walk->pAction[n])
                    {
                        walk->pAction[n]->OnEvent(event,data);
                    }
                }
                walk = walk->pNext;
            }

            return;
        }

        CrstHolder ch(m_pLock);

        if (event == Event_ClrDisabled)
        {
            if (m_CLRDisabled)
            {
                return;
            }
            m_CLRDisabled = TRUE;
        }
        m_ProcessEvent ++;

        ch.Release();

        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        ActionNode *walk = m_pAction[event];
        while (walk)
        {
            for (int n = 0; n < ActionNode::ActionArraySize; n ++)
            {
                if (walk->pAction[n])
                {
                    walk->pAction[n]->OnEvent(event,data);
                }
            }
            walk = walk->pNext;
        }
        END_SO_TOLERANT_CODE_CALLING_HOST;
        ch.Acquire();
        m_ProcessEvent --;
    }

    BOOL IsActionRegisteredForEvent(EClrEvent event)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        WRAPPER_CONTRACT;

        // Check to see if the event manager has been set up.
        if (m_pLock == NULL)
            return FALSE;

        CrstHolder ch(m_pLock);

        ActionNode *walk = m_pAction[event];
        while (walk)
        {
            for (int n = 0; n < ActionNode::ActionArraySize; n ++)
            {
                if (walk->pAction[n] != NULL)
                {
                    // We found an action registered for this event.
                    return TRUE;
                }
            }
            walk = walk->pNext;
        }

        // There weren't any actions registered.
        return FALSE;
    }

    HRESULT Init()
    {
        STATIC_CONTRACT_SO_TOLERANT;
        WRAPPER_CONTRACT;
        HRESULT hr = S_OK;
        if (m_pLock == NULL)
        {
            EX_TRY
            {
                InitHelper();
            }
            EX_CATCH
            {
                hr = GET_EXCEPTION()->GetHR();
            }
            EX_END_CATCH(SwallowAllExceptions);
        }

        return hr;
    }


private:
    struct ActionNode
    {
        static const int ActionArraySize = 8;

        IActionOnCLREvent *pAction[ActionArraySize];
        ActionNode        *pNext;

        ActionNode ()
        : pNext(NULL)
        {
            LEAF_CONTRACT;

            for (int n = 0; n < ActionArraySize; n ++)
                pAction[n] = 0;
        }
    };
    ActionNode *m_pAction[MaxClrEvent];

    Crst* m_pLock;

    BOOL m_CLRDisabled;

    // We can not call out into host while holding the lock.  At the same time
    // we need to make our data consistent.  Therefore, m_ProcessEvent is a marker
    // to forbid touching the data structure from Register and UnRegister.
    DWORD m_ProcessEvent;

    void InitHelper()
    {
        CONTRACTL
        {
            GC_NOTRIGGER;
            THROWS;
            MODE_ANY;
            SO_TOLERANT;
        }
        CONTRACTL_END;

        m_ProcessEvent = 0;

        Crst* tmp = new Crst("OnEventManager", CrstOnEventManager, CrstFlags(CRST_DEFAULT | CRST_DEBUGGER_THREAD));
        if (FastInterlockCompareExchangePointer((void**)&m_pLock, tmp, NULL) != NULL)
            delete tmp;
    }
};

static CCLROnEventManager s_OnEventManager;

void ProcessEventForHost(EClrEvent event, void *data)
{
    WRAPPER_CONTRACT;

    _ASSERTE (event != Event_StackOverflow);

    GCX_PREEMP();

    s_OnEventManager.ProcessEvent(event,data);
}

// We do not call ProcessEventForHost for stack overflow, since we have limit stack
// and we should avoid calling GCX_PREEMPT
void ProcessSOEventForHost(EXCEPTION_POINTERS *pExceptionInfo, BOOL fInSoTolerant)
{
}

BOOL IsHostRegisteredForEvent(EClrEvent event)
{
    WRAPPER_CONTRACT;

    return s_OnEventManager.IsActionRegisteredForEvent(event);
}

inline size_t SizeInKBytes(size_t cbSize)
{
    LEAF_CONTRACT;
    size_t cb = (cbSize % 1024) ? 1 : 0;
    return ((cbSize / 1024) + cb);
}

DWORD Host_SegmentSize = 0;
DWORD Host_MaxGen0Size = 0;
BOOL  Host_fSegmentSizeSet = FALSE;
BOOL  Host_fMaxGen0SizeSet = FALSE;

void UpdateGCSettingFromHost ()
{
    _ASSERTE (g_pConfig);
    if (Host_fSegmentSizeSet)
    {
        g_pConfig->SetSegmentSize(Host_SegmentSize);
    }
    if (Host_fMaxGen0SizeSet)
    {
        g_pConfig->SetGCgen0size(Host_MaxGen0Size);
    }
}

class CCLRGCManager: public ICLRGCManager
{
public:
    virtual HRESULT STDMETHODCALLTYPE Collect(LONG Generation)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        HRESULT     hr = S_OK;

        BEGIN_ENTRYPOINT_NOTHROW;

        if (Generation > (long) GCHeap::GetGCHeap()->GetMaxGeneration())
            hr = E_INVALIDARG;

        if (SUCCEEDED(hr))
        {
            // Put thread into co-operative mode, which is how GC must run.
            GCX_COOP();

            EX_TRY
            {
                STRESS_LOG0(LF_GC, LL_INFO100, "Host triggers GC\n");
                hr = GCHeap::GetGCHeap()->GarbageCollect(Generation);
            }
            EX_CATCH
            {
                hr = GET_EXCEPTION()->GetHR();
            }
            EX_END_CATCH(SwallowAllExceptions);
        }

        END_ENTRYPOINT_NOTHROW;

        return (hr);
    }

    virtual HRESULT STDMETHODCALLTYPE GetStats(COR_GC_STATS *pStats)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            ENTRY_POINT;
        }
        CONTRACTL_END;

        HRESULT hr = S_OK;
        BEGIN_ENTRYPOINT_NOTHROW;

    #if defined(ENABLE_PERF_COUNTERS)

        Perf_GC     *pgc = &GetPrivatePerfCounters().m_GC;

        if (!pStats)
            IfFailGo(E_INVALIDARG);

        if (pStats->Flags & COR_GC_COUNTS)
        {
            pStats->ExplicitGCCount = pgc->cInducedGCs;

            for (int idx=0; idx<3; idx++)
                pStats->GenCollectionsTaken[idx] = pgc->cGenCollections[idx];
        }

        if (pStats->Flags & COR_GC_MEMORYUSAGE)
        {
            pStats->CommittedKBytes = SizeInKBytes(pgc->cTotalCommittedBytes);
            pStats->ReservedKBytes = SizeInKBytes(pgc->cTotalReservedBytes);
            pStats->Gen0HeapSizeKBytes = SizeInKBytes(pgc->cGenHeapSize[0]);
            pStats->Gen1HeapSizeKBytes = SizeInKBytes(pgc->cGenHeapSize[1]);
            pStats->Gen2HeapSizeKBytes = SizeInKBytes(pgc->cGenHeapSize[2]);
            pStats->LargeObjectHeapSizeKBytes = SizeInKBytes(pgc->cLrgObjSize);
            pStats->KBytesPromotedFromGen0 = SizeInKBytes(pgc->cbPromotedMem[0]);
            pStats->KBytesPromotedFromGen1 = SizeInKBytes(pgc->cbPromotedMem[1]);
        }
        hr = S_OK;
    #else
        hr = E_NOTIMPL;
    #endif // ENABLE_PERF_COUNTERS

ErrExit:
        END_ENTRYPOINT_NOTHROW;
        return hr;
    }
    virtual HRESULT STDMETHODCALLTYPE SetGCStartupLimits(
        DWORD SegmentSize,
        DWORD MaxGen0Size)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            ENTRY_POINT;

        }
        CONTRACTL_END;

        HRESULT hr = S_OK;
        BEGIN_ENTRYPOINT_NOTHROW;

        // Set default overrides if specified by caller.
        if (SegmentSize != (DWORD) ~0 && SegmentSize > 0)
        {
            // Sanity check the value, it must be a power of two and big enough.
            if (!GCHeap::IsValidSegmentSize(SegmentSize))
            {
                hr = E_INVALIDARG;
            }
            else
            {
                Host_SegmentSize = SegmentSize;
                Host_fSegmentSizeSet = TRUE;
            }
        }

        if (SUCCEEDED(hr) && MaxGen0Size != (DWORD) ~0 && MaxGen0Size > 0)
        {
            // Sanity check the value is at least large enough.
            if (!GCHeap::IsValidGen0MaxSize(MaxGen0Size))
            {
                hr = E_INVALIDARG;
            }
            else
            {
            Host_MaxGen0Size = MaxGen0Size;
            Host_fMaxGen0SizeSet = TRUE;
        }
        }
        END_ENTRYPOINT_NOTHROW;

        return (hr);
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, OUT PVOID *ppUnk)
    {
        LEAF_CONTRACT;
        if (riid != IID_ICLRGCManager && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppUnk = this;
        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LEAF_CONTRACT;
        return 1;
    }
};

static CCLRGCManager s_GCManager;

BYTE g_CorHostProtectionManagerInstance[sizeof(CorHostProtectionManager)];

void InitHostProtectionManager()
{
    WRAPPER_CONTRACT;
    new (g_CorHostProtectionManagerInstance) CorHostProtectionManager();
}

BOOL g_CLRPolicyRequested = FALSE;

class CCorCLRControl: public ICLRControl
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetCLRManager(REFIID riid, void **ppObject)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            SO_TOLERANT;    // no global state updates
        }
        CONTRACTL_END;

        // Sanity check.
        if (ppObject == NULL)
            return E_INVALIDARG;

        // ErrorReportingManager is allowed, even if runtime is started, so
        //  make this check first.
        // Host must call release on CLRErrorReportingManager after this call
        if (riid == IID_ICLRErrorReportingManager)
        {
            *ppObject = &s_CLRErrorReportingManager;
            return S_OK;
        }

        else if (g_fEEStarted && !m_fFullAccess)
        {
            // If runtime has been started, do not allow user to obtain CLR managers.
            return HOST_E_INVALIDOPERATION;
        }

        else if (riid == IID_ICLRTaskManager) {
            *ppObject = &s_CLRTaskManager;
            return S_OK;
        }
        else if (riid == IID_ICLRPolicyManager) {
            *ppObject = &s_PolicyManager;
            FastInterlockExchange((LONG*)&g_CLRPolicyRequested, TRUE);
            return S_OK;
        }
        else if (riid == IID_ICLRHostProtectionManager) {
            *ppObject = GetHostProtectionManager();
            return S_OK;
        }

        // Host must call release on CLRDebugManager after this call
        else if (riid == IID_ICLRDebugManager)
        {
            *ppObject = &s_CLRDebugManager;
            return S_OK;
        }

        else if (riid == IID_ICLROnEventManager)
        {
            HRESULT hr = s_OnEventManager.Init();
            if (FAILED(hr))
                return hr;
            *ppObject = &s_OnEventManager;
            return S_OK;
        }
        else if (riid == IID_ICLRGCManager)
        {
            *ppObject = &s_GCManager;
            return S_OK;
        }
        else
            return (E_NOINTERFACE);
    }

    virtual HRESULT STDMETHODCALLTYPE SetAppDomainManagerType(
        LPCWSTR pwzAppDomainManagerAssembly,
        LPCWSTR pwzAppDomainManagerType)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            ENTRY_POINT;    // no global state updates
        }
        CONTRACTL_END;

        HRESULT hr = S_OK;

        NewArrayHolder<WCHAR> pAppDomainManagerAsm;
        NewArrayHolder<WCHAR> pAppDomainManagerType;

        BEGIN_ENTRYPOINT_NOTHROW;

        if (g_fEEStarted || CorHost2::HasStarted())
        {
            hr = HOST_E_INVALIDOPERATION;
        }
        // Most specify both or neither
        else if (pwzAppDomainManagerAssembly == NULL ||
            pwzAppDomainManagerType == NULL)
        {
            hr = E_INVALIDARG;
        }

        size_t lenAsm = 0;
        size_t lenType = 0;
        if (SUCCEEDED(hr))
        {
            lenAsm = wcslen(pwzAppDomainManagerAssembly) + 1;
            lenType = wcslen(pwzAppDomainManagerType) + 1;

            pAppDomainManagerAsm.Assign(new (nothrow) WCHAR[lenAsm]);
            if (!pAppDomainManagerAsm)
            {
                 hr = E_OUTOFMEMORY;
            }
        }

        if (SUCCEEDED(hr))
        {
            pAppDomainManagerType.Assign(new (nothrow) WCHAR[lenType]);
            if (!pAppDomainManagerType)
            {
                hr = E_OUTOFMEMORY;
            }
        }

        if (SUCCEEDED(hr))
        {
            wcsncpy_s(pAppDomainManagerAsm, lenAsm, pwzAppDomainManagerAssembly, lenAsm - 1);
            wcsncpy_s(pAppDomainManagerType, lenType, pwzAppDomainManagerType, lenType - 1);

            if (FastInterlockCompareExchangePointer((void**)&m_wszAppDomainManagerAsm,
                                                    pAppDomainManagerAsm, NULL) == NULL)
            {
                m_wszAppDomainManagerType = pAppDomainManagerType;
                    hr = S_OK;
            }
            else
            {
                hr = E_FAIL;
            }
        }
        if (SUCCEEDED(hr))
        {
            pAppDomainManagerAsm.SuppressRelease();
            pAppDomainManagerType.SuppressRelease();
        }
        END_ENTRYPOINT_NOTHROW;
        return hr;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LEAF_CONTRACT;
        return 1;
    }

    BEGIN_INTERFACE HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                                             void **ppvObject)
    {
        LEAF_CONTRACT;
        if (riid != IID_ICLRControl && riid != IID_IUnknown)
            return (E_NOINTERFACE);
        *ppvObject = this;
        return S_OK;
    }
    LPCWSTR GetAppDomainManagerAsm()
    {
        LEAF_CONTRACT;
        return m_wszAppDomainManagerAsm;
    }

    LPCWSTR GetAppDomainManagerType()
    {
        LEAF_CONTRACT;
        return m_wszAppDomainManagerType;
    }

    // This is to avoid having ctor.  We have static objects, and it is
    // difficult to support ctor on certain platform.
    void SetAccess(BOOL fFullAccess)
    {
        LEAF_CONTRACT;
        m_fFullAccess = fFullAccess;
    }
private:
    LPWSTR m_wszAppDomainManagerAsm;
    LPWSTR m_wszAppDomainManagerType;
    BOOL m_fFullAccess;
};

// Before CLR starts, we give out s_CorCLRControl which has full access to all managers.
// After CLR starts, we give out s_CorCLRControlLimited which allows limited access to managers.
static CCorCLRControl s_CorCLRControl;
static CCorCLRControl s_CorCLRControlLimited;

///////////////////////////////////////////////////////////////////////////////
// ICLRRuntimeHost::GetCLRControl
HRESULT CorHost2::GetCLRControl(ICLRControl** pCLRControl)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;

    BEGIN_ENTRYPOINT_NOTHROW;

    if (!g_fEEStarted && m_Version == 2)
    {
        s_CorCLRControl.SetAccess(TRUE);
        *pCLRControl = &s_CorCLRControl;
    }
    else
    {
        // Even CLR is hosted by v1 hosting interface, we still allow part of CLRControl, like IID_ICLRErrorReportingManager.
        s_CorCLRControlLimited.SetAccess(FALSE);
        *pCLRControl = &s_CorCLRControlLimited;
    }
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}

LPCWSTR CorHost2::GetAppDomainManagerAsm()
{
    LEAF_CONTRACT;
    _ASSERTE (g_fEEStarted);
    return s_CorCLRControl.GetAppDomainManagerAsm();
}

LPCWSTR CorHost2::GetAppDomainManagerType()
{
    LEAF_CONTRACT;
    _ASSERTE (g_fEEStarted);
    return s_CorCLRControl.GetAppDomainManagerType();
}

// We do not implement the Release since our host does not control the lifetime on this object
ULONG CCLRDebugManager::Release()
{
    LEAF_CONTRACT;
    return (1);
}

HRESULT CCLRDebugManager::QueryInterface(REFIID riid, void **ppUnk)
{
    if (!ppUnk)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    if (ppUnk == NULL)
    {
        return E_POINTER;
    }

    *ppUnk = 0;

    // Deliberately do NOT hand out ICorConfiguration.  They must explicitly call
    // GetConfiguration to obtain that interface.
    if (riid == IID_IUnknown)
    {
        *ppUnk = (IUnknown *) this;
    }
    else if (riid == IID_ICLRDebugManager)
    {
        *ppUnk = (ICLRDebugManager *) this;
    }
    else 
    {
        hr = E_NOINTERFACE;
    }

    return hr;

}

/*
*
* Called once to when process start up to initialize the lock for connection name hash table
*
*/
void CCLRDebugManager::ProcessInit()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_lockConnectionNameTable.Init("ConnectionNameTableLock", CrstDummy, (CrstFlags) (CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));
}

/*
* Called once to when process shut down to destroy the lock for connection name hash table
*
*/
void CCLRDebugManager::ProcessCleanup()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;

    m_lockConnectionNameTable.Destroy();
}

/*
*   Call to send out notification for existing connections upon debugger attach
*
*/
HRESULT CCLRDebugManager::IterateConnectionForAttach(BOOL fAttaching)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;

    HRESULT     hr = S_OK;
    HASHFIND    hashfind;
    ConnectionNameHashEntry* pConnection;

    // grab the critical section before using the data
    CrstHolder  ch(&m_lockConnectionNameTable);

    // no connection yet
    if (m_pConnectionNameHash == NULL || fAttaching == FALSE)
        return hr;

    // send out the create connection for each existing connection
    pConnection = (ConnectionNameHashEntry*) m_pConnectionNameHash->FindFirstEntry(&hashfind);
    while (pConnection)
    {
        _ASSERTE(g_pDebugInterface != NULL);

        // send out notification
        g_pDebugInterface->CreateConnection(pConnection->m_dwConnectionId, pConnection->m_pwzName, TRUE);

        // now get the next connection record
        pConnection = (ConnectionNameHashEntry*)m_pConnectionNameHash->FindNextEntry(&hashfind);
    }

    // now send out the change connection for each existing connection
    pConnection = (ConnectionNameHashEntry*) m_pConnectionNameHash->FindFirstEntry(&hashfind);
    while (pConnection)
    {
        _ASSERTE(g_pDebugInterface != NULL);

        // send out notification
        g_pDebugInterface->ChangeConnection(pConnection->m_dwConnectionId, TRUE);

        // now get the next connection record
        pConnection = (ConnectionNameHashEntry*)m_pConnectionNameHash->FindNextEntry(&hashfind);
    }

    return hr;
}

HRESULT CCLRDebugManager::IsDebuggerAttached(BOOL *pbAttached)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if (pbAttached == NULL)
        return E_INVALIDARG;

    BEGIN_ENTRYPOINT_NOTHROW;

#ifdef DEBUGGING_SUPPORTED
    *pbAttached = (CORDebuggerAttached() != 0);
#else
    *pbAttached = FALSE;
#endif

    END_ENTRYPOINT_NOTHROW;


    return S_OK;
}

// By default, we permit symbols to be read for full-trust assemblies only
ESymbolReadingSetBy CCLRDebugManager::m_symbolReadingSetBy = eSymbolReadingSetByDefault;
ESymbolReadingPolicy CCLRDebugManager::m_symbolReadingPolicy = eSymbolReadingFullTrustOnly;

HRESULT CCLRDebugManager::SetSymbolReadingPolicy(ESymbolReadingPolicy policy)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;

    if( policy > eSymbolReadingFullTrustOnly )
    {
        return E_INVALIDARG;
    }

    SetSymbolReadingPolicy( policy, eSymbolReadingSetByHost );    

    return S_OK;
}

void CCLRDebugManager::SetSymbolReadingPolicy( ESymbolReadingPolicy policy, ESymbolReadingSetBy setBy )
{
    LEAF_CONTRACT;
    _ASSERTE( policy <= eSymbolReadingFullTrustOnly );  // don't have _COUNT because it's not in convention for mscoree.idl enums
    _ASSERTE( setBy < eSymbolReadingSetBy_COUNT );

    // if the setter meets or exceeds the precendence of the existing setting then override the setting
    if( setBy >= m_symbolReadingSetBy )
    {
        m_symbolReadingSetBy = setBy;
        m_symbolReadingPolicy = policy;
    }
}


/*
*   Call by host to set the name of a connection and begin a connection.
*
*/
HRESULT CCLRDebugManager::BeginConnection(
        CONNID  dwConnectionId,
        __in_z wchar_t *wzConnectionName)
{
    CONTRACTL
    {
        GC_TRIGGERS;    // I am having problem in putting either GC_TRIGGERS or GC_NOTRIGGER. It is not happy either way when debugger
                        // call back event needs to enable preemptive GC.
        ENTRY_POINT;
        NOTHROW;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    ConnectionNameHashEntry *pEntry = NULL;

    CrstHolder ch(&m_lockConnectionNameTable);

    // check input parameter
    if (dwConnectionId == INVALID_CONNECTION_ID || wzConnectionName == NULL || wzConnectionName[0] == L'\0')
        IfFailGo(E_INVALIDARG);

    if (wcslen(wzConnectionName) >= MAX_CONNECTION_NAME)
        IfFailGo(E_INVALIDARG);

    if (m_pConnectionNameHash == NULL)
    {
        m_pConnectionNameHash = new (nothrow) ConnectionNameTable(50);
        IfNullGo(m_pConnectionNameHash);
        IfFailGo(m_pConnectionNameHash->NewInit(50, sizeof(ConnectionNameHashEntry), USHRT_MAX));
    }

    // error: Should not have an existing connection id already
    if (m_pConnectionNameHash->FindConnection(dwConnectionId))
        IfFailGo(E_INVALIDARG);

    // Our implementation of hashtable cannot throw out of memory exception
    pEntry = m_pConnectionNameHash->AddConnection(dwConnectionId, wzConnectionName);
    IfNullGo(pEntry);

    ch.Release();

    // send notification to debugger
    if (CORDebuggerAttached())
        g_pDebugInterface->CreateConnection(dwConnectionId, wzConnectionName, FALSE);

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

/*
*   Call by host to end a connection
*/
HRESULT CCLRDebugManager::EndConnection(CONNID   dwConnectionId)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        NOTHROW;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    UINT CLRTaskCount = 0;
    ICLRTask **ppCLRTaskArray = NULL;

    BEGIN_ENTRYPOINT_NOTHROW;


    CrstHolder ch(&m_lockConnectionNameTable);
    ConnectionNameHashEntry *pEntry = NULL;

    if (dwConnectionId == INVALID_CONNECTION_ID)
        IfFailGo(E_INVALIDARG);

    // No connection exist at all
    if (m_pConnectionNameHash == NULL)
        IfFailGo(E_FAIL);

    if ((pEntry = m_pConnectionNameHash->FindConnection(dwConnectionId)) == NULL)
        IfFailGo(E_INVALIDARG);

    // Note that the Release on CLRTask chould take a ThreadStoreLock. So we need to finish our
    // business with ConnectionNameHash before hand and release our name hash lock
    //
    CLRTaskCount = pEntry->m_CLRTaskCount;
    ppCLRTaskArray = pEntry->m_ppCLRTaskArray;
    pEntry->m_ppCLRTaskArray = NULL;
    pEntry->m_CLRTaskCount = 0;
    m_pConnectionNameHash->DeleteConnection(dwConnectionId);
    ch.Release();

    if (CLRTaskCount != 0)
    {
        _ASSERTE(ppCLRTaskArray != NULL);
        for (UINT i = 0; i < CLRTaskCount; i++)
        {
            ((Thread *)ppCLRTaskArray[i])->SetConnectionId(INVALID_CONNECTION_ID);
            ppCLRTaskArray[i]->Release();
        }
        delete [] ppCLRTaskArray;
    }

    // send notification to debugger
    if (CORDebuggerAttached())
        g_pDebugInterface->DestroyConnection(dwConnectionId);

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

/*
*   Call by host to set a set of tasks as a connection.
*
*/
HRESULT CCLRDebugManager::SetConnectionTasks(
    DWORD id,
    DWORD dwCount,
    ICLRTask **ppCLRTask)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        NOTHROW;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    ICLRTask **ppCLRTaskArrayNew = NULL;
    UINT CLRTaskCountPrevious = 0;
    ICLRTask **ppCLRTaskArrayPrevious = NULL;

    BEGIN_ENTRYPOINT_NOTHROW;

    CrstHolder ch(&m_lockConnectionNameTable);

    DWORD       index;
    Thread      *pThread;
    ConnectionNameHashEntry *pEntry = NULL;

    if (id == INVALID_CONNECTION_ID || dwCount == 0 || ppCLRTask == NULL)
        IfFailGo(E_INVALIDARG);

    // check the BeginConnectin has been called.
    if (m_pConnectionNameHash == NULL)
        // No connection exist
        IfFailGo(E_INVALIDARG);

    // Host forget to call BeginConnection before calling SetConnectionTask!
    if ((pEntry = m_pConnectionNameHash->FindConnection(id)) == NULL)
        IfFailGo(E_INVALIDARG);

    for (index = 0; index < dwCount; index++)
    {
        // Check on input parameter
        pThread = (Thread *) ppCLRTask[index];
        if (pThread == NULL)
        {
            // _ASSERTE(!"Host passed in NULL ICLRTask pointer");
            IfFailGo(E_INVALIDARG);
        }

        // Check for Finalizer thread
        if (GCHeap::IsGCHeapInitialized() && (pThread == GCHeap::GetGCHeap()->GetFinalizerThread()))
        {
            // _ASSERTE(!"Host should not try to schedule user code on our Finalizer Thread");
            IfFailGo(E_INVALIDARG);

        }
    }

    ppCLRTaskArrayNew = new (nothrow) ICLRTask*[dwCount];
    IfNullGo(ppCLRTaskArrayNew);

    CLRTaskCountPrevious = pEntry->m_CLRTaskCount;
    ppCLRTaskArrayPrevious = pEntry->m_ppCLRTaskArray;
    pEntry->m_ppCLRTaskArray = NULL;
    pEntry->m_CLRTaskCount = 0;

    if (CLRTaskCountPrevious != 0)
    {
        // Clear the old connection set
        _ASSERTE(ppCLRTaskArrayPrevious != NULL);
        for (UINT i = 0; i < CLRTaskCountPrevious; i++)
            ((Thread *)ppCLRTaskArrayPrevious[i])->SetConnectionId(INVALID_CONNECTION_ID);
    }

    // now remember the new set
    pEntry->m_ppCLRTaskArray = ppCLRTaskArrayNew;

    for (index = 0; index < dwCount; index++)
    {
        pThread = (Thread *) ppCLRTask[index];
        pThread->SetConnectionId( id );
        pEntry->m_ppCLRTaskArray[index] = ppCLRTask[index];
    }
    pEntry->m_CLRTaskCount = dwCount;

    // AddRef and Release on Thread object can call ThreadStoreLock. So we will release our
    // lock first of all.
    ch.Release();

    // Does the addref on the new set
    for (index = 0; index < dwCount; index++)
        ppCLRTaskArrayNew[index]->AddRef();

    // Does the release on the old set
    if (CLRTaskCountPrevious != 0)
    {
        _ASSERTE(ppCLRTaskArrayPrevious != NULL);
        for (UINT i = 0; i < CLRTaskCountPrevious; i++)
            ppCLRTaskArrayPrevious[i]->Release();
        delete ppCLRTaskArrayPrevious;
    }

    // send notification to debugger
    if (CORDebuggerAttached())
        g_pDebugInterface->ChangeConnection(id, FALSE);

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

HRESULT CCLRDebugManager::SetDacl(PACL pacl)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;
    HRESULT hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    hr = E_NOTIMPL;

    END_ENTRYPOINT_NOTHROW;
    return hr;
}   // SetDACL


HRESULT CCLRDebugManager::GetDacl(PACL *pacl)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;
    HRESULT hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    hr = E_NOTIMPL;

    END_ENTRYPOINT_NOTHROW;
    return hr;
}   // SetDACL


HRESULT CCLRErrorReportingManager::QueryInterface(REFIID riid, void** ppUnk)
{
    if (!ppUnk)
        return E_POINTER;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    
    if (ppUnk == NULL)
    {
        return E_POINTER;
    }

    *ppUnk = 0;

    // Deliberately do NOT hand out ICorConfiguration.  They must explicitly call
    // GetConfiguration to obtain that interface.
    if (riid == IID_IUnknown)
    {
        *ppUnk = (IUnknown *) this;
    }
    else if (riid == IID_ICLRErrorReportingManager)
    {
        *ppUnk = (ICLRErrorReportingManager *) this;
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;

} // HRESULT CCLRErrorReportingManager::QueryInterface()

ULONG CCLRErrorReportingManager::AddRef()
{
    LEAF_CONTRACT;
    return 1;
} // HRESULT CCLRErrorReportingManager::AddRef()

ULONG CCLRErrorReportingManager::Release()
{
    LEAF_CONTRACT;
    return 1;
} // HRESULT CCLRErrorReportingManager::Release()

// Get Watson bucket parameters for "current" exception (on calling thread).
HRESULT CCLRErrorReportingManager::GetBucketParametersForCurrentException(
    BucketParameters *pParams)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // To avoid confusion, clear the buckets.
    memset(pParams, 0, sizeof(BucketParameters));

    // Since Rotor doesn't implement Watson, return a not implemented error.
    hr = E_NOTIMPL;

    END_ENTRYPOINT_NOTHROW;

    return hr;

} // HRESULT CCLRErrorReportingManager::GetBucketParametersForCurrentException()

//
// The BeginCustomDump function configures the custom dump support
//
// Parameters -
// dwFlavor     - The flavor of the dump
// dwNumItems   - The number of items in the CustomDumpItem array.
//                  Should always be zero today, since no custom items are defined
// items        - Array of CustomDumpItem structs specifying items to be added to the dump.
//                  Should always be NULL today, since no custom items are defined.
// dwReserved   - reserved for future use. Must be zero today
//
HRESULT CCLRErrorReportingManager::BeginCustomDump( ECustomDumpFlavor dwFlavor,
                                        DWORD dwNumItems,
                                        CustomDumpItem items[],
                                        DWORD dwReserved)
{
    STATIC_CONTRACT_ENTRY_POINT;
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    if (dwNumItems != 0 ||  items != NULL || dwReserved != 0)
    {
        IfFailGo(E_INVALIDARG);
    }
    if (g_ECustomDumpFlavor != DUMP_FLAVOR_Default)
    {
        // BeginCustomDump is called without matching EndCustomDump
        IfFailGo(E_INVALIDARG);
    }
    g_ECustomDumpFlavor = dwFlavor;

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

//
// EndCustomDump clears the custom dump configuration
//
HRESULT CCLRErrorReportingManager::EndCustomDump()
{
    STATIC_CONTRACT_ENTRY_POINT;
    // NOT IMPLEMENTED YET
    BEGIN_ENTRYPOINT_NOTHROW;
    g_ECustomDumpFlavor = DUMP_FLAVOR_Default;
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}



// This is the instance that exposes interfaces out to all the other DLLs of the CLR
// so they can use our services for TLS, synchronization, memory allocation, etc.
static BYTE g_CEEInstance[sizeof(CExecutionEngine)];
static IExecutionEngine * g_pCEE = NULL;

PTLS_CALLBACK_FUNCTION CExecutionEngine::Callbacks[MAX_PREDEFINED_TLS_SLOT];

extern "C" IExecutionEngine * __stdcall IEE()
{
    LEAF_CONTRACT;




    if ( !g_pCEE )
    {
        // Create a local copy on the stack and then copy it over to the static instance.
        // This avoids race conditions caused by multiple initializations of vtable in the constructor
       CExecutionEngine local;
       memcpy(&g_CEEInstance, &local, sizeof(CExecutionEngine));

       g_pCEE = (IExecutionEngine *)(CExecutionEngine*)&g_CEEInstance;
    }
    //END_ENTRYPOINT_VOIDRET;

    return g_pCEE;
}


HRESULT STDMETHODCALLTYPE CExecutionEngine::QueryInterface(REFIID id, void **pInterface)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (!pInterface)
        return E_POINTER;

    *pInterface = NULL;

    //CANNOTTHROWCOMPLUSEXCEPTION();
    if (id == IID_IExecutionEngine)
        *pInterface = (IExecutionEngine *)this;
    else if (id == IID_IEEMemoryManager)
        *pInterface = (IEEMemoryManager *)this;
    else if (id == IID_IUnknown)
        *pInterface = (IUnknown *)(IExecutionEngine *)this;
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
} // HRESULT STDMETHODCALLTYPE CExecutionEngine::QueryInterface()


ULONG STDMETHODCALLTYPE CExecutionEngine::AddRef()
{
    LEAF_CONTRACT;
    return 1;
}

ULONG STDMETHODCALLTYPE CExecutionEngine::Release()
{
    LEAF_CONTRACT;
    return 1;
}

struct ClrTlsInfo
{
    void* data[MAX_PREDEFINED_TLS_SLOT];
    // When hosted, we may not be able to delete memory in DLL_THREAD_DETACH.
    // We will chain this into a side list, and free these on Finalizer thread.
    ClrTlsInfo *next;
};

#define DataToClrTlsInfo(a) (a)?(ClrTlsInfo*)((BYTE*)a - offsetof(ClrTlsInfo, data)):NULL

static BOOL fHasFlsSupport = FALSE;



static volatile ClrTlsInfo *g_pDetachedTlsInfo;

BOOL CExecutionEngine::HasDetachedTlsInfo()
{
    LEAF_CONTRACT;

    return g_pDetachedTlsInfo != NULL;
}

void CExecutionEngine::CleanupDetachedTlsInfo()
{
    WRAPPER_CONTRACT;

    if (g_pDetachedTlsInfo == NULL)
    {
        return;
    }
    ClrTlsInfo *head = (ClrTlsInfo*) FastInterlockExchangePointer((void **)&g_pDetachedTlsInfo, NULL);

    while (head)
    {
        ClrTlsInfo *node = head;
        head = head->next;
        DeleteTLS(node->data);
    }
}

void CExecutionEngine::DetachTlsInfo(void **pTlsData)
{
    LEAF_CONTRACT;
   
    if (pTlsData == NULL)
    {
        return;
    }    

    if (UnsafeTlsGetValue(TlsIndex) == pTlsData)
    {
        UnsafeTlsSetValue(TlsIndex, 0);
    }


    ClrTlsInfo *pTlsInfo = DataToClrTlsInfo(pTlsData);
    // PREFIX_ASSUME needs TLS.  If we use it here, we may do memory allocation.
    _ASSERTE(pTlsInfo != NULL);

    if (pTlsInfo->data[TlsIdx_StressLog])
    {
#ifdef STRESS_LOG
      CantAllocHolder caHolder; 
      StressLog::ThreadDetach ((ThreadStressLog *)pTlsInfo->data[TlsIdx_StressLog]);
      pTlsInfo->data[TlsIdx_StressLog] = NULL;
#else
        _ASSERTE (!"Shouldn't have stress log!");
#endif
    }

    while (TRUE)
    {
        ClrTlsInfo *head = (ClrTlsInfo*) g_pDetachedTlsInfo;
        pTlsInfo->next =  head;
        if (FastInterlockCompareExchangePointer((void**)&g_pDetachedTlsInfo, pTlsInfo, head) == (void*)head)
        {
            return;
        }
    }
}

// TLS support for the other DLLs of the CLR operates quite differently in hosted
// and unhosted scenarios.
void **CExecutionEngine::CheckThreadState(DWORD slot, BOOL force)
{
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_ANY;

    // !!! This function is called during Thread::SwitchIn and SwitchOut
    // !!! It is extremely important that while executing this function, we will not
    // !!! cause fiber switch.  This means we can not allocate memory, lock, etc...


    // Treat as a runtime assertion, since the invariant spans many DLLs.
    _ASSERTE(slot < MAX_PREDEFINED_TLS_SLOT);
//    if (slot >= MAX_PREDEFINED_TLS_SLOT)
//        COMPlusThrow(kArgumentOutOfRangeException);


    // Ensure we have a TLS Index
    if (TlsIndex == TLS_OUT_OF_INDEXES)
    {
        DWORD tryTlsIndex = UnsafeTlsAlloc();
        if (tryTlsIndex != TLS_OUT_OF_INDEXES)
        {
            if (FastInterlockCompareExchange((LONG*)&TlsIndex, tryTlsIndex, TLS_OUT_OF_INDEXES) != (LONG)TLS_OUT_OF_INDEXES)
            {
                UnsafeTlsFree(tryTlsIndex);
            }
        }
        if (TlsIndex == TLS_OUT_OF_INDEXES)
        {
            COMPlusThrowOM();
        }
    }

    void **pTlsData = (void **)UnsafeTlsGetValue(TlsIndex);
    BOOL fInTls = (pTlsData != NULL);


    ClrTlsInfo *pTlsInfo = DataToClrTlsInfo(pTlsData);

    if (pTlsInfo == 0 && force)
    {
#undef HeapAlloc
#undef GetProcessHeap
        // !!! Contract uses our TLS support.  Contract may be used before our host support is set up.
        // !!! To better support contract, we call into OS for memory allocation.
        pTlsInfo = (ClrTlsInfo*) ::HeapAlloc(GetProcessHeap(),0,sizeof(ClrTlsInfo));
#define GetProcessHeap() Dont_Use_GetProcessHeap()
#define HeapAlloc(hHeap, dwFlags, dwBytes) Dont_Use_HeapAlloc(hHeap, dwFlags, dwBytes)
        if (pTlsInfo == NULL)
        {
            goto LError;
        }
        memset (pTlsInfo, 0, sizeof(ClrTlsInfo));
    }

    if (!fInTls && pTlsInfo)
    {
        // If we have a thread object or are on a non-fiber thread, we are safe for fiber switching.
        if (!fHasFlsSupport ||
            GetThread() ||
            ((g_fEEStarted || g_fEEInit) && !CLRTaskHosted()) ||
            (((size_t)pTlsInfo->data[TlsIdx_ThreadType]) & (ThreadType_GC | ThreadType_Gate | ThreadType_Timer | ThreadType_DbgHelper)))
        {
#ifdef _DEBUG
            Thread *pThread = GetThread();
            if (pThread)
            {
                pThread->AddFiberInfo(Thread::ThreadTrackInfo_Lifetime);
            }
#endif
            if (UnsafeTlsSetValue(TlsIndex, pTlsInfo->data) == 0 && !fHasFlsSupport)
            {
                goto LError;
            }
        }
    }

    return pTlsInfo?pTlsInfo->data:NULL;

LError:
    if (pTlsInfo)
    {
#undef HeapFree
#undef GetProcessHeap
        ::HeapFree(GetProcessHeap(), 0, pTlsInfo);
#define GetProcessHeap() Dont_Use_GetProcessHeap()
#define HeapFree(hHeap, dwFlags, lpMem) Dont_Use_HeapFree(hHeap, dwFlags, lpMem)
    }
    // If this is for the stack probe, and we failed to allocate memory for it, we won't
    // put in a guard page.
    if (slot == TlsIdx_ClrDebugState || slot == TlsIdx_StackProbe)
        return NULL;

    ThrowOutOfMemory();
}


void **CExecutionEngine::CheckThreadStateNoCreate(DWORD slot)
{
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_MODE_ANY;

    // !!! This function is called during Thread::SwitchIn and SwitchOut
    // !!! It is extremely important that while executing this function, we will not
    // !!! cause fiber switch.  This means we can not allocate memory, lock, etc...


    // Treat as a runtime assertion, since the invariant spans many DLLs.
    _ASSERTE(slot < MAX_PREDEFINED_TLS_SLOT);

    // Ensure we have a TLS Index
    if (TlsIndex == TLS_OUT_OF_INDEXES)
    {
        return NULL;
    }

    _ASSERTE(TlsIndex != TLS_OUT_OF_INDEXES);

    void **pTlsData = (void **)UnsafeTlsGetValue(TlsIndex);


    ClrTlsInfo *pTlsInfo = DataToClrTlsInfo(pTlsData);

    return pTlsInfo?pTlsInfo->data:NULL;
}

void CExecutionEngine::SetupTLSForThread(Thread *pThread)
{
#ifdef _DEBUG
    pThread->AddFiberInfo(Thread::ThreadTrackInfo_Lifetime);
#endif
#ifdef STRESS_LOG
    if (StressLog::StressLogOn(-1, 0))
    {
        if (StressLog::CreateThreadStressLog() == NULL)
        {
            ThrowOutOfMemory();
        }
    }
#endif
    void **pTlsData = CheckThreadState(0);

    PREFIX_ASSUME(pTlsData != NULL);

}

// Keep the TLS info strictly either in the Thread or in TLS -- but never in both
// places at the same time.  One reason for the discipline is tighter assertions.
// The other reason is that it prevents a ~Thread and a DLL_THREAD_DETACH both
// cleaning up the same state by accident via the ThreadDetaching call.
void CExecutionEngine::SwitchIn()
{
    // No real contracts here.  This function is called by Thread::SwitchIn.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_ENTRY_POINT;


}

void CExecutionEngine::SwitchOut()
{
    // No real contracts here.  This function is called by Thread::SwitchOut
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_ENTRY_POINT;

    if (fHasFlsSupport)
    {
        UnsafeTlsSetValue(TlsIndex, NULL);
    }
}

static void ThreadDetachingHelper(PTLS_CALLBACK_FUNCTION callback, void* pData)
{
    // Do not use contract.  We are freeing TLS blocks.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    // We may be freeing ClrDebugState.  Using EX_TRY may need ClrDebugState.
    PAL_CPP_TRY
    {
        callback(pData);
    }
    PAL_CPP_CATCH_ALL
    {
        _ASSERTE (!"callback should not throw exception");
    }
    PAL_CPP_ENDTRY;
}

// Called here from a thread detach or from destruction of a Thread object.  In
// the detach case, we get our info from TLS.  In the destruct case, it comes from
// the object we are destructing.
void CExecutionEngine::ThreadDetaching(void ** pTlsData)
{
    // Can not cause memory allocation during thread detach, so no real contracts.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    if (TlsIndex == TLS_OUT_OF_INDEXES)
    {
        // We have not allocated TlsIndex yet.
        return;
    }

    // This function may be called twice:
    // 1. When a physical thread dies, our DLL_THREAD_DETACH calls this function with pTlsData = NULL
    // 2. When a fiber is destroyed, or OS calls FlsCallback after DLL_THREAD_DETACH process.
    // We will null the FLS and TLS entry if it matches the deleted one.

    if (pTlsData)
    {
        DeleteTLS (pTlsData);
    }
}

void CExecutionEngine::DeleteTLS(void ** pTlsData)
{
    // Can not cause memory allocation during thread detach, so no real contracts.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    if (TlsIndex == TLS_OUT_OF_INDEXES)
    {
        // We have not allocated TlsIndex yet.
        return;
    }

    PREFIX_ASSUME(pTlsData != NULL);

    ClrTlsInfo *pTlsInfo = DataToClrTlsInfo(pTlsData);
    BOOL fNeed;
    do
    {
        fNeed = FALSE;
        for (int i=0; i<MAX_PREDEFINED_TLS_SLOT; i++)
        {
            if (i == TlsIdx_ClrDebugState ||
                i == TlsIdx_StressLog)
            {
                // StressLog and DebugState may be needed during callback.
                continue;
            }
            // If we have some data and a callback, issue it.
            if (Callbacks[i] != 0 && pTlsInfo->data[i] != 0)
            {
                void* pData = pTlsInfo->data[i];
                pTlsInfo->data[i] = 0;
                ThreadDetachingHelper(Callbacks[i], pData);
                fNeed = TRUE;
            }
        }
    } while (fNeed);

    if (pTlsInfo->data[TlsIdx_StressLog] != 0)
    {
#ifdef STRESS_LOG
        StressLog::ThreadDetach((ThreadStressLog *)pTlsInfo->data[TlsIdx_StressLog]);
#else
        _ASSERTE (!"should not have StressLog");
#endif
    }
    if (Callbacks[TlsIdx_ClrDebugState] != 0 && pTlsInfo->data[TlsIdx_ClrDebugState] != 0)
    {
        void* pData = pTlsInfo->data[TlsIdx_ClrDebugState];
        pTlsInfo->data[TlsIdx_ClrDebugState] = 0;
        ThreadDetachingHelper(Callbacks[TlsIdx_ClrDebugState], pData);
    }

#ifdef _DEBUG
    Thread *pThread = GetThread();
    if (pThread)
    {
        pThread->AddFiberInfo(Thread::ThreadTrackInfo_Lifetime);
    }
#endif

    // NULL TLS and FLS entry so that we don't double free.
    // We may get two callback here on thread death
    // 1. From EEDllMain
    // 2. From OS callback on FLS destruction
    if (UnsafeTlsGetValue(TlsIndex) == pTlsData)
    {
        UnsafeTlsSetValue(TlsIndex, 0);
    }


#undef HeapFree
#undef GetProcessHeap
    ::HeapFree (GetProcessHeap(),0,pTlsInfo);
#define HeapFree(hHeap, dwFlags, lpMem) Dont_Use_HeapFree(hHeap, dwFlags, lpMem)
#define GetProcessHeap() Dont_Use_GetProcessHeap()

}


VOID STDMETHODCALLTYPE CExecutionEngine::TLS_AssociateCallback(DWORD slot, PTLS_CALLBACK_FUNCTION callback)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    CheckThreadState(slot);

    // They can toggle between a callback and no callback.  But anything else looks like
    // confusion on their part.
    //
    // (TlsIdx_ClrDebugState associates its callback from utilcode.lib - which can be replicated. But
    // all the callbacks are equally good.)
    _ASSERTE(slot == TlsIdx_ClrDebugState || Callbacks[slot] == 0 || Callbacks[slot] == callback || callback == 0);
    if (slot == TlsIdx_ClrDebugState)
    {
        _ASSERTE (!"should not get here");
    }
    else
        Callbacks[slot] = callback;
}

DWORD STDMETHODCALLTYPE CExecutionEngine::TLS_GetMasterSlotIndex()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CheckThreadState(0, FALSE);
    _ASSERTE(TlsIndex != TLS_OUT_OF_INDEXES);
    return TlsIndex;
}

LPVOID STDMETHODCALLTYPE CExecutionEngine::TLS_GetValue(DWORD slot)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EETlsGetValue(slot);
}

BOOL STDMETHODCALLTYPE CExecutionEngine::TLS_CheckValue(DWORD slot, LPVOID * pValue)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EETlsCheckValue(slot, pValue);
}

VOID STDMETHODCALLTYPE CExecutionEngine::TLS_SetValue(DWORD slot, LPVOID pData)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    EETlsSetValue(slot,pData);
}


VOID STDMETHODCALLTYPE CExecutionEngine::TLS_ThreadDetaching()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CExecutionEngine::ThreadDetaching(NULL);
}


CRITSEC_COOKIE STDMETHODCALLTYPE CExecutionEngine::CreateLock(LPCSTR szTag, LPCSTR level, CrstFlags flags)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_ENTRY_POINT;
    CRITSEC_COOKIE cookie = NULL;
    BEGIN_ENTRYPOINT_VOIDRET;
    cookie = ::EECreateCriticalSection(szTag, level, flags);
    END_ENTRYPOINT_VOIDRET;
    return cookie;
}

void STDMETHODCALLTYPE CExecutionEngine::DestroyLock(CRITSEC_COOKIE cookie)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    ::EEDeleteCriticalSection(cookie);
}

void STDMETHODCALLTYPE CExecutionEngine::AcquireLock(CRITSEC_COOKIE cookie)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    ::EEEnterCriticalSection(cookie);
}

void STDMETHODCALLTYPE CExecutionEngine::ReleaseLock(CRITSEC_COOKIE cookie)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    ::EELeaveCriticalSection(cookie);
}

// Locking routines supplied by the EE to the other DLLs of the CLR.  In a _DEBUG
// build of the EE, we poison the Crst as a poor man's attempt to do some argument
// validation.
#define POISON_BITS 3

static inline EVENT_COOKIE CLREventToCookie(CLREvent * pEvent)
{
    LEAF_CONTRACT;
    _ASSERTE((((uintptr_t) pEvent) & POISON_BITS) == 0);
#ifdef _DEBUG
    pEvent = (CLREvent *) (((uintptr_t) pEvent) | POISON_BITS);
#endif
    return (EVENT_COOKIE) pEvent;
}

static inline CLREvent *CookieToCLREvent(EVENT_COOKIE cookie)
{
    LEAF_CONTRACT;

    _ASSERTE((((uintptr_t) cookie) & POISON_BITS) == POISON_BITS);
#ifdef _DEBUG
    cookie = (EVENT_COOKIE) (((uintptr_t) cookie) & ~POISON_BITS);
#endif
    return (CLREvent *) cookie;
}


EVENT_COOKIE STDMETHODCALLTYPE CExecutionEngine::CreateAutoEvent(BOOL bInitialState)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    EVENT_COOKIE event = NULL;
    BEGIN_ENTRYPOINT_THROWS;
    NewHolder<CLREvent> pEvent(new CLREvent());
    pEvent->CreateAutoEvent(bInitialState);
    event = CLREventToCookie(pEvent);
    pEvent.SuppressRelease();
    END_ENTRYPOINT_THROWS;

    return event;
}

EVENT_COOKIE STDMETHODCALLTYPE CExecutionEngine::CreateManualEvent(BOOL bInitialState)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    EVENT_COOKIE event = NULL;
    BEGIN_ENTRYPOINT_THROWS;

    NewHolder<CLREvent> pEvent(new CLREvent());
    pEvent->CreateManualEvent(bInitialState);
    event = CLREventToCookie(pEvent);
    pEvent.SuppressRelease();

    END_ENTRYPOINT_THROWS;

    return event;
}

void STDMETHODCALLTYPE CExecutionEngine::CloseEvent(EVENT_COOKIE event)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if (event) {
        CLREvent *pEvent = CookieToCLREvent(event);
        pEvent->CloseEvent();
        delete pEvent;
    }
}

BOOL STDMETHODCALLTYPE CExecutionEngine::ClrSetEvent(EVENT_COOKIE event)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if (event) {
        CLREvent *pEvent = CookieToCLREvent(event);
        return pEvent->Set();
    }
    return FALSE;
}

BOOL STDMETHODCALLTYPE CExecutionEngine::ClrResetEvent(EVENT_COOKIE event)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if (event) {
        CLREvent *pEvent = CookieToCLREvent(event);
        return pEvent->Reset();
    }
    return FALSE;
}

DWORD STDMETHODCALLTYPE CExecutionEngine::WaitForEvent(EVENT_COOKIE event,
                                                       DWORD dwMilliseconds,
                                                       BOOL bAlertable)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if (event) {
        CLREvent *pEvent = CookieToCLREvent(event);
        return pEvent->Wait(dwMilliseconds,bAlertable);
    }

    if (GetThread() && bAlertable)
        ThrowHR(E_INVALIDARG);
    return WAIT_FAILED;
}

DWORD STDMETHODCALLTYPE CExecutionEngine::WaitForSingleObject(HANDLE handle,
                                                              DWORD dwMilliseconds)
{
    STATIC_CONTRACT_WRAPPER;
    STATIC_CONTRACT_SO_TOLERANT;
    LeaveRuntimeHolder holder((size_t)(::WaitForSingleObject));
    return ::WaitForSingleObject(handle,dwMilliseconds);
}

static inline SEMAPHORE_COOKIE CLRSemaphoreToCookie(CLRSemaphore * pSemaphore)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    _ASSERTE((((uintptr_t) pSemaphore) & POISON_BITS) == 0);
#ifdef _DEBUG
    pSemaphore = (CLRSemaphore *) (((uintptr_t) pSemaphore) | POISON_BITS);
#endif
    return (SEMAPHORE_COOKIE) pSemaphore;
}

static inline CLRSemaphore *CookieToCLRSemaphore(SEMAPHORE_COOKIE cookie)
{
    LEAF_CONTRACT;
    _ASSERTE((((uintptr_t) cookie) & POISON_BITS) == POISON_BITS);
#ifdef _DEBUG
    cookie = (SEMAPHORE_COOKIE) (((uintptr_t) cookie) & ~POISON_BITS);
#endif
    return (CLRSemaphore *) cookie;
}


SEMAPHORE_COOKIE STDMETHODCALLTYPE CExecutionEngine::ClrCreateSemaphore(DWORD dwInitial,
                                                                        DWORD dwMax)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    NewHolder<CLRSemaphore> pSemaphore(new CLRSemaphore());
    pSemaphore->Create(dwInitial, dwMax);
    SEMAPHORE_COOKIE ret = CLRSemaphoreToCookie(pSemaphore);;
    pSemaphore.SuppressRelease();
    return ret;
}

void STDMETHODCALLTYPE CExecutionEngine::ClrCloseSemaphore(SEMAPHORE_COOKIE semaphore)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CLRSemaphore *pSemaphore = CookieToCLRSemaphore(semaphore);
    pSemaphore->Close();
    delete pSemaphore;
}

BOOL STDMETHODCALLTYPE CExecutionEngine::ClrReleaseSemaphore(SEMAPHORE_COOKIE semaphore,
                                                             LONG lReleaseCount,
                                                             LONG *lpPreviousCount)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CLRSemaphore *pSemaphore = CookieToCLRSemaphore(semaphore);
    return pSemaphore->Release(lReleaseCount,lpPreviousCount);
}

DWORD STDMETHODCALLTYPE CExecutionEngine::ClrWaitForSemaphore(SEMAPHORE_COOKIE semaphore,
                                                              DWORD dwMilliseconds,
                                                              BOOL bAlertable)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CLRSemaphore *pSemaphore = CookieToCLRSemaphore(semaphore);
    return pSemaphore->Wait(dwMilliseconds,bAlertable);
}

static inline MUTEX_COOKIE CLRMutexToCookie(CLRMutex * pMutex)
{
    LEAF_CONTRACT;
    _ASSERTE((((uintptr_t) pMutex) & POISON_BITS) == 0);
#ifdef _DEBUG
    pMutex = (CLRMutex *) (((uintptr_t) pMutex) | POISON_BITS);
#endif
    return (MUTEX_COOKIE) pMutex;
}

static inline CLRMutex *CookieToCLRMutex(MUTEX_COOKIE cookie)
{
    LEAF_CONTRACT;
    _ASSERTE((((uintptr_t) cookie) & POISON_BITS) == POISON_BITS);
#ifdef _DEBUG
    cookie = (MUTEX_COOKIE) (((uintptr_t) cookie) & ~POISON_BITS);
#endif
    return (CLRMutex *) cookie;
}


MUTEX_COOKIE STDMETHODCALLTYPE CExecutionEngine::ClrCreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
                                                                BOOL bInitialOwner,
                                                                LPCTSTR lpName)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        SO_TOLERANT;    // we catch any erros and free the allocated memory
    }
    CONTRACTL_END;


    MUTEX_COOKIE mutex = 0;
    CLRMutex *pMutex = new (nothrow) CLRMutex();
    if (pMutex)
    {
        EX_TRY
        {
            pMutex->Create(lpMutexAttributes, bInitialOwner, lpName);
            mutex = CLRMutexToCookie(pMutex);
        }
        EX_CATCH
        {
            delete pMutex;
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    return mutex;
}

void STDMETHODCALLTYPE CExecutionEngine::ClrCloseMutex(MUTEX_COOKIE mutex)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CLRMutex *pMutex = CookieToCLRMutex(mutex);
    pMutex->Close();
    delete pMutex;
}

BOOL STDMETHODCALLTYPE CExecutionEngine::ClrReleaseMutex(MUTEX_COOKIE mutex)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CLRMutex *pMutex = CookieToCLRMutex(mutex);
    return pMutex->Release();
}

DWORD STDMETHODCALLTYPE CExecutionEngine::ClrWaitForMutex(MUTEX_COOKIE mutex,
                                                          DWORD dwMilliseconds,
                                                          BOOL bAlertable)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    CLRMutex *pMutex = CookieToCLRMutex(mutex);
    return pMutex->Wait(dwMilliseconds,bAlertable);
}

#undef ClrSleepEx
DWORD STDMETHODCALLTYPE CExecutionEngine::ClrSleepEx(DWORD dwMilliseconds, BOOL bAlertable)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return EESleepEx(dwMilliseconds,bAlertable);
}
#define ClrSleepEx EESleepEx

#undef ClrAllocationDisallowed
BOOL STDMETHODCALLTYPE CExecutionEngine::ClrAllocationDisallowed()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEAllocationDisallowed();
}
#define ClrAllocationDisallowed EEAllocationDisallowed

#undef ClrVirtualAlloc
LPVOID STDMETHODCALLTYPE CExecutionEngine::ClrVirtualAlloc(LPVOID lpAddress,
                                                           SIZE_T dwSize,
                                                           DWORD flAllocationType,
                                                           DWORD flProtect)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}
#define ClrVirtualAlloc EEVirtualAlloc

#undef ClrVirtualFree
BOOL STDMETHODCALLTYPE CExecutionEngine::ClrVirtualFree(LPVOID lpAddress,
                                                        SIZE_T dwSize,
                                                        DWORD dwFreeType)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEVirtualFree(lpAddress, dwSize, dwFreeType);
}
#define ClrVirtualFree EEVirtualFree

#undef ClrVirtualQuery
SIZE_T STDMETHODCALLTYPE CExecutionEngine::ClrVirtualQuery(LPCVOID lpAddress,
                                                           PMEMORY_BASIC_INFORMATION lpBuffer,
                                                           SIZE_T dwLength)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEVirtualQuery(lpAddress, lpBuffer, dwLength);
}
#define ClrVirtualQuery EEVirtualQuery

#undef ClrVirtualProtect
BOOL STDMETHODCALLTYPE CExecutionEngine::ClrVirtualProtect(LPVOID lpAddress,
                                                           SIZE_T dwSize,
                                                           DWORD flNewProtect,
                                                           PDWORD lpflOldProtect)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEVirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
}
#define ClrVirtualProtect EEVirtualProtect

#undef ClrGetProcessHeap
HANDLE STDMETHODCALLTYPE CExecutionEngine::ClrGetProcessHeap()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEGetProcessHeap();
}
#define ClrGetProcessHeap EEGetProcessHeap

#undef ClrGetProcessExecutableHeap
HANDLE STDMETHODCALLTYPE CExecutionEngine::ClrGetProcessExecutableHeap()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEGetProcessExecutableHeap();
}
#define ClrGetProcessExecutableHeap EEGetProcessExecutableHeap


#undef ClrHeapCreate
HANDLE STDMETHODCALLTYPE CExecutionEngine::ClrHeapCreate(DWORD flOptions,
                                                         SIZE_T dwInitialSize,
                                                         SIZE_T dwMaximumSize)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEHeapCreate(flOptions, dwInitialSize, dwMaximumSize);
}
#define ClrHeapCreate EEHeapCreate

#undef ClrHeapDestroy
BOOL STDMETHODCALLTYPE CExecutionEngine::ClrHeapDestroy(HANDLE hHeap)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEHeapDestroy(hHeap);
}
#define ClrHeapDestroy EEHeapDestroy

#undef ClrHeapAlloc
LPVOID STDMETHODCALLTYPE CExecutionEngine::ClrHeapAlloc(HANDLE hHeap,
                                                        DWORD dwFlags,
                                                        SIZE_T dwBytes)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    // We need to guarentee a very small stack consumption in allocating.  And we can't allow
    // an SO to happen while calling into the host.  This will force a hard SO which is OK because
    // we shouldn't ever get this close inside the EE in SO-intolerant code, so this should
    // only fail if we call directly in from outside the EE, such as the JIT.
    MINIMAL_STACK_PROBE_CHECK_THREAD(GetThread());

    return EEHeapAlloc(hHeap, dwFlags, dwBytes);
}
#define ClrHeapAlloc EEHeapAlloc

#undef ClrHeapFree
BOOL STDMETHODCALLTYPE CExecutionEngine::ClrHeapFree(HANDLE hHeap,
                                                     DWORD dwFlags,
                                                     LPVOID lpMem)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEHeapFree(hHeap, dwFlags, lpMem);
}
#define ClrHeapFree EEHeapFree

#undef ClrHeapValidate
BOOL STDMETHODCALLTYPE CExecutionEngine::ClrHeapValidate(HANDLE hHeap,
                                                         DWORD dwFlags,
                                                         LPCVOID lpMem)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return EEHeapValidate(hHeap, dwFlags, lpMem);
}
#define ClrHeapValidate EEHeapValidate

//------------------------------------------------------------------------------
// Helper function to get an exception object from outside the exception.  In
//  the CLR, it may be from the Thread object.  Non-CLR users have no thread object,
//  and it will do nothing.

void CExecutionEngine::GetLastThrownObjectExceptionFromThread(void **ppvException)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    // Cast to our real type.
    Exception **ppException = reinterpret_cast<Exception**>(ppvException);

    // Try to get a better message.
    GetLastThrownObjectExceptionFromThread_Internal(ppException);

} // HRESULT CExecutionEngine::GetLastThrownObjectExceptionFromThread()



#endif // !DACCESS_COMPILE

