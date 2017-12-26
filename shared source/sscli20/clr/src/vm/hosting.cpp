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

#include "common.h"

#include "hosting.h"
#include "mscoree.h"
#include "corhost.h"
#include "threads.h"


#define countof(x) (sizeof(x) / sizeof(x[0]))

//
// memory management functions
//

// global debug only tracking utilities
#ifdef _DEBUG

static const LONG MaxGlobalAllocCount = 8;

class GlobalAllocStore {
public:
    static void AddAlloc (LPVOID p)
    {
        LEAF_CONTRACT;

        if (!p) {
            return;
        }
        if (m_Disabled) {
            return;
        }

        //InterlockedIncrement (&numMemWriter);
        //if (CheckMemFree) {
        //    goto Return;
        //}

        //m_Count is number of allocation we've ever tried, it's OK to be bigger than
        //size of m_Alloc[]
        InterlockedIncrement (&m_Count);

        //this is by no means an accurate record of heap allocation.
        //the algorithm used here can't guarantee an allocation is saved in
        //m_Alloc[] even there's enough free space. However this is only used
        //for debugging purpose and most importantly, m_Count is accurate.
        for (size_t n = 0; n < countof(m_Alloc); n ++) {
            if (m_Alloc[n] == 0) {
                if (InterlockedCompareExchangePointer(&m_Alloc[n],p,0) == 0) {
                    return;
                }
            }
        }
        
        //InterlockedDecrement (&numMemWriter);
    }

    //this is called in non-host case where we don't care the free after
    //alloc store is disabled
    static BOOL RemoveAlloc (LPVOID p)
    {
        LEAF_CONTRACT;

        if (m_Disabled)
        {
            return TRUE;
        }
        //decrement the counter even we might not find the allocation
        //in m_Alloc. Because it's possible for an allocation not to be saved
        //in the array
        InterlockedDecrement (&m_Count);
        // Binary search        
        for (size_t n = 0; n < countof(m_Alloc); n ++) {
            if (m_Alloc[n] == p) {
                m_Alloc[n] = 0;
                return TRUE;
            }
        }
        return FALSE;
    }

    //this is called in host case where if the store is disabled, we want to 
    //guarantee we don't try to free anything the host doesn't know about
    static void ValidateFree(LPVOID p)
    {
        LEAF_CONTRACT;

        if (p == 0) {
            return;
        }
        if (m_Disabled) {
            for (size_t n = 0; n < countof(m_Alloc); n ++) {
                //there could be miss, because an allocation might not be saved
                //in the array
                if (m_Alloc[n] == p) {
                    _ASSERTE (!"Free a memory that host interface does not know");
                    return;
                }
            }
        }
    }

    static void Validate()
    {
        LEAF_CONTRACT;

        if (m_Count > MaxGlobalAllocCount) {
            _ASSERTE (!"Using too many memory allocator before Host Interface is set up");
        }       
        
        //while (numMemWriter != 0) {
        //    Sleep(5);
        //}
        //qsort (GlobalMemAddr, (MemAllocCount>MaxAllocCount)?MaxAllocCount:MemAllocCount, sizeof(LPVOID), MemAddrCompare);
    }

    static void Disable ()
    {
        LEAF_CONTRACT;
        if (!m_Disabled) 
        {
            // Let all threads know
            InterlockedIncrement((LONG*)&m_Disabled);
        }
    }

private:
    static BOOL m_Disabled;    
    static LPVOID m_Alloc[MaxGlobalAllocCount];
    //m_Count is number of allocation we tried, it's legal to be bigger than
    //size of m_Alloc[]
    static LONG m_Count;
    // static LONG numMemWriter = 0;
};

// used from corhost.cpp
void ValidateHostInterface()
{
    WRAPPER_CONTRACT;

    GlobalAllocStore::Validate();
    GlobalAllocStore::Disable();    
}

void DisableGlobalAllocStore ()
{
    WRAPPER_CONTRACT;
    GlobalAllocStore::Disable();
}
LPVOID GlobalAllocStore::m_Alloc[MaxGlobalAllocCount];
LONG GlobalAllocStore::m_Count = 0;
BOOL GlobalAllocStore::m_Disabled = FALSE;

#endif

#ifdef _DEBUG
// The helper thread can't call regular new / delete b/c of interop-debugging deadlocks.
// It must use the (InteropSafe) heap from debugger.h, you also can't allocate normally
// when we have any other thread hard-suspended.

