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
// File: IPCFuncCallImpl.cpp
//
// Implement support for a cross process function call. 
//
//*****************************************************************************

#include "stdafx.h"
#include "ipcfunccall.h"
#include "ipcshared.h"

// #define ENABLE_TIMING

#ifdef ENABLE_TIMING
#include "timer.h"
CTimer g_time;
#endif // ENABLE_TIMING

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define NamePrexix L"Global\\ROTOR_"

// Name of sync objects
#define StartEnumEventName	NamePrexix L"PerfMon_StartEnumEvent"
#define DoneEnumEventName	NamePrexix L"PerfMon_DoneEnumEvent"
#define WrapMutexName		NamePrexix L"PerfMon_WrapMutex"

static LPCWSTR FixupName(LPCWSTR lpName)
{

    LEAF_CONTRACT;

    return lpName;
}

// Time the Source Caller is willing to wait for Handler to finish
// Note, a nefarious handler can at worst case make caller
// wait twice the delay below.
const DWORD START_ENUM_TIMEOUT = 500; // time out in milliseconds

//-----------------------------------------------------------------------------
// Wrap an unsafe call in a mutex to assure safety
// Biggest error issues are:
// 1. Timeout (probably handler doesn't exist)
// 2. Handler can be destroyed at any time.
//-----------------------------------------------------------------------------
IPCFuncCallSource::EError IPCFuncCallSource::DoThreadSafeCall()
{
    WRAPPER_CONTRACT;

    DWORD dwDesiredAccess;
    DWORD dwErr;
    EError err = Ok;

#if defined(ENABLE_TIMING)
    g_time.Reset();
    g_time.Start();
#endif

    dwDesiredAccess = EVENT_ALL_ACCESS;

    HANDLE hStartEnum = NULL;
    HANDLE hDoneEnum = NULL;
    HANDLE hWrapCall = NULL;
    DWORD dwWaitRet;

    // Check if we have a handler (handler creates the events) and
    // abort if not.  Do this check asap to optimize the most common
    // case of no handler.
    hStartEnum = WszOpenEvent(dwDesiredAccess,	
                                FALSE,
                                FixupName(StartEnumEventName));
    if (hStartEnum == NULL) 
    {
        dwErr = GetLastError();
        err = Fail_NoHandler;
        goto errExit;
    }

    hDoneEnum = WszOpenEvent(dwDesiredAccess,
                                FALSE,
                                FixupName(DoneEnumEventName));
    if (hDoneEnum == NULL) 
    {
        dwErr = GetLastError();
        err = Fail_NoHandler;
        goto errExit;
    }

    // Need to create the mutex
    hWrapCall = WszCreateMutex(NULL, FALSE, FixupName(WrapMutexName));
    if (hWrapCall == NULL)
    {
        dwErr = GetLastError();
        err = Fail_CreateMutex;
        goto errExit;
    }


// Wait for our turn	
    dwWaitRet = WaitForSingleObject(hWrapCall, START_ENUM_TIMEOUT);
    dwErr = GetLastError();
    switch(dwWaitRet) {
    case WAIT_OBJECT_0:
        // Good case. All other cases are errors and goto errExit.
        break;

    case WAIT_TIMEOUT:
        err = Fail_Timeout_Lock;
        goto errExit;
        break;
    default:
        err = Failed;
        goto errExit;
        break;
    }

    // Our turn: Make the function call
    {
        BOOL fSetOK = 0;

    // Reset the 'Done event' to make sure that Handler sets it after they start.
        fSetOK = ResetEvent(hDoneEnum);
        _ASSERTE(fSetOK);
        dwErr = GetLastError();

    // Signal Handler to execute callback   
        fSetOK = SetEvent(hStartEnum);
        _ASSERTE(fSetOK);
        dwErr = GetLastError();

    // Now wait for handler to finish.
        
        dwWaitRet = WaitForSingleObject(hDoneEnum, START_ENUM_TIMEOUT);
        dwErr = GetLastError();
        switch (dwWaitRet)
        {   
        case WAIT_OBJECT_0:
            break;
        case WAIT_TIMEOUT:
            err = Fail_Timeout_Call;
            break;      
        default:
            err = Failed;
            break;
        }
        

        BOOL fMutexOk;
        fMutexOk = ReleaseMutex(hWrapCall);
        _ASSERTE(fMutexOk);
        dwErr = GetLastError();

    } // End function call



errExit:
// Close all handles
    if (hStartEnum != NULL) 
    {
        CloseHandle(hStartEnum);
        hStartEnum = NULL;
        
    }
    if (hDoneEnum != NULL) 
    {
        CloseHandle(hDoneEnum);
        hDoneEnum = NULL;
    }
    if (hWrapCall != NULL) 
    {
        CloseHandle(hWrapCall);
        hWrapCall = NULL;
    }

#if defined(ENABLE_TIMING)
    g_time.End();
    DWORD dwTime = g_time.GetEllapsedMS();
#endif


    return err;

}


