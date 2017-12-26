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
#include "stdafx.h"

#include "unsafe.h"
#include "clrhost.h"
#include "utilcode.h"
#include "ex.h"
#include "hostimpl.h"
#include "contract.h"
#include "tls.h"
#include "memoryreport.h"

struct CantAllocThread
{
    PVOID m_fiberId;
    LONG  m_CantCount;
    CantAllocThread()
    {
        m_fiberId = 0;
        m_CantCount = 0;
    }
};

class CantAllocThreads
{
    static const int MaxThreadNum=100;
    CantAllocThread m_threads[100];
    static volatile LONG g_CantAllocStressLogCount;
public:
    void MarkThread();
    void UnmarkThread();
    BOOL IsInCantAllocStressLogRegion();

};

CantAllocThreads g_CantAllocThreads;
volatile LONG CantAllocThreads::g_CantAllocStressLogCount = 0;

void CantAllocThreads::MarkThread()
{
    size_t count = 0;
    if (ClrFlsCheckValue(TlsIdx_CantAllocCount, (LPVOID *)&count))
    {
        _ASSERTE (count >= 0);
        ClrFlsSetValue(TlsIdx_CantAllocCount,  (LPVOID)(count+1));
        return;
    }
    PVOID fiberId = ClrTeb::GetFiberPtrId();
    for (int i = 0; i < MaxThreadNum; i ++)
    {
        if (m_threads[i].m_fiberId == fiberId)
        {
            m_threads[i].m_CantCount ++;
            return;
        }
    }
    for (int i = 0; i < MaxThreadNum; i ++)
    {
        if (m_threads[i].m_fiberId == NULL)
        {
            if (InterlockedCompareExchangePointer(&m_threads[i].m_fiberId, fiberId, NULL) == NULL)
            {
                _ASSERTE(m_threads[i].m_CantCount == 0);
                m_threads[i].m_CantCount = 1;
                return;
            }
        }
    }
    count = InterlockedIncrement (&g_CantAllocStressLogCount);
    _ASSERTE (count >= 1);
    return;

}

void CantAllocThreads::UnmarkThread()
{
    size_t count = 0;
    if (ClrFlsCheckValue(TlsIdx_CantAllocCount, (LPVOID *)&count))
    {
        if (count > 0)
        {
            ClrFlsSetValue(TlsIdx_CantAllocCount,  (LPVOID)(count-1));
            return;
        }
    }
    PVOID fiberId = ClrTeb::GetFiberPtrId();
    for (int i = 0; i < MaxThreadNum; i ++)
    {
        if (m_threads[i].m_fiberId == fiberId)
        {
            _ASSERTE (m_threads[i].m_CantCount > 0);
            m_threads[i].m_CantCount --;
            if (m_threads[i].m_CantCount == 0)
            {
                m_threads[i].m_fiberId = NULL;
            }
            return;
        }
    }
    _ASSERTE (g_CantAllocStressLogCount > 0);
    InterlockedDecrement (&g_CantAllocStressLogCount);
    return;
}

BOOL CantAllocThreads::IsInCantAllocStressLogRegion()
{
    size_t count = 0;
    if (ClrFlsCheckValue(TlsIdx_CantAllocCount, (LPVOID *)&count))
    {
        if (count > 0)
        {
            return true;
        }
    }
    PVOID fiberId = ClrTeb::GetFiberPtrId();
    for (int i = 0; i < MaxThreadNum; i ++)
    {
        if (m_threads[i].m_fiberId == fiberId)
        {
            _ASSERTE (m_threads[i].m_CantCount > 0);
            return true;
        }
    }

    return g_CantAllocStressLogCount > 0;
}

// In some cirumstance (e.g, the thread suspecd another thread), allocation on heap
// could cause dead lock. We use a counter in TLS to indicate the current thread is not allowed
// to do heap allocation.
//In cases where CLRTlsInfo doesn't exist and we still want to track CantAlloc info (this is important to 
//stress log), We use a global counter. This introduces the problem where one thread could disable allocation 
//for another thread, but the cases should be rare (we limit the use to stress log for now) and the period 
//should (MUST) be short
//only stress log check this counter

void IncCantAllocCount()
{
    g_CantAllocThreads.MarkThread();
}