void AssertAllocationAllowed();
#endif



#undef VirtualAlloc
LPVOID EEVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


#ifdef _DEBUG
    AssertAllocationAllowed();
#endif

#ifdef _DEBUG
        if (g_fEEStarted && !IsInIEEInit()) {
            _ASSERTE (!EEAllocationDisallowed());
        }
#endif

    IHostMemoryManager *pMM = CorHost2::GetHostMemoryManager();
    if (pMM) {
        LPVOID pMem;
        EMemoryCriticalLevel eLevel = eTaskCritical;
        if (!g_fEEStarted)
        {
            eLevel = eProcessCritical;
        }
        else
        {
            Thread *pThread = GetThread();
            if (pThread && pThread->HasLockInCurrentDomain())
            {
                if (GetAppDomain()->IsDefaultDomain())
                {
                    eLevel = eProcessCritical;
                }
                else
                {
                    eLevel = eAppDomainCritical;
                }
            }
        }
        HRESULT hr = S_OK;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pMM->VirtualAlloc (lpAddress, dwSize, flAllocationType, flProtect, eLevel, &pMem);
        END_SO_TOLERANT_CODE_CALLING_HOST;

        if ((hr == S_OK)  && (flAllocationType & MEM_COMMIT))
             MEMORY_REPORT_ALLOCATE(pMem, dwSize, MemoryReport::VIRTUAL_ALLOCATED_COMMIT, 1 /* callers */);

        if(hr != S_OK)
        {
            STRESS_LOG_OOM_STACK(dwSize);
        }

        return (hr == S_OK) ? pMem : NULL;
    }
    else {

        LPVOID p = NULL;

#ifdef _DEBUG
        if (lpAddress == NULL && (flAllocationType & MEM_RESERVE) != 0 && PEDecoder::GetForceRelocs())
        {
            // Allocate memory top to bottom to stress ngen fixups with LARGEADDRESSAWARE support.
            p = ::VirtualAlloc(lpAddress, dwSize, flAllocationType | MEM_TOP_DOWN, flProtect);
        }
#endif // _DEBUG

        // Fall back to the default method if the forced relocation failed
        if (p == NULL)
        {
            p = ::VirtualAlloc (lpAddress, dwSize, flAllocationType, flProtect);
        }

#ifdef _DEBUG
        GlobalAllocStore::AddAlloc (p);
#endif

       if ((p != NULL)  && (flAllocationType & MEM_COMMIT))
            MEMORY_REPORT_ALLOCATE(p, dwSize, MemoryReport::VIRTUAL_ALLOCATED_COMMIT, 1 /* callers */);

        if(p == NULL){
             STRESS_LOG_OOM_STACK(dwSize);
        }

        return p;
    }

}
#define VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect) Dont_Use_VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect)

#undef VirtualFree
BOOL EEVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL retVal = FALSE;

    IHostMemoryManager *pMM = CorHost2::GetHostMemoryManager();
    if (pMM) {
#ifdef _DEBUG
        GlobalAllocStore::ValidateFree(lpAddress);
#endif

        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        retVal = pMM->VirtualFree (lpAddress, dwSize, dwFreeType) == S_OK;
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    else {
#ifdef _DEBUG
        GlobalAllocStore::RemoveAlloc (lpAddress);
#endif

        retVal = (BOOL)(BYTE)::VirtualFree (lpAddress, dwSize, dwFreeType);
    }
    if (retVal)
        MEMORY_REPORT_FREE_RANGE(lpAddress, dwSize, MemoryReport::VIRTUAL_ALLOCATED_COMMIT /* hack use dwFreeType */);

    return retVal;
}
#define VirtualFree(lpAddress, dwSize, dwFreeType) Dont_Use_VirtualFree(lpAddress, dwSize, dwFreeType)

#undef VirtualQuery
SIZE_T EEVirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IHostMemoryManager *pMM = CorHost2::GetHostMemoryManager();
    if (pMM) {
        SIZE_T result;
        HRESULT hr = S_OK;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pMM->VirtualQuery((void*)lpAddress, lpBuffer, dwLength, &result);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
            return 0;
        return result;
    }
    else {
        return ::VirtualQuery(lpAddress, lpBuffer, dwLength);
    }
}
#define VirtualQuery(lpAddress, lpBuffer, dwLength) Dont_Use_VirtualQuery(lpAddress, lpBuffer, dwLength)

