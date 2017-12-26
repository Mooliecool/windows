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
// ===========================================================================
// File: CEEMAIN.H
//

// CEEMAIN.H defines the entrypoints into the Virtual Execution Engine and
// gets the load/run process going.
// ===========================================================================
#ifndef CEEMain_H
#define CEEMain_H

#include <windef.h> // for HFILE, HANDLE, HMODULE

class EEDbgInterfaceImpl;

// Ensure the EE is started up.
HRESULT EnsureEEStarted(COINITIEE flags);

// Force shutdown of the EE
void ForceEEShutdown();
void InnerCoEEShutDownCOM();

// We have an internal class that can be used to expose EE functionality to other CLR
// DLLs, via the deliberately obscure IEE DLL exports from the shim and the EE
class CExecutionEngine : public IExecutionEngine, public IEEMemoryManager
{
    //***************************************************************************
    // public API:
    //***************************************************************************
public:

    // Notification of a DLL_THREAD_DETACH or a Thread Terminate.
    static void ThreadDetaching(void **pTlsData);

    // Delete on TLS block
    static void DeleteTLS(void **pTlsData);

    // Fiber switch notifications
    static void SwitchIn();
    static void SwitchOut();

    static void **CheckThreadState(DWORD slot, BOOL force = TRUE);
    static void **CheckThreadStateNoCreate(DWORD slot);

    // Setup FLS simulation block, including ClrDebugState and StressLog.
    static void SetupTLSForThread(Thread *pThread);

    static DWORD GetTlsIndex () {return TlsIndex;}

    static BOOL HasDetachedTlsInfo();

    static void CleanupDetachedTlsInfo();

    static void DetachTlsInfo(void **pTlsData);

    //***************************************************************************
    // private implementation:
    //***************************************************************************
private:

    // The debugger needs access to the TlsIndex so that we can read it from OOP.
    friend class EEDbgInterfaceImpl;

    SVAL_DECL (DWORD, TlsIndex);

    static PTLS_CALLBACK_FUNCTION Callbacks[MAX_PREDEFINED_TLS_SLOT];


    //***************************************************************************
    // IUnknown methods
    //***************************************************************************

    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID id,
            void **pInterface);

    ULONG STDMETHODCALLTYPE AddRef();

    ULONG STDMETHODCALLTYPE Release();

    //***************************************************************************
    // IExecutionEngine methods for TLS
    //***************************************************************************

    // Associate a callback for cleanup with a TLS slot
    VOID  STDMETHODCALLTYPE TLS_AssociateCallback(
            DWORD slot,
            PTLS_CALLBACK_FUNCTION callback);

    // May be called once to get the master TLS block slot for fast Get/Set operations
    DWORD STDMETHODCALLTYPE TLS_GetMasterSlotIndex();

    // Get the value at a slot
    LPVOID STDMETHODCALLTYPE TLS_GetValue(DWORD slot);

    // Get the value at a slot, return FALSE if TLS info block doesn't exist
    BOOL STDMETHODCALLTYPE TLS_CheckValue(DWORD slot, LPVOID * pValue);

    // Set the value at a slot
    VOID STDMETHODCALLTYPE TLS_SetValue(DWORD slot, LPVOID pData);

    // Free TLS memory block and make callback
    VOID STDMETHODCALLTYPE TLS_ThreadDetaching();
    
    //***************************************************************************
    // IExecutionEngine methods for locking
    //***************************************************************************

    CRITSEC_COOKIE STDMETHODCALLTYPE CreateLock(LPCSTR szTag, LPCSTR level, CrstFlags flags);

    void STDMETHODCALLTYPE DestroyLock(CRITSEC_COOKIE lock);

    void STDMETHODCALLTYPE AcquireLock(CRITSEC_COOKIE lock);

    void STDMETHODCALLTYPE ReleaseLock(CRITSEC_COOKIE lock);

    EVENT_COOKIE STDMETHODCALLTYPE CreateAutoEvent(BOOL bInitialState);
    EVENT_COOKIE STDMETHODCALLTYPE CreateManualEvent(BOOL bInitialState);
    void STDMETHODCALLTYPE CloseEvent(EVENT_COOKIE event);
    BOOL STDMETHODCALLTYPE ClrSetEvent(EVENT_COOKIE event);
    BOOL STDMETHODCALLTYPE ClrResetEvent(EVENT_COOKIE event);
    DWORD STDMETHODCALLTYPE WaitForEvent(EVENT_COOKIE event, DWORD dwMilliseconds, BOOL bAlertable);
    DWORD STDMETHODCALLTYPE WaitForSingleObject(HANDLE handle, DWORD dwMilliseconds);

    SEMAPHORE_COOKIE STDMETHODCALLTYPE ClrCreateSemaphore(DWORD dwInitial, DWORD dwMax);
    void STDMETHODCALLTYPE ClrCloseSemaphore(SEMAPHORE_COOKIE semaphore);
    DWORD STDMETHODCALLTYPE ClrWaitForSemaphore(SEMAPHORE_COOKIE semaphore, DWORD dwMilliseconds, BOOL bAlertable);
    BOOL STDMETHODCALLTYPE ClrReleaseSemaphore(SEMAPHORE_COOKIE semaphore, LONG lReleaseCount, LONG *lpPreviousCount);

    MUTEX_COOKIE STDMETHODCALLTYPE ClrCreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
                                                  BOOL bInitialOwner,
                                                  LPCTSTR lpName);
    void STDMETHODCALLTYPE ClrCloseMutex(MUTEX_COOKIE mutex);
    BOOL STDMETHODCALLTYPE ClrReleaseMutex(MUTEX_COOKIE mutex);
    DWORD STDMETHODCALLTYPE ClrWaitForMutex(MUTEX_COOKIE mutex,
                                            DWORD dwMilliseconds,
                                            BOOL bAlertable);

    DWORD STDMETHODCALLTYPE ClrSleepEx(DWORD dwMilliseconds, BOOL bAlertable);

    BOOL STDMETHODCALLTYPE ClrAllocationDisallowed();

    void STDMETHODCALLTYPE GetLastThrownObjectExceptionFromThread(void **ppvException);

    //***************************************************************************
    // IEEMemoryManager methods for locking
    //***************************************************************************
    LPVOID STDMETHODCALLTYPE ClrVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
    BOOL STDMETHODCALLTYPE ClrVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
    SIZE_T STDMETHODCALLTYPE ClrVirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
    BOOL STDMETHODCALLTYPE ClrVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
    HANDLE STDMETHODCALLTYPE ClrGetProcessHeap();
    HANDLE STDMETHODCALLTYPE ClrHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
    BOOL STDMETHODCALLTYPE ClrHeapDestroy(HANDLE hHeap);
    LPVOID STDMETHODCALLTYPE ClrHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
    BOOL STDMETHODCALLTYPE ClrHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
    BOOL STDMETHODCALLTYPE ClrHeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
    HANDLE STDMETHODCALLTYPE ClrGetProcessExecutableHeap();
    
};

#ifdef _DEBUG
extern void DisableGlobalAllocStore ();
#endif //_DEBUG 

#endif