// Reset vars so we can be sure that Init was called
IPCFuncCallHandler::IPCFuncCallHandler()
{   
    m_hStartEnum    = NULL; // event to notify start call
    m_hDoneEnum     = NULL; // event to notify end call
    m_hAuxThread    = NULL; // thread to listen for m_hStartEnum
    m_pfnCallback   = NULL; // Callback handler
    m_fShutdownAuxThread = FALSE;
    m_hShutdownThread = NULL;
    m_hAuxThreadShutdown = NULL;
    m_hCallbackModule = NULL; // module in which the aux thread's start function lives
}

IPCFuncCallHandler::~IPCFuncCallHandler()
{
    // If Terminate was not called then do so now. This should have been 
    // called from CloseCtrs perf counters API. But in Windows XP this order is
    // not guaranteed.
    TerminateFCHandler();

    _ASSERTE((m_hStartEnum  == NULL) && "Make sure all handles (e.g.reg keys) are closed.");
    _ASSERTE(m_hDoneEnum    == NULL);
    _ASSERTE(m_hAuxThread   == NULL);
    _ASSERTE(m_pfnCallback  == NULL);
}

//-----------------------------------------------------------------------------
// Thread callback
//-----------------------------------------------------------------------------
DWORD WINAPI HandlerAuxThreadProc(
    LPVOID lpParameter   // thread data
)
{
    
    IPCFuncCallHandler * pHandler = (IPCFuncCallHandler *) lpParameter;
    HANDLER_CALLBACK pfnCallback = pHandler->m_pfnCallback;
    
    DWORD dwErr = 0;
    DWORD dwWaitRet; 
    
    HANDLE lpHandles[] = {pHandler->m_hShutdownThread, pHandler->m_hStartEnum};
    DWORD dwHandleCount = 2;

    PAL_TRY
    {
    
        do {
            dwWaitRet = WaitForMultipleObjects(dwHandleCount, lpHandles, FALSE /*Wait Any*/, INFINITE);
            dwErr = GetLastError();
    
            // If we are in terminate mode then exit this helper thread.
            if (pHandler->m_fShutdownAuxThread)
                break;
            
            // Keep the 0th index for the terminate thread so that we never miss it
            // in case of multiple events. note that the ShutdownAuxThread flag above it purely 
            // to protect us against some bug in waitForMultipleObjects.
            if ((dwWaitRet-WAIT_OBJECT_0) == 0)
                break;

            // execute callback if wait succeeded
            if ((dwWaitRet-WAIT_OBJECT_0) == 1)
            {           
                (*pfnCallback)();
                            
                // reset manual event
                BOOL fResetOK;
                fResetOK = ResetEvent(pHandler->m_hStartEnum);
                _ASSERTE(fResetOK);                
                dwErr = GetLastError();

                BOOL fSetOK;
                fSetOK = SetEvent(pHandler->m_hDoneEnum);
                _ASSERTE(fSetOK);
                dwErr = GetLastError();
            }
        } while (dwWaitRet != WAIT_FAILED);

    }
    PAL_FINALLY
    {
        if (!SetEvent (pHandler->m_hAuxThreadShutdown))
        {
            dwErr = GetLastError();
            _ASSERTE (!"HandlerAuxTHreadProc: SetEvent(m_hAuxThreadShutdown) failed");
        }

        FreeLibraryAndExitThread (pHandler->m_hCallbackModule, 0);
        // Above call doesn't return
    }
    PAL_ENDTRY

    return 0;
}
 