#undef VirtualProtect
BOOL EEVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    IHostMemoryManager *pMM = CorHost2::GetHostMemoryManager();
    if (pMM) {
        BOOL result = FALSE;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        result = pMM->VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect) == S_OK;
        END_SO_TOLERANT_CODE_CALLING_HOST;
        return result;
    }
    else {
        return ::VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
    }
}
#define VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect) Dont_Use_VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect)

#undef GetProcessHeap
HANDLE EEGetProcessHeap() {
    // Note: this can be called a little early for real contracts, so we use static contracts instead.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    IHostMemoryManager *pMM = CorHost2::GetHostMemoryManager();
    if (pMM) {
        return (HANDLE)1; // pretending we return an handle is ok because handles are ignored by the hosting api
    }
    else {
        return GetProcessHeap();
    }
}
#define GetProcessHeap() Dont_Use_GetProcessHeap()

#undef HeapCreate
HANDLE EEHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    return NULL;
}
#define HeapCreate(flOptions, dwInitialSize, dwMaximumSize) Dont_Use_HeapCreate(flOptions, dwInitialSize, dwMaximumSize)

#undef HeapDestroy
BOOL EEHeapDestroy(HANDLE hHeap) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    return TRUE;
}
#define HeapDestroy(hHeap) Dont_Use_HeapDestroy(hHeap)

#ifdef _DEBUG
#ifdef _X86_
#define OS_HEAP_ALIGN 8
#else
#define OS_HEAP_ALIGN 16
#endif
#endif

#undef HeapAlloc
LPVOID EEHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_SO_INTOLERANT;


#ifdef _DEBUG
    AssertAllocationAllowed();
#endif

    BOOL fExecute = FALSE;
    if (hHeap == NULL)
    {
        hHeap = EEGetProcessHeap();
        fExecute = TRUE;
    }

    IHostMalloc *pHM = CorHost2::GetHostMalloc();
    if (pHM
        )
    {
        EMemoryCriticalLevel eLevel = eTaskCritical;
        if (!g_fEEStarted)
        {
            eLevel = eProcessCritical;
        }
        else
        {
            Thread *pThread = GetThread();
            if (pThread && pThread->HasLockInCurrentDomain())
            {
                if (GetAppDomain()->IsDefaultDomain())
                {
                    eLevel = eProcessCritical;
                }
                else
                {
                    eLevel = eAppDomainCritical;
                }
            }
        }
        LPVOID pMem = NULL;
        HRESULT hr = S_OK;
        {
            CantAllocHolder caHolder;
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            hr = pHM->Alloc(dwBytes, eLevel, &pMem);
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }
        if (hr == S_OK)
             MEMORY_REPORT_ALLOCATE(pMem, dwBytes, MemoryReport::PROCESS_HEAP_ALLOCATED, 1 /* callers */);

        if(hr != S_OK 
            //under OOM, we might not be able to get Execution Engine and can't access stress log
            && GetExecutionEngine ()
            // If we have not created StressLog ring buffer, we should not try to use it.
           // StressLog is going to do a memory allocation.  We may enter an endless loop.
           && ClrFlsGetValue(TlsIdx_StressLog) != NULL )
        {
            STRESS_LOG_OOM_STACK(dwBytes);
        }

        return (hr == S_OK) ? pMem : NULL;
    }
    else {

        LPVOID p = NULL;
#ifdef _DEBUG
        // Store the heap handle to detect heap contamination
        p = ::HeapAlloc (hHeap, dwFlags, dwBytes + OS_HEAP_ALIGN);
        if(p)
        {
            *((HANDLE*)p) = hHeap;
            p = (BYTE*)p + OS_HEAP_ALIGN;
        }
        GlobalAllocStore::AddAlloc (p);
#else
        p = ::HeapAlloc (hHeap, dwFlags, dwBytes);
#endif

        if (p != NULL)
        {
            if (fExecute)
            {
                DWORD fOldProtection;
                EEVirtualProtect(p, dwBytes, PAGE_EXECUTE_READWRITE, &fOldProtection);
            }
            MEMORY_REPORT_ALLOCATE(p, dwBytes, MemoryReport::PROCESS_HEAP_ALLOCATED, 1 /* callers */);
        }

        if(p == NULL
            //under OOM, we might not be able to get Execution Engine and can't access stress log
            && GetExecutionEngine ()
           // If we have not created StressLog ring buffer, we should not try to use it.
           // StressLog is going to do a memory allocation.  We may enter an endless loop.
           && ClrFlsGetValue(TlsIdx_StressLog) != NULL )
        {
            STRESS_LOG_OOM_STACK(dwBytes);
        }

        return p;
    }
}
#define HeapAlloc(hHeap, dwFlags, dwBytes) Dont_Use_HeapAlloc(hHeap, dwFlags, dwBytes)

