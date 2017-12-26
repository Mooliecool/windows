/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    localstorage.c

Abstract:

    Implementation of Thread local storage functions.

--*/

#include "pal/palinternal.h"

#include <pthread.h>

#include "pal/dbgmsg.h"
#include "pal/thread.h"
#include "process.h"
#include "pal/misc.h"

#include <stddef.h>


SET_DEFAULT_DEBUG_CHANNEL(THREAD);

/* This tracks the slots that are used for TlsAlloc. Its size in bits
   must be the same as TLS_SLOT_SIZE in pal/thread.h. Since this is
   static, it is initialized to 0, which is what we want. */
static unsigned __int64 sTlsSlotFields;

/*++
Function:
  TlsAlloc

See MSDN doc.
--*/
DWORD
PALAPI
TlsAlloc(
	 VOID)
{
    DWORD dwIndex;
    unsigned int i;

    PERF_ENTRY(TlsAlloc);
    ENTRY("TlsAlloc()\n");

    /* Yes, this could be ever so slightly improved. It's not
       likely to be called enough to matter, though, so we won't
       optimize here until or unless we need to. */
    PROCProcessLock();
    for(i = 0; i < sizeof(sTlsSlotFields) * 8; i++)
    {
        if ((sTlsSlotFields & ((unsigned __int64) 1 << i)) == 0)
        {
            sTlsSlotFields |= ((unsigned __int64) 1 << i);
            break;
        }
    }
    if (i == sizeof(sTlsSlotFields) * 8)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        dwIndex = TLS_OUT_OF_INDEXES;
    }
    else
    {
        dwIndex = i;
    }
    PROCProcessUnlock();

    LOGEXIT("TlsAlloc returns DWORD %u\n", dwIndex);
    PERF_EXIT(TlsAlloc);
    return dwIndex;
}