void DecCantAllocCount()
{
    g_CantAllocThreads.UnmarkThread();
}

// for stress log the rule is more restrict, we have to check the global counter too
BOOL IsInCantAllocStressLogRegion()
{   
    return g_CantAllocThreads.IsInCantAllocStressLogRegion();
}

static DWORD g_MasterSlotIndex = TLS_OUT_OF_INDEXES;





#ifdef _DEBUG_IMPL

//
//
static BOOL dbg_fDisableThrowCheck = FALSE;

void DisableThrowCheck()
{
    LEAF_CONTRACT;

    dbg_fDisableThrowCheck = TRUE;
}



#endif // #ifdef _DEBUG_IMPL

LPVOID ClrAllocInProcessHeapBootstrap (DWORD dwFlags, SIZE_T dwBytes)
{
    STATIC_CONTRACT_SO_INTOLERANT;


    static HANDLE hHeap = NULL;

    // This could race, but the result would be that this
    // variable gets double initialized.
    if (hHeap == NULL)
        hHeap = ClrGetProcessHeap();

    return ClrHeapAlloc(hHeap, dwFlags,dwBytes);
}
FastAllocInProcessHeapFunc __ClrAllocInProcessHeap = (FastAllocInProcessHeapFunc) ClrAllocInProcessHeapBootstrap;

BOOL ClrFreeInProcessHeapBootstrap (DWORD dwFlags, LPVOID lpMem)
{

    STATIC_CONTRACT_SO_INTOLERANT;

    static HANDLE hHeap = NULL;

    // This could race, but the result would be that this
    // variable gets double initialized.
    if (hHeap == NULL)
        hHeap = ClrGetProcessHeap();

    return ClrHeapFree(hHeap, dwFlags,lpMem);
}
FastFreeInProcessHeapFunc __ClrFreeInProcessHeap = (FastFreeInProcessHeapFunc) ClrFreeInProcessHeapBootstrap;


#if !defined(__GNUC__) || __GNUC__ >= 3
const NoThrow nothrow = { 0 };
#endif

void * __cdecl operator new(size_t n, const NoThrow&) {
  SAVE_LASTERROR;
  STATIC_CONTRACT_NOTHROW;
  STATIC_CONTRACT_FAULT;

#ifndef _MSC_VER
  void * result = NULL;

  PAL_CPP_TRY
  {
      result = ::operator new (n);
  }
  PAL_CPP_CATCH_ALL
  {
      result = NULL;
  }
  PAL_CPP_ENDTRY

  return result;
#else // _MSC_VER
  // MSVC's CRT new returns NULL on failure instead of throwing!
  return ::operator new (n);
#endif // _MSC_VER
}

void * __cdecl operator new[](size_t n, const NoThrow&) {
  SAVE_LASTERROR;
  STATIC_CONTRACT_NOTHROW;
  STATIC_CONTRACT_FAULT;

#ifndef _MSC_VER
  void * result = NULL;

  PAL_CPP_TRY
  {
      result = ::operator new[] (n);
  }
  PAL_CPP_CATCH_ALL
  {
      result = NULL;
  }
  PAL_CPP_ENDTRY

  return result;
#else // _MSC_VER
  // MSVC's CRT doesn't have a vector new operator!
  return ::operator new (n);
#endif // _MSC_VER
}



/* ------------------------------------------------------------------------ *
 * New operator overloading for the executable heap
 * ------------------------------------------------------------------------ */

const CExecutable executable = { 0 };

void * __cdecl operator new(size_t n, const CExecutable&)
{
    SAVE_LASTERROR;


    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;    // The memory management routines should be SO-tolerant.

    void * result = ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, n);
    if (result == NULL) {
        ThrowOutOfMemory();
    }
    return result;
}

void * __cdecl operator new[](size_t n, const CExecutable&)
{
    SAVE_LASTERROR;


    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;    // The memory management routines should be SO-tolerant.

    void * result = ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, n);
    if (result == NULL) {
        ThrowOutOfMemory();
    }
    return result;
}

void * __cdecl operator new(size_t n, const CExecutable&, const NoThrow&)
{
    SAVE_LASTERROR;

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;    // The memory management routines should be SO-tolerant.

    return ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, n);
}