LPVOID EEHeapAllocInProcessHeap(DWORD dwFlags, SIZE_T dwBytes)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    static HANDLE ProcessHeap = NULL;

    if (ProcessHeap == NULL)
        ProcessHeap = EEGetProcessHeap();

    // We need to guarentee a very small stack consumption in allocating.  And we can't allow
    // an SO to happen while calling into the host.  This will force a hard SO which is OK because
    // we shouldn't ever get this close inside the EE in SO-intolerant code, so this should
    // only fail if we call directly in from outside the EE, such as the JIT.
    MINIMAL_STACK_PROBE_CHECK_THREAD(GetThread());

    return EEHeapAlloc(ProcessHeap,dwFlags,dwBytes);
}

#undef HeapFree
BOOL EEHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    CONTRACT_VIOLATION(SOToleranceViolation);

#ifdef _DEBUG
    AssertAllocationAllowed();
#endif

    if (hHeap == NULL)
    {
        hHeap = EEGetProcessHeap();
    }
    BOOL retVal = FALSE;

    IHostMalloc *pHM = CorHost2::GetHostMalloc();
    if (pHM
        )
    {
        if (lpMem == NULL) {
            retVal = TRUE;
        }
#ifdef _DEBUG
        GlobalAllocStore::ValidateFree(lpMem);
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        retVal = pHM->Free(lpMem) == S_OK;
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    else {
#ifdef _DEBUG
        GlobalAllocStore::RemoveAlloc (lpMem);

        // Check the heap handle to detect heap contamination
        lpMem = (BYTE*)lpMem - OS_HEAP_ALIGN;
        HANDLE storedHeapHandle = *((HANDLE*)lpMem);
        if(storedHeapHandle != hHeap)
            _ASSERTE(!"Heap contamination detected! HeapFree was called on a heap other than the one that memory was allocated from.\n"
                      "Possible cause: you used new (executable) to allocate the memory, but didn't use DeleteExecutable() to free it.");
#endif
        // DON'T REMOVE THIS SEEMINGLY USELESS CAST
        //
        // On AMD64 the OS HeapFree calls RtlFreeHeap which returns a 1byte
        // BOOLEAN, HeapFree then doesn't correctly clean the return value
        // so the other 3 bytes which come back can be junk and in that case
        // this return value can never be false.
        retVal =  (BOOL)(BYTE)::HeapFree (hHeap, dwFlags, lpMem);
    }

    if (retVal)
        MEMORY_REPORT_FREE(lpMem, MemoryReport::PROCESS_HEAP_ALLOCATED);

    return retVal;
}
#define HeapFree(hHeap, dwFlags, lpMem) Dont_Use_HeapFree(hHeap, dwFlags, lpMem)

BOOL EEHeapFreeInProcessHeap(DWORD dwFlags, LPVOID lpMem)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    static HANDLE ProcessHeap = NULL;

    if (ProcessHeap == NULL)
        ProcessHeap = EEGetProcessHeap();


    return EEHeapFree(ProcessHeap,dwFlags,lpMem);
}


#undef HeapValidate
BOOL EEHeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem) {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;


    return TRUE;
}
#define HeapValidate(hHeap, dwFlags, lpMem) Dont_Use_HeapValidate(hHeap, dwFlags, lpMem)

HANDLE EEGetProcessExecutableHeap() {
    // Note: this can be called a little early for real contracts, so we use static contracts instead.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    return NULL;

}