/*++
Function:
  TlsGetValue

See MSDN doc.
--*/
LPVOID
PALAPI
TlsGetValue(
	    IN DWORD dwTlsIndex)
{
    THREAD *thread;
    PERF_ENTRY(TlsGetValue);
    ENTRY("TlsGetValue()\n");
    if (dwTlsIndex == (DWORD) -1 || dwTlsIndex >= TLS_SLOT_SIZE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    thread = PROCGetCurrentThreadObject();
    if (thread == NULL)
    {
        ASSERT("Unable to access the current thread\n");
        SetLastError(ERROR_INVALID_DATA);
        return 0;
    }

    /* From MSDN : "The TlsGetValue function calls SetLastError to clear a
       thread's last error when it succeeds." */
    thread->lastError = NO_ERROR;
    
    LOGEXIT("TlsGetValue \n" );
    PERF_EXIT(TlsGetValue); 
    return thread->tlsSlots[dwTlsIndex];
}


/*++
Function:
  TlsSetValue

See MSDN doc.
--*/
BOOL
PALAPI
TlsSetValue(
	    IN DWORD dwTlsIndex,
	    IN LPVOID lpTlsValue)
{
    THREAD *thread;
    BOOL bRet = FALSE;
    PERF_ENTRY(TlsSetValue);
    ENTRY("TlsSetValue(dwTlsIndex=%u, lpTlsValue=%p)\n", dwTlsIndex, lpTlsValue);

    if (dwTlsIndex == (DWORD) -1 || dwTlsIndex >= TLS_SLOT_SIZE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }

    thread = PROCGetCurrentThreadObject();
    if (thread == NULL)
    {
        ASSERT("Unable to access the current thread\n");
        goto EXIT;
    }

    thread->tlsSlots[dwTlsIndex] = lpTlsValue;
    bRet = TRUE;
EXIT:
    LOGEXIT("TlsSetValue returns BOOL %d\n", bRet);
    PERF_EXIT(TlsSetValue);
    return bRet;
}


/*++
Function:
  TlsFree

See MSDN doc.
--*/
BOOL
PALAPI
TlsFree(
	IN DWORD dwTlsIndex)
{
    THREAD *pThread;

    PERF_ENTRY(TlsFree);
    ENTRY("TlsFree(dwTlsIndex=%u)\n", dwTlsIndex);

    
    if (dwTlsIndex == (DWORD) -1 || dwTlsIndex >= TLS_SLOT_SIZE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT("TlsFree returns BOOL FALSE\n");
        PERF_EXIT(TlsFree);
        return FALSE;
    }

    PROCProcessLock();
    /* Reset all threads' values to zero for this index. */
    for(pThread = pGThreadList; 
        pThread != NULL; pThread = pThread->next)
    {
        pThread->tlsSlots[dwTlsIndex] = 0;
    }
    sTlsSlotFields &= ~((unsigned __int64) 1 << dwTlsIndex);
    PROCProcessUnlock();

    LOGEXIT("TlsFree returns BOOL TRUE\n");
    PERF_EXIT(TlsFree);
    return TRUE;
}


/* Internal function definitions **********************************************/

/*++
Function:
    TlsInternalGetValue

    Do the work of TlsGetValue, but without calling SetLastError(NO_ERROR)
    This is so that PAL functions can call it without changing LastError

Parameters :
    DWORD dwTlsIndex : TLS index to query
    LPVOID *pValue : pointer where the requested value will be placed

Return value :
    TRUE if function succeeds, FALSE otherwise.

--*/
BOOL TlsInternalGetValue(DWORD dwTlsIndex, LPVOID *pValue)
{
    THREAD *thread;

    if (dwTlsIndex == (DWORD) -1 || dwTlsIndex >= TLS_SLOT_SIZE)
    {
        ASSERT("Invalid index (%d)\n", dwTlsIndex);
        return FALSE;
    }

    thread = PROCGetCurrentThreadObject();
    if (thread == NULL)
    {
        ASSERT("Unable to access the current thread\n");
        return FALSE;
    }

    *pValue = thread->tlsSlots[dwTlsIndex];
    return TRUE;
}

#if defined(USE_TLSLOOKASIDECACHE)

// dummy thread to use as filler for thread_hints to avoid checks for NULL
static const THREAD dummyTHREAD;

// thread_hints is table indexed by hashed stack pointer that contains hints 
// on what the current THREAD* object may be. Every TLS lookup tries to use 
// hint in this table first before going through the slow pthread_getspecific call.
volatile THREADHINTS thread_hints;

// flush_counter is changed to a new unique value every time
// a THREAD structure is reused. The TLS cache uses it to detect races.
// The thread safety is guaranteed this way without taking locks.
volatile ULONG flush_counter;

#if !defined(USE_OPTIMIZEDTLSGETTER)

// Why is the thread_hints datastructure thread safe?
//
// The thread_hints entries matching the given thread are cleaned up on the thread exit 
// or THREAD structure destruction - whatever comes first. Thus the hint is guaranteed to 
// be for a live thread when fetched. The only potential problem is when the thread dies 
// during the check of stack boundaries. This case will be detected by flush_counter mismatch.

THREAD* PROCGetCurrentThreadObjectInternal()
{
    ULONG counter;
    THREAD* pThread;
    volatile char stack;
    size_t sp = (size_t)(LPVOID)&stack;

    // assume that the minimum stack size is 128k
    int key = sp >> 17;

    // mix it
    key -= key >> 7;
    key -= key >> 5;
    key -= key >> 3;

    key &= 0xFF;

    counter = flush_counter;

    // this fetch is atomic since thread_hints is volatile
    pThread = thread_hints[key];

    if ((size_t)pThread->minStack <= sp && sp < (size_t)pThread->maxStack)
    {
        // make sure that nothing changed in the meantime
        if (counter == flush_counter)
        {
            return pThread;
        }
    }

    // fetch it hard way
    pThread = pthread_getspecific(thObjKey);

    if (pThread != NULL)
    {
        // Update the stack boundaries. The stack range for 
        // the thread never shrinks. This invariant prevents malicious 
        // races from happening.
        if ((size_t)pThread->maxStack < sp)
        {
            pThread->maxStack = (LPVOID)((sp + VIRTUAL_PAGE_MASK) & ~VIRTUAL_PAGE_MASK);
        }

        if ((size_t)pThread->minStack > sp)
        {
            pThread->minStack = (LPVOID)(sp & ~VIRTUAL_PAGE_MASK);
        }

        thread_hints[key] = pThread;
    }

    return pThread;
}

#else // !USE_OPTIMIZEDTLSGETTER

PAL_POPTIMIZEDTLSGETTER fnPROCGetCurrentThreadObject;

// Slow part of optimized TLS getter called from the machine-specific code 
// generated at runtime

THREAD* PROCGetCurrentThreadObjectSlow(size_t sp, THREAD** ppThreadHint)
{
    THREAD* pThread;

    // fetch it hard way
    pThread = pthread_getspecific(thObjKey);

    if (pThread != NULL)
    {
        // Update the stack boundaries. The stack range for
        // the thread never shrinks. This invariant prevents malicious
        // races from happening.
        if ((size_t)pThread->maxStack < sp)
        {
            pThread->maxStack = (LPVOID)((sp + VIRTUAL_PAGE_MASK) & ~VIRTUAL_PAGE_MASK);
        }

        if ((size_t)pThread->minStack > sp)
        {
            pThread->minStack = (LPVOID)(sp & ~VIRTUAL_PAGE_MASK);
        }

        *ppThreadHint = pThread;
    }

    return pThread;
}

#endif // !USE_OPTIMIZEDTLSGETTER

#endif // USE_TLSLOOKASIDECACHE

/*++
Function:
    TLSCacheInitialize

    Initialize the TLS cache
--*/
VOID TLSCacheInitialize()
{
#if defined(USE_TLSLOOKASIDECACHE)
    int i;

    for (i = 0; i < sizeof(thread_hints) / sizeof(thread_hints[0]); i++)
    {
        thread_hints[i] = (THREAD*)&dummyTHREAD;
    }
#endif
}

/*++
Function:
    TLSCacheFlush

    Notify TLS cache about THREAD structure being terminated
--*/
VOID TLSCacheFlush(THREAD* pThread)
{
#if defined(USE_TLSLOOKASIDECACHE)
    int i;

    for (i = 0; i < sizeof(thread_hints) / sizeof(thread_hints[0]); i++)
    {
        // There is no need for atomic assignement here. It does not hurt if somebody else 
        // comes in and sets a new valid hint. On systems with weaker memory models than x86, the changes 
        // will be published by the following InterlockedIncrement. InterlockedIncrement should raise 
        // memory barrier on such systems per MSDN.
        if (thread_hints[i] == pThread)
        {
            thread_hints[i] = (THREAD*)&dummyTHREAD;
        }
    }

    InterlockedIncrement(&flush_counter);
#endif
}


/*++
Function:
    PAL_MakeOptimizedTlsGetter

    Creates a platform-optimized version of TlsGetValue compiled
    for a particular index. Can return NULL - the caller should substitute
    a non-optimized getter in this case.
--*/
PAL_POPTIMIZEDTLSGETTER
PALAPI
PAL_MakeOptimizedTlsGetter(
        IN DWORD dwTlsIndex)
{
    PAL_POPTIMIZEDTLSGETTER Ret;

    ENTRY("PAL_MakeOptimizedTlsGetter(dwTlsIndex=%u)\n", dwTlsIndex);

    if (dwTlsIndex == TLS_OUT_OF_INDEXES)
    {
        ASSERT("Invalid TLS index");
    }

#if defined(USE_OPTIMIZEDTLSGETTER)
    Ret = TLSMakeOptimizedGetter(dwTlsIndex);
    if (!Ret)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    }
#else
    SetLastError(ERROR_NOT_SUPPORTED);
    Ret = NULL;
#endif

    LOGEXIT("PAL_MakeOptimizedTlsGetter returns PAL_POPTIMIZEDTLSGETTER %p\n", Ret);

    return Ret;
}

/*++
Function:
    PAL_FreeOptimizedTlsGetter

    Frees a function created by MakeOptimizedTlsGetter().
--*/
VOID
PALAPI
PAL_FreeOptimizedTlsGetter(
        IN PAL_POPTIMIZEDTLSGETTER pOptimizedTlsGetter)
{
    ENTRY("PAL_FreeOptimizedTlsGetter(pOptimizedTlsGetter=%p)\n", pOptimizedTlsGetter);

#if defined(USE_OPTIMIZEDTLSGETTER)
    if (pOptimizedTlsGetter != NULL)
    {
        TLSFreeOptimizedGetter(pOptimizedTlsGetter);
    }
#else
    if (pOptimizedTlsGetter != NULL)
    {
        ASSERT("Unexpected value of pOptimizedTlsGetter\n");
    }
#endif

    LOGEXIT("PAL_FreeOptimizedTlsGetter returns\n");
}