void * __cdecl operator new[](size_t n, const CExecutable&, const NoThrow&)
{
    SAVE_LASTERROR;

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;    // The memory management routines should be SO-tolerant.

    return ClrHeapAlloc(ClrGetProcessExecutableHeap(), 0, n);
}

#ifdef _DEBUG

// This is a DEBUG routing to verify that a memory region complies with executable requirements
BOOL DbgIsExecutable(LPVOID lpMem, SIZE_T length)
{
    return TRUE;
}

#endif // _DEBUG




// Access various ExecutionEngine support services, like a logical TLS that abstracts
// fiber vs. thread issues.  We obtain it from a DLL export via the shim.

typedef IExecutionEngine * (__stdcall * IEE_FPTR) ();
typedef HRESULT (STDAPICALLTYPE *PLoadLibraryShim)(LPCWSTR szDllName, LPCWSTR szVersion, LPVOID pvReserved, HMODULE *phModDll);

#ifdef _DEBUG_IMPL
static int g_dbgIEEInInit = 0;
BOOL IsInIEEInit()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_SO_TOLERANT;


    return g_dbgIEEInInit > 0;
}

void BeginIEEInit()
{
    LEAF_CONTRACT;

    g_dbgIEEInInit ++;
}
void EndIEEInit()
{
    LEAF_CONTRACT;

    g_dbgIEEInInit --;
    _ASSERTE (g_dbgIEEInInit >= 0);
}
#endif

//
// Access various ExecutionEngine support services, like a logical TLS that abstracts
// fiber vs. thread issues.
// From an IExecutionEngine is possible to get other services via QueryInterfaces such
// as memory management
//
IExecutionEngine *g_pExecutionEngine = NULL;

#ifdef SELF_NO_HOST
BYTE g_ExecutionEngineInstance[sizeof(UtilExecutionEngine)];
#endif

IExecutionEngine *GetExecutionEngine()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    if (g_pExecutionEngine == NULL)
    {
#ifdef SELF_NO_HOST
        // Create a local copy on the stack and then copy it over to the static instance.
        // This avoids race conditions caused by multiple initializations of vtable in the constructor
        UtilExecutionEngine local;
        memcpy(&g_ExecutionEngineInstance, &local, sizeof(UtilExecutionEngine));

        g_pExecutionEngine = (IExecutionEngine*)(UtilExecutionEngine*)&g_ExecutionEngineInstance;
#else
#ifdef _DEBUG
        BeginIEEInit();
#endif
        // We may not have gone through "UseUnicodeAPI" yet, so
        OnUnicodeSystem();

        HMODULE hMod;

        {
            // It's a bug to ask for the ExecutionEngine interface in scenarios where the
            // ExecutionEngine cannot be loaded.
            SCAN_IGNORE_FAULT;

            hMod = WszLoadLibrary(MSCOREE_SHIM_W);
        }

        if (hMod != 0)
        {
            IEE_FPTR fptr = (IEE_FPTR) GetProcAddress(hMod, "IEE");
            if (fptr != 0)
                g_pExecutionEngine = (*fptr)();
            else
            {



                PLoadLibraryShim pfnLoadLibraryShim = (PLoadLibraryShim)GetProcAddress(hMod, "LoadLibraryShim");

                _ASSERTE(pfnLoadLibraryShim != NULL);
                if (pfnLoadLibraryShim != NULL)
                {
                    HMODULE hmod = NULL;

                    if (SUCCEEDED(pfnLoadLibraryShim(L"mscorwks.dll", NULL, NULL, &hmod)) && hmod != NULL)
                    {
                        fptr = (IEE_FPTR) GetProcAddress(hmod, "IEE");
                        if (fptr != 0)
                            g_pExecutionEngine = (*fptr)();
                    }
                }
            }
        }
        // And don't balance with a FreeLibrary
#ifdef _DEBUG
        EndIEEInit();
#endif
#endif  // SELF_NO_HOST
    }

    // It's a bug to ask for the ExecutionEngine interface in scenarios where the
    // ExecutionEngine cannot be loaded.
    _ASSERTE(g_pExecutionEngine);
    return g_pExecutionEngine;
} // IExecutionEngine *GetExecutionEngine()