#undef SleepEx
#undef Sleep
DWORD EESleepEx(DWORD dwMilliseconds, BOOL bAlertable)
{
    WRAPPER_CONTRACT;
    DWORD res;

    IHostTaskManager *provider = CorHost2::GetHostTaskManager();
    if ((provider == NULL)) {
        res = ::SleepEx(dwMilliseconds, bAlertable);
    }
    else {
        DWORD option = 0;
        if (bAlertable)
        {
            option = WAIT_ALERTABLE;
        }


        HRESULT hr;

        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = provider->Sleep(dwMilliseconds, option);
        END_SO_TOLERANT_CODE_CALLING_HOST;

        if (hr == S_OK) {
            res = WAIT_OBJECT_0;
        }
        else if (hr == HOST_E_INTERRUPTED) {
            _ASSERTE(bAlertable);
            Thread *pThread = GetThread();
            if (pThread)
            {
                pThread->UserInterruptAPC(APC_Code);
            }
            res = WAIT_IO_COMPLETION;
        }
        else
        {
            _ASSERTE (!"Unknown return from host Sleep\n");
            res = WAIT_OBJECT_0;
        }
    }

    return res;
}
#define SleepEx(dwMilliseconds,bAlertable) \
        Dont_Use_SleepEx(dwMilliseconds,bAlertable)
#define Sleep(a) Dont_Use_Sleep(a)


// non-zero return value if this function causes the OS to switch to another thread
BOOL __SwitchToThread (DWORD dwSleepMSec)
{
  CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
	
    return  __DangerousSwitchToThread(dwSleepMSec, FALSE);
}

BOOL __DangerousSwitchToThread (DWORD dwSleepMSec, BOOL goThroughOS)
{
    // If you sleep for a long time, the thread should be in Preemptive GC mode.
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(dwSleepMSec < 10000 || GetThread() == NULL || !GetThread()->PreemptiveGCDisabled());
    }
    CONTRACTL_END;

    if (CLRTaskHosted())
    {
        Thread *pThread = GetThread();
        if (pThread && pThread->HasThreadState(Thread::TS_YieldRequested))
        {
            pThread->ResetThreadState(Thread::TS_YieldRequested);
        }
    }

#ifdef THREAD_DELAY
    if (g_pConfig !=NULL && g_pConfig->m_pThreadDelay != NULL)
    {
        if (g_pConfig->m_pThreadDelay->SpikeOn ())
            g_pConfig->m_pThreadDelay->SpikeDelay ();
        if (g_pConfig->m_pThreadDelay->DelayOn ())
            g_pConfig->m_pThreadDelay->ShortDelay ();
    }
#endif //THREAD_DELAY

    if (dwSleepMSec > 0)
    {
        ClrSleepEx(dwSleepMSec,FALSE);
        return TRUE;
    }
    IHostTaskManager *provider = CorHost2::GetHostTaskManager();
    if ((provider == NULL) || (goThroughOS == TRUE)){
        return SwitchToThread();
    }
    else {
        DWORD option = 0;

        HRESULT hr;

        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = provider->SwitchToTask(option);
        END_SO_TOLERANT_CODE_CALLING_HOST;

        return hr == S_OK;
    }
}

// Locking routines supplied by the EE to the other DLLs of the CLR.  In a _DEBUG
// build of the EE, we poison the Crst as a poor man's attempt to do some argument
// validation.
#define POISON_BITS 3

static inline CRITSEC_COOKIE CrstToCookie(Crst * pCrst) {
    LEAF_CONTRACT;

    _ASSERTE((((uintptr_t) pCrst) & POISON_BITS) == 0);
#ifdef _DEBUG
    pCrst = (Crst *) (((uintptr_t) pCrst) | POISON_BITS);
#endif
    return (CRITSEC_COOKIE) pCrst;
}

static inline Crst *CookieToCrst(CRITSEC_COOKIE cookie) {
    LEAF_CONTRACT;

    _ASSERTE((((uintptr_t) cookie) & POISON_BITS) == POISON_BITS);
#ifdef _DEBUG
    cookie = (CRITSEC_COOKIE) (((uintptr_t) cookie) & ~POISON_BITS);
#endif
    return (Crst *) cookie;
}

CRITSEC_COOKIE EECreateCriticalSection(LPCSTR szTag, LPCSTR level, CrstFlags flags) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CRITSEC_COOKIE ret = NULL;

    EX_TRY
    {
    // This may be controversial, but seems like the correct discipline.  If the
    // EE has called out to any other DLL of the CLR in cooperative mode, we
    // arbitrarily force lock acquisition to occur in preemptive mode.  See our
    // treatment of AcquireLock below.
    //_ASSERTE((flags & (CRST_UNSAFE_COOPGC | CRST_UNSAFE_ANYMODE)) == 0);
        ret = CrstToCookie(new Crst(szTag, (CrstLevel) level, flags));
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    // Note: we'll return NULL if the create fails. That's a true NULL, not a poisoned NULL.
    return ret;
}