//-----------------------------------------------------------------------------
// Receieves the call. This should be in a different process than the source
//-----------------------------------------------------------------------------
HRESULT IPCFuncCallHandler::InitFCHandler(HANDLER_CALLBACK pfnCallback)
{
    m_pfnCallback = pfnCallback;

    HRESULT hr = NOERROR;
    DWORD dwThreadId;
    DWORD dwErr = 0;
    DWORD dwDesiredAccess;

    SetLastError(0);

    // Grab the SA
    DWORD dwPid = 0;
    SECURITY_ATTRIBUTES *pSA = NULL;

    dwDesiredAccess = EVENT_ALL_ACCESS;

    dwPid = GetCurrentProcessId();
    hr = IPCShared::CreateWinNTDescriptor(dwPid, FALSE, &pSA);

    if (FAILED(hr))
        goto errExit;;

    // try to open event first (another process may already have created one)
    m_hStartEnum = WszOpenEvent(dwDesiredAccess,
                              FALSE,
                              L"Global\\" StartEnumEventName);
    if (m_hStartEnum == NULL)
    {
        // Create the StartEnum Event
        m_hStartEnum = WszCreateEvent(pSA,
                                        TRUE, // manual event for multiple instances of corperfmonext.dll
                                        FALSE,
                                        FixupName(StartEnumEventName));
    }
    
    if (m_hStartEnum == NULL)
    {
        dwErr = GetLastError();
        hr = HRESULT_FROM_WIN32(dwErr); 
        goto errExit;
    }

    // try to open event first (another process may already have created one)
    m_hDoneEnum = WszOpenEvent(dwDesiredAccess,
                             FALSE,
                             L"Global\\" DoneEnumEventName);

    if (m_hDoneEnum == NULL)
    {
        // Create the EndEnumEvent
        m_hDoneEnum = WszCreateEvent(pSA,
                                        TRUE, // manual event for multiple instances of corperfmonext.dll
                                        FALSE,
                                        FixupName(DoneEnumEventName));
    }
    if (m_hDoneEnum == NULL)
    {
        dwErr = GetLastError();
        hr = HRESULT_FROM_WIN32(dwErr); 
        goto errExit;
    }

    // Create the ShutdownThread Event
    m_hShutdownThread = WszCreateEvent(pSA,
                                       TRUE, /* Manual Reset */
                                       FALSE, /* Initial state not signalled */
                                       NULL);
    
    dwErr = GetLastError();
    if (m_hShutdownThread == NULL)
    {
        hr = HRESULT_FROM_WIN32(dwErr); 
        goto errExit;
    }

    // Create the AuxThreadShutdown Event
    m_hAuxThreadShutdown = WszCreateEvent(pSA,
                                          TRUE, /* Manual Reset */
                                          FALSE,
                                          NULL);

    dwErr = GetLastError();
    if (m_hAuxThreadShutdown == NULL)
    {
        hr = HRESULT_FROM_WIN32(dwErr); 
        goto errExit;
    }

    // The thread that we are about to create should always 
    // find the code in memory. So we take a ref on the DLL. 
    // and do a free library at the end of the thread's start function
    m_hCallbackModule = WszLoadLibrary (L"CorPerfmonExt.dll");

    dwErr = GetLastError();
    if (m_hCallbackModule == NULL)
    {
        hr = HRESULT_FROM_WIN32(dwErr); 
        goto errExit;
    }

    // Create thread
    m_hAuxThread = CreateThread(
        NULL,
        0,
        HandlerAuxThreadProc,
        this,
        0,
        &dwThreadId);

    dwErr = GetLastError();
    if (m_hAuxThread == NULL)
    {
        hr = HRESULT_FROM_WIN32(dwErr);	

        // In case of an error free this library here otherwise
        // the thread's exit would take care of it.
        if (m_hCallbackModule)
            FreeLibrary (m_hCallbackModule);
        goto errExit;
    }

errExit:
    if (!SUCCEEDED(hr)) 
    {
        TerminateFCHandler();
    }
    return hr;
 
}