IEEMemoryManager *GetEEMemoryManager()
{
    STATIC_CONTRACT_NOTHROW;

    static IEEMemoryManager *pEEMemoryManager = NULL;
    if (NULL == pEEMemoryManager) {
        IExecutionEngine *pExecutionEngine = GetExecutionEngine();
        _ASSERTE(pExecutionEngine);

        // It is dangerous to pass a global pointer to QueryInterface.  The pointer may be set
        // to NULL in the call.  Imagine that thread 1 calls QI, and get a pointer.  But before thread 1
        // returns the pointer to caller, thread 2 calls QI and the pointer is set to NULL.
        IEEMemoryManager *pEEMM;
        pExecutionEngine->QueryInterface(IID_IEEMemoryManager, (void**)&pEEMM);
        pEEMemoryManager = pEEMM;
    }
    // It's a bug to ask for the MemoryManager interface in scenarios where it cannot be loaded.
    _ASSERTE(pEEMemoryManager);
    return pEEMemoryManager;
}

// should return some error code or exception
void SetExecutionEngine(IExecutionEngine *pEE)
{
    STATIC_CONTRACT_NOTHROW;

    _ASSERTE(pEE && !g_pExecutionEngine);
    if (!g_pExecutionEngine) {
        g_pExecutionEngine = pEE;
        g_pExecutionEngine->AddRef();
    }
}

void ClrFlsAssociateCallback(DWORD slot, PTLS_CALLBACK_FUNCTION callback)
{
    WRAPPER_CONTRACT;

    GetExecutionEngine()->TLS_AssociateCallback(slot, callback);
}

extern void * __stdcall ClrFlsGetBlockGeneric();
extern void * __stdcall ClrFlsGetBlockBootstrap();

void * __stdcall ClrFlsGetBlockGeneric()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    return UnsafeTlsGetValue(g_MasterSlotIndex);
}

void * __stdcall ClrFlsGetBlockBootstrap()
{
    STATIC_CONTRACT_SO_INTOLERANT;

    OnUnicodeSystem();

    // Initialize the index
    g_MasterSlotIndex = GetExecutionEngine()->TLS_GetMasterSlotIndex();

    // Use the generic getter for any recursive Fls needs.
    __ClrFlsGetBlock = (POPTIMIZEDTLSGETTER)ClrFlsGetBlockGeneric;

    // Now build a nice fast one.
    POPTIMIZEDTLSGETTER clrFlsGetBlock = MakeOptimizedTlsGetter(g_MasterSlotIndex);
    if (clrFlsGetBlock)
        __ClrFlsGetBlock = clrFlsGetBlock;

    // Return the result using the new getter
    return (*__ClrFlsGetBlock)();
}

POPTIMIZEDTLSGETTER __ClrFlsGetBlock = (POPTIMIZEDTLSGETTER)ClrFlsGetBlockBootstrap;

CRITSEC_COOKIE ClrCreateCriticalSection(LPCSTR szTag, CrstLevel level, CrstFlags flags)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->CreateLock(szTag, level, flags);
}

HRESULT ClrDeleteCriticalSection(CRITSEC_COOKIE cookie)
{
    WRAPPER_CONTRACT;
    GetExecutionEngine()->DestroyLock(cookie);
    return S_OK;
}

void ClrEnterCriticalSection(CRITSEC_COOKIE cookie)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->AcquireLock(cookie);
}

void ClrLeaveCriticalSection(CRITSEC_COOKIE cookie)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ReleaseLock(cookie);
}

EVENT_COOKIE ClrCreateAutoEvent(BOOL bInitialState)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->CreateAutoEvent(bInitialState);
}

EVENT_COOKIE ClrCreateManualEvent(BOOL bInitialState)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->CreateManualEvent(bInitialState);
}

void ClrCloseEvent(EVENT_COOKIE event)
{
    WRAPPER_CONTRACT;

    GetExecutionEngine()->CloseEvent(event);
}

BOOL ClrSetEvent(EVENT_COOKIE event)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrSetEvent(event);
}

BOOL ClrResetEvent(EVENT_COOKIE event)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrResetEvent(event);
}