void EEDeleteCriticalSection(CRITSEC_COOKIE cookie) {
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_NOTRIGGER);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATE_BACKOUT_STACK_CONSUMPTION;

    Crst *pCrst = CookieToCrst(cookie);
    _ASSERTE(pCrst);

    delete pCrst;
}

void EEEnterCriticalSection(CRITSEC_COOKIE cookie) {
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    Crst *pCrst = CookieToCrst(cookie);
    _ASSERTE(pCrst);
    //if (pCrst)
    //    COMPlusThrow(kNullArgumentException);

    pCrst->Enter();
}

void EELeaveCriticalSection(CRITSEC_COOKIE cookie) {
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Crst *pCrst = CookieToCrst(cookie);
    _ASSERTE(pCrst);
    //if (pCrst)
    //    COMPlusThrow(kNullArgumentException);

    pCrst->Leave();
}

LPVOID EETlsGetValue(DWORD slot)
{
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_MODE_ANY;

    //
    //
    SCAN_IGNORE_THROW;

    void **pTlsData = CExecutionEngine::CheckThreadState(slot, FALSE);

    if (pTlsData)
        return pTlsData[slot];
    else
        return NULL;
}

BOOL EETlsCheckValue(DWORD slot, LPVOID * pValue)
{
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_MODE_ANY;

    //
    //
    SCAN_IGNORE_THROW;

    void **pTlsData = CExecutionEngine::CheckThreadState(slot, FALSE);

    if (pTlsData)
    {
        *pValue = pTlsData[slot];
        return TRUE;
    }

    return FALSE;
}

VOID EETlsSetValue(DWORD slot, LPVOID pData)
{
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_ANY;

    void **pTlsData = CExecutionEngine::CheckThreadState(slot);

    if (pTlsData)  // Yes, CheckThreadState(slot, TRUE) can return NULL now.
    {
        pTlsData[slot] = pData;
    }
}

BOOL EEAllocationDisallowed()
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    // On Debug build we make sure that a thread is not going to do memory allocation
    // after it suspends another thread, since the another thread may be suspended while
    // having OS Heap lock.
    return !Thread::AllowCallout();
#else
    return FALSE;
#endif
}


//
// Export used in the ClickOnce installer for launching manifest-based applications.
//

typedef struct _tagNameMap {
    LPWSTR  pwszProcessorArch;
    DWORD   dwRuntimeInfoFlag;
} NAME_MAP;

DWORD g_DfSvcSpinLock = 0;
void EnterDfSvcSpinLock () {
    while (1) {
        if (InterlockedExchange ((LPLONG)&g_DfSvcSpinLock, 1) == 1)
            ClrSleepEx (5, FALSE);
        else
            return;
    }
}

void LeaveDfSvcSpinLock () {
    InterlockedExchange ((LPLONG)&g_DfSvcSpinLock, 0);
}

STDAPI CorLaunchApplication (HOST_TYPE               dwClickOnceHost,
                             LPCWSTR                 pwzAppFullName,
                             DWORD                   dwManifestPaths,
                             LPCWSTR                 *ppwzManifestPaths,
                             DWORD                   dwActivationData,
                             LPCWSTR                 *ppwzActivationData,
                             LPPROCESS_INFORMATION   lpProcessInformation)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pwzAppFullName, NULL_OK));
        PRECONDITION(CheckPointer(ppwzManifestPaths, NULL_OK));
        PRECONDITION(CheckPointer(ppwzActivationData, NULL_OK));
        PRECONDITION(dwClickOnceHost == HOST_TYPE_DEFAULT || dwClickOnceHost == HOST_TYPE_APPLAUNCH || dwClickOnceHost == HOST_TYPE_CORFLAG);
        PRECONDITION(CheckPointer(lpProcessInformation));
    } CONTRACTL_END;


    if (pwzAppFullName == NULL)
        return E_POINTER;

    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    END_ENTRYPOINT_NOTHROW;

    return hr;
}