//-----------------------------------------------------------------------------
// Close all our handles
//-----------------------------------------------------------------------------
void IPCFuncCallHandler::TerminateFCHandler()
{
    if ((m_hStartEnum == NULL) &&
        (m_hDoneEnum == NULL) &&
        (m_hAuxThread == NULL) &&
        (m_pfnCallback == NULL))
    {
        return;
    }

    if(m_hAuxThread != NULL)
    {
        // First make sure that we make the aux thread gracefully exit
        m_fShutdownAuxThread = TRUE;

        // Hope that this set event makes the thread quit.
        if (!SetEvent (m_hShutdownThread))
        {
            _ASSERTE (!"TerminateFCHandler: SetEvent(m_hShutdownThread) failed");
        }
        else
        {
            // Wait for the aux thread to tell us that its not in the callback
            // and is about to terminate
            // wait here till the Aux thread exits
            DWORD AUX_THREAD_WAIT_TIMEOUT = 60 * 1000; // 1 minute

            BOOL doWait = TRUE;
            HANDLE lpHandles[] = {m_hAuxThreadShutdown, m_hAuxThread};
            DWORD dwHandleCount = 2;

            while (doWait)
            {

                DWORD dwWaitRet = WaitForMultipleObjects(dwHandleCount, lpHandles, FALSE /*Wait Any*/, AUX_THREAD_WAIT_TIMEOUT);
                if (dwWaitRet == WAIT_OBJECT_0 || dwWaitRet == (WAIT_OBJECT_0+1))
                {
                    doWait = FALSE;
                    // Not really necessary but cleanup after ourselves
                    ResetEvent(m_hAuxThreadShutdown);
                }
                else if (dwWaitRet == WAIT_TIMEOUT)
                {
                    // Make sure that the aux thread is still alive
                    DWORD dwThreadState = WaitForSingleObject(m_hAuxThread, 0);
                    if ((dwThreadState == WAIT_FAILED) || (dwThreadState == WAIT_OBJECT_0))
                        doWait = FALSE;
                }
                else
                {
                    // We failed for some reason. Bail on the aux thread.
                    _ASSERTE(!"WaitForSingleObject failed while waiting for aux thread");
                    doWait = FALSE;
                }
            }
        }
    }

    if (m_hStartEnum != NULL)
    {
        CloseHandle(m_hStartEnum);
        m_hStartEnum = NULL;
    }

    if (m_hDoneEnum != NULL)
    {
        CloseHandle(m_hDoneEnum);
        m_hDoneEnum = NULL;
    }

    if (m_hAuxThread != NULL)
    {
        CloseHandle(m_hAuxThread);
        m_hAuxThread = NULL;
    }

    if (m_hAuxThreadShutdown != NULL) 
    {
        CloseHandle(m_hAuxThreadShutdown);
        m_hAuxThreadShutdown = NULL;
    }

    if (m_hShutdownThread != NULL)
    {
        CloseHandle(m_hShutdownThread);
        m_hShutdownThread = NULL;
    }

    m_pfnCallback = NULL;
}