DWORD ClrWaitEvent(EVENT_COOKIE event, DWORD dwMilliseconds, BOOL bAlertable)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->WaitForEvent(event, dwMilliseconds, bAlertable);
}

SEMAPHORE_COOKIE ClrCreateSemaphore(DWORD dwInitial, DWORD dwMax)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrCreateSemaphore(dwInitial, dwMax);
}

void ClrCloseSemaphore(SEMAPHORE_COOKIE semaphore)
{
    WRAPPER_CONTRACT;

    GetExecutionEngine()->ClrCloseSemaphore(semaphore);
}

BOOL ClrReleaseSemaphore(SEMAPHORE_COOKIE semaphore, LONG lReleaseCount, LONG *lpPreviousCount)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrReleaseSemaphore(semaphore, lReleaseCount, lpPreviousCount);
}

DWORD ClrWaitSemaphore(SEMAPHORE_COOKIE semaphore, DWORD dwMilliseconds, BOOL bAlertable)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrWaitForSemaphore(semaphore, dwMilliseconds, bAlertable);
}

MUTEX_COOKIE ClrCreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,
                            BOOL bInitialOwner,
                            LPCTSTR lpName)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrCreateMutex(lpMutexAttributes, bInitialOwner, lpName);
}

void ClrCloseMutex(MUTEX_COOKIE mutex)
{
    WRAPPER_CONTRACT;

    GetExecutionEngine()->ClrCloseMutex(mutex);
}

BOOL ClrReleaseMutex(MUTEX_COOKIE mutex)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrReleaseMutex(mutex);
}

DWORD ClrWaitForMutex(MUTEX_COOKIE mutex, DWORD dwMilliseconds, BOOL bAlertable)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrWaitForMutex(mutex, dwMilliseconds, bAlertable);
}

DWORD ClrSleepEx(DWORD dwMilliseconds, BOOL bAlertable)
{
    WRAPPER_CONTRACT;

    return GetExecutionEngine()->ClrSleepEx(dwMilliseconds, bAlertable);
}

LPVOID ClrVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    WRAPPER_CONTRACT;

    LPVOID result =  GetEEMemoryManager()->ClrVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);


    return result;
}

BOOL ClrVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    WRAPPER_CONTRACT;


    BOOL result = GetEEMemoryManager()->ClrVirtualFree(lpAddress, dwSize, dwFreeType);

    return result;
}

SIZE_T ClrVirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
    WRAPPER_CONTRACT;


    return GetEEMemoryManager()->ClrVirtualQuery(lpAddress, lpBuffer, dwLength);
}

BOOL ClrVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    WRAPPER_CONTRACT;


    return GetEEMemoryManager()->ClrVirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
}

HANDLE ClrGetProcessHeap()
{
    WRAPPER_CONTRACT;

    return GetEEMemoryManager()->ClrGetProcessHeap();
}

HANDLE ClrHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
{
    WRAPPER_CONTRACT;

    return GetEEMemoryManager()->ClrHeapCreate(flOptions, dwInitialSize, dwMaximumSize);
}

BOOL ClrHeapDestroy(HANDLE hHeap)
{
    WRAPPER_CONTRACT;

    return GetEEMemoryManager()->ClrHeapDestroy(hHeap);
}

LPVOID ClrHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    WRAPPER_CONTRACT;

    LPVOID result = GetEEMemoryManager()->ClrHeapAlloc(hHeap, dwFlags, dwBytes);

    return result;
}

BOOL ClrHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
    WRAPPER_CONTRACT;

    BOOL result = GetEEMemoryManager()->ClrHeapFree(hHeap, dwFlags, lpMem);

    return result;
}

BOOL ClrHeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
    WRAPPER_CONTRACT;

    return GetEEMemoryManager()->ClrHeapValidate(hHeap, dwFlags, lpMem);
}

HANDLE ClrGetProcessExecutableHeap()
{
    WRAPPER_CONTRACT;

    return GetEEMemoryManager()->ClrGetProcessExecutableHeap();
}

void GetLastThrownObjectExceptionFromThread(void **ppvException)
{
    WRAPPER_CONTRACT;

    GetExecutionEngine()->GetLastThrownObjectExceptionFromThread(ppvException);
} // void GetLastThrownObjectExceptionFromThread()

