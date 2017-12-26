/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    thread.c

Abstract:

    Implementation of functions related to threads.

Note:
   Current implementation uses the pthread_t as
   the thread ID which is a DWORD. It works fine on a 32 bits
   architecture, but it will need to be tested on a 64
   bits architecture.

Revision History:

--*/
#include "pal/palinternal.h"
#include "pal/critsect.h"
#include "pal/thread.h"
#include "process.h"
#include "pal/dbgmsg.h"
#include "pal/init.h"
#include "pal/socket2.h"
#include "pal/mutex.h"
#include "pal/seh.h"
#include "pal/misc.h"

#include <pthread.h>
#if (HAVE_PTHREAD_SUSPEND || HAVE_PTHREAD_SUSPEND_NP) && (HAVE_PTHREAD_RESUME || HAVE_PTHREAD_RESUME_NP || HAVE_PTHREAD_CONTINUE || HAVE_PTHREAD_CONTINUE_NP)
#if HAVE_PTHREAD_NP_H
#include <pthread_np.h>
#endif
#elif HAVE_MACH_THREADS
#include <mach/thread_act.h>
#else
#error "Don't know how to suspend and resume threads on this platform!"
#endif
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#if HAVE_POLL
#include <poll.h>
#else
#include "pal/fakepoll.h"
#endif  // HAVE_POLL
#include <limits.h>
#if HAVE_STROPTS_H
#include <stropts.h>
#endif  // HAVE_STROPTS_H
#if HAVE_SYS_LWP_H
#include <sys/lwp.h>
// If we don't have sys/lwp.h but do expect to use _lwp_self, declare it to silence compiler warnings
#elif HAVE__LWP_SELF
int _lwp_self ();
#endif // HAVE_LWP_H


/* ------------------- Static function prototypes ------------------------------*/
static HANDLE THREADCreateThread(
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN DWORD dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId,
    BOOL bIsInternalThread);
static void *THREADEntry(void *lpThread);
static int dupThreadHandle( HANDLE handle, HOBJSTRUCT *handle_data);
static int closeThreadHandle( HOBJSTRUCT *handle_data);
static int dupDummyThreadHandle( HANDLE handle, HOBJSTRUCT *handle_data);
static int closeDummyThreadHandle( HOBJSTRUCT *handle_data);
static int createBlockingPipe( DWORD tid );
static VOID destroyBlockingPipe( int pipe, DWORD ThreadId, THREAD_WAIT_STATE wait_state);
static BOOL init_suspender(THREAD *pThread);
static BOOL THREADSuspendThread(THREAD *lpThread);
static BOOL THREADContinueThread(THREAD *lpThread);


/* ------------------- Definitions ------------------------------*/
SET_DEFAULT_DEBUG_CHANNEL(THREAD);

/* code used by ResumeThread to wake up a thread using its blocking pipe. */
CONST DWORD WAKEUPCODE=0xaabbccdd;

/* code used by closeDummyThreadHandle to unblock a process stopped in 
   CreateProcess */
CONST DWORD GIVEUPCODE=0x13579BDF;

static const char THREADPIPE_PREFIX[] = ".rotor_pal_threadpipe";

/* minimal stack size for new threads, currently 256KB */
static const int MINIMAL_STACK_SIZE = 256*1024; 

/*++
Function:
    isValidThreadObject

Abstract:
    Verify if the object is a thread, by checking the magic field
  
Parameters:
    IN  lpThread: THREAD object

Return:
    TRUE: if valid
    FALSE: otherwise
--*/
static inline BOOL isValidThreadObject(const THREAD *lpThread)
{
    return (lpThread->objHeader.type == HOBJ_THREAD);
}

/* list of free THREAD structures */
static THREAD * volatile free_threads_list = NULL;

/* lock to access list of free THREAD structures */
/* NOTE: can't use a CRITICAL_SECTION here (see comment in FreeTHREAD) */
static volatile int free_threads_spinlock = 0;

/*++
Function:
  TLSInitialize

  Initialize the TLS subsystem
--*/
BOOL TLSInitialize()
{
    /* Create the pthread key for thread objects, which we use
       for fast access to the current thread object. */
    if (pthread_key_create(&thObjKey, NULL))
    {
        ERROR("Couldn't create the thread object key\n");
        return FALSE;
    }

#if defined(USE_OPTIMIZEDTLSGETTER)
    fnPROCGetCurrentThreadObject = TLSMakeOptimizedGetter(THREAD_OBJECT_TLS_INDEX);
    if (fnPROCGetCurrentThreadObject == NULL)
    {
        ERROR( "Failed to create optimized TLS getter.\n" );
        return FALSE;
    }
#endif

    SPINLOCKInit(&free_threads_spinlock);

    TLSCacheInitialize();

    return TRUE;
}

/*++
Function:
    TLSCleanup

    Shutdown the TLS subsystem
--*/
VOID TLSCleanup()
{
    THREAD* pThread;
    THREAD* pTmp;

    pThread = free_threads_list;
    free_threads_list = NULL;
    while (pThread != NULL) {
        pTmp = pThread->next;
        free(pThread);
        pThread = pTmp;
    }

    SPINLOCKDestroy(&free_threads_spinlock);
}

/*++
Function:
    AllocTHREAD

Abstract:
    Allocate THREAD structure
  
Return:
    The fresh zero-filled thread structure, NULL otherwise
--*/
static THREAD* AllocTHREAD()
{
    THREAD* pThread = NULL;

    /* Get the lock */
    SPINLOCKAcquire(&free_threads_spinlock, 0);

    pThread = free_threads_list;
    if (pThread != NULL)
    {
        free_threads_list = pThread->next;
    }

    /* Release the lock */
    SPINLOCKRelease(&free_threads_spinlock);

    if (pThread == NULL)
    {
        pThread = (THREAD*)malloc(sizeof(THREAD));
        if (pThread == NULL)
        {
            return NULL;
        }
    }

    // set everything to NULL
    ZeroMemory(pThread, sizeof(THREAD));

    return pThread;
}

/*++
Function:
    FreeTHREAD

Abstract:
    Free THREAD structure
  
--*/
static void FreeTHREAD(THREAD* pThread)
{
#if _DEBUG
    // invalidate the content of the structure in debug builds
    FillMemory(pThread, sizeof(THREAD), 0xCE);
#endif

    // Never actually free the THREAD structure to make the TLS lookaside cache work. 
    // THREAD* for terminated thread can be stuck in the lookaside cache code for an 
    // arbitrary amount of time. The unused THREAD* structures has to remain in a 
    // valid memory and thus can't be returned to the heap.

    /* NOTE: can't use a CRITICAL_SECTION here: EnterCriticalSection(&cs,TRUE) and
       LeaveCriticalSection(&cs,TRUE) need to access the thread private data 
       stored in the very THREAD structure that we just destroyed. Entering and 
       leaving the critical section with internal==FALSE leads to possible hangs
       in the PROCSuspendOtherThreads logic, at shutdown time */

    /* Get the lock */
    SPINLOCKAcquire(&free_threads_spinlock, 0);

    pThread->next = free_threads_list;
    free_threads_list = pThread;

    /* Release the lock */
    SPINLOCKRelease(&free_threads_spinlock);
}

/*++
Function:
  THREADGetThreadProcessId

returns the process owner ID of the indicated hThread
--*/
DWORD 
THREADGetThreadProcessId(
    HANDLE hThread)
{
    THREAD* lpThread;
    DWORD ret = 0;

    if(hThread == hPseudoCurrentThread)
    {
        return GetCurrentProcessId();
    } 
        
    if (NULL != (lpThread = (THREAD*) HMGRLockHandle2 (hThread, HOBJ_THREAD)))
    {
        /* make sure that hThread is a thread handle and not a dummy thread handle */
        if (lpThread->objHeader.close_handle != closeDummyThreadHandle)
        {
            ret = GetCurrentProcessId();
        }
        
        HMGRUnlockHandle(hThread, &lpThread->objHeader);        
        return ret;
    }
    
    ERROR("Couldn't retreive the hThread:%p pid owner !\n", hThread);
    return ret;
}

/*++
Function:
  GetCurrentThreadId

See MSDN doc.
--*/
DWORD
PALAPI
GetCurrentThreadId(
            VOID)
{
    DWORD ret;

    PERF_ENTRY(GetCurrentThreadId);
    ENTRY("GetCurrentThreadId()\n");
    ret = THREADSilentGetCurrentThreadId();
    LOGEXIT("GetCurrentThreadId returns DWORD %#x\n", ret);    
    PERF_EXIT(GetCurrentThreadId);
    return ret;
}



/*++
Function:
  GetCurrentThread

See MSDN doc.
--*/
HANDLE
PALAPI
GetCurrentThread(
          VOID)
{
    PERF_ENTRY(GetCurrentThread);
    ENTRY("GetCurrentThread()\n");
    
    LOGEXIT("GetCurrentThread returns HANDLE %p\n", hPseudoCurrentThread);
    PERF_EXIT(GetCurrentThread);

    /* return a pseudo handle */
    return (HANDLE) hPseudoCurrentThread;
}

/*++
Function:
  SwitchToThread

See MSDN doc.
--*/
BOOL
PALAPI
SwitchToThread(
    VOID)
{
    BOOL ret;

    PERF_ENTRY(SwitchToThread);
    ENTRY("SwitchToThread(VOID)\n");

    /* sched_yield yields to another thread in the current process. This implementation 
       won't work well for cross-process synchronization. */
    ret = (sched_yield() == 0);

    LOGEXIT("SwitchToThread returns BOOL %d\n", ret);
    PERF_EXIT(SwitchToThread);

    return ret;
}

/*++
Function:
  THREADNewThreadObject [internal]
  
  Allocate and initialize a THREAD object. This is a helper for CreateThread, 
  to reduce the size of error paths
  
--*/
static THREAD *THREADNewThreadObject(void)
{
    THREAD *lpThreadObj;
    BOOL *pAwakened;

    lpThreadObj = AllocTHREAD();

    if (!lpThreadObj) 
    {
        ERROR("malloc failure; can't allocate new THREAD object\n");
        return NULL;
    }

    /* fill the thread structure */
    lpThreadObj->objHeader.type = HOBJ_THREAD;
    lpThreadObj->objHeader.close_handle = closeThreadHandle; 
    lpThreadObj->objHeader.dup_handle = dupThreadHandle;
    lpThreadObj->refCount = 1;
    lpThreadObj->thread_state = TS_STARTING;
    lpThreadObj->blockingPipe = -1;
    lpThreadObj->threadPriority = THREAD_PRIORITY_NORMAL;
    lpThreadObj->minStack = (LPVOID)-1;
    lpThreadObj->safe_state = TRUE;
    // everything else is zero-filled by AllocTHREAD       
        
    lpThreadObj->hThread = HMGRGetHandle((HOBJSTRUCT *) lpThreadObj);
    if(INVALID_HANDLE_VALUE == lpThreadObj->hThread)
    {
        ERROR("Couldn't allocate handle for new THREAD object\n");
        FreeTHREAD(lpThreadObj);
        return NULL;
    }

    lpThreadObj->waitAwakened = SHMalloc(sizeof(DWORD));
    pAwakened = SHMPTR_TO_PTR(lpThreadObj->waitAwakened);
    if(NULL == pAwakened)
    {
        ERROR("Couldn't allocate thread's awakened state\n");
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        return NULL;
    }               
    *pAwakened = TWS_ACTIVE;

    if (0 != SYNCInitializeCriticalSection(&(lpThreadObj->thread_crit_section)))
    {
        ERROR("Couldn't initialize thread's critical section\n");
        SHMfree(lpThreadObj->waitAwakened);
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        return NULL;
    }

    if(!init_suspender(lpThreadObj))
    {
        ERROR("couldn't initialize thread object's suspension resources\n");
        DeleteCriticalSection(&lpThreadObj->thread_crit_section);
        SHMfree(lpThreadObj->waitAwakened);
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        return NULL;
    }
    return lpThreadObj;
}

/*++
Function:
  CreateThread

Note:
  lpThreadAttributes could be ignored.

See MSDN doc.

--*/
HANDLE
PALAPI
CreateThread(
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN DWORD dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId)
{
    if (!PALIsInitialized() || PALIsShuttingDown())
    {
        /* Disable the client-exposed CreateThread when PAL is not
           yet initialized or it is shutting down. */
        ERROR("Process is starting up or terminating, can't create new thread at this time.\n");
        SetLastError(ERROR_PROCESS_ABORTED);
        return NULL;
    }
    return THREADCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress,
                              lpParameter, dwCreationFlags, lpThreadId, FALSE);
}

/*++
Function:
  CreateInternalThread

Note:
  lpThreadAttributes could be ignored.

See MSDN doc.

--*/
HANDLE
PALAPI
CreateInternalThread(
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN DWORD dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId)
{
    return THREADCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress,
                              lpParameter, dwCreationFlags, lpThreadId, TRUE);
}

/*++
Function:
  THREADCreateThread

Note:
  lpThreadAttributes could be ignored.
  bIsInternalThread marks whether the thread is a PAL thread or a thread
  in the host application.

See CreateThread.

--*/
static
HANDLE
THREADCreateThread(
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN DWORD dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId,
    BOOL bIsInternalThread)
{
    THREAD *lpThreadObj;
    pthread_t pthread;
    pthread_attr_t pthreadAttr;
    size_t pthreadStackSize;
#if PTHREAD_CREATE_MODIFIES_ERRNO
    int storedErrno;
#endif  // PTHREAD_CREATE_MODIFIES_ERRNO
    int ret;
    HANDLE DupThreadHandle;
    HANDLE hRet = NULL;

    PERF_ENTRY(CreateThread);
    ENTRY("CreateThread(lpThreadAttr=%p, dwStackSize=%u, lpStartAddress=%p, "
          "lpParameter=%p, dwFlags=%#x, lpThreadId=%#x)\n",
          lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter,
          dwCreationFlags, lpThreadId);

    if(0 != terminator)
    {
        /* somebody called ExitProcess (or TerminateProcess). PROCCleanupProcess
           will be suspending all threads; it takes the process lock while 
           walking the thread list, but has to release it before calling 
           SuspendThread. This gives the target thread a small window in which 
           it might create a new thread and add it to the list; it would then 
           be possible for this new thread to create yet another new thread, 
           etc... To avoid this, let CreateThread fail if shutdown has started. 
           Note : it might be better to sleep infinitely instead of returning...
           but then we'd still have to return if it's this thread that called 
           ExitProcess (and we got here through a DllMain call)... */
        ERROR("process is terminating, can't create new thread.\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto EXIT;
    }

    /* Validate parameters */

    if (lpThreadAttributes != NULL)
    {
        ASSERT("lpThreadAttributes parameter must be NULL (%p)\n", 
               lpThreadAttributes);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }
    
    if ((dwCreationFlags != 0) && (dwCreationFlags != CREATE_SUSPENDED))
    {
        ASSERT("dwCreationFlags parameter is invalid (%#x)\n", dwCreationFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }

    /* Create the thread object */
    lpThreadObj = THREADNewThreadObject();
    if (!lpThreadObj) 
    {
        ERROR("failed to create new THREAD object\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto EXIT;
    }
    lpThreadObj->lpStartAddress = lpStartAddress;
    lpThreadObj->lpStartParameter = lpParameter;
    lpThreadObj->bCreateSuspended = (dwCreationFlags & CREATE_SUSPENDED);
    lpThreadObj->SuspendedWithPipeFlag = FALSE;
    lpThreadObj->isInternal = bIsInternalThread;

    /* Duplicate the thread handle, the duplicate will be returned.
       Like this, the original thread handle is never returned to
       the application, and is only being closed in TerminateCurrentThread
       function. */
    if (DuplicateHandle(GetCurrentProcess(), lpThreadObj->hThread, 
                    GetCurrentProcess(), &DupThreadHandle,
                    0, FALSE, DUPLICATE_SAME_ACCESS) == 0)
    {
        ASSERT("Could not duplicate the thread handle\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        CloseHandle(lpThreadObj->hThread);
        goto EXIT;
    }

    /* We need to create an event here.  This will be used to allow us
       to block CreateThread's return until our new thread is properly
       initialized in THREADEntry(). */
    lpThreadObj->hBlockCreateThreadEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if ( lpThreadObj->hBlockCreateThreadEvent == NULL )
    {
        ERROR("CreateEvent failed\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        CloseHandle(DupThreadHandle);
        CloseHandle(lpThreadObj->hThread);
        goto EXIT;
    }

    /* add the thread to the list of thread for this process */
    if(!PROCAddThread(lpThreadObj))
    {
        ERROR("couldn't add thread object to process list\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        CloseHandle(lpThreadObj->hBlockCreateThreadEvent);
        CloseHandle(DupThreadHandle);
        CloseHandle(lpThreadObj->hThread);
        goto EXIT;
    }

    if (0 != pthread_attr_init(&pthreadAttr))
    {
        ERROR("couldn't initialize pthread attributes\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        PROCRemoveThread(lpThreadObj);
        CloseHandle(lpThreadObj->hBlockCreateThreadEvent);
        CloseHandle(DupThreadHandle);
        CloseHandle(lpThreadObj->hThread);
        goto EXIT;        
    }

    /* adjust the stack size if necessary */
    if (0 != pthread_attr_getstacksize(&pthreadAttr, &pthreadStackSize))
    {
        ERROR("couldn't set thread stack size\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        pthread_attr_destroy(&pthreadAttr);
        PROCRemoveThread(lpThreadObj);
        CloseHandle(lpThreadObj->hBlockCreateThreadEvent);
        CloseHandle(DupThreadHandle);
        CloseHandle(lpThreadObj->hThread);
        goto EXIT;        
    }

    TRACE("default pthread stack size is %d, caller requested %d (minimum is "
          "%d)\n", pthreadStackSize, dwStackSize, MINIMAL_STACK_SIZE);

    /* don't let the stack size fall below the minimum */
    if(MINIMAL_STACK_SIZE>dwStackSize)
    {
        dwStackSize = MINIMAL_STACK_SIZE;
    }

    if(PTHREAD_STACK_MIN > pthreadStackSize)
    {
        WARN("default stack size is reported as %d, but PTHREAD_STACK_MIN is "
             "%d\n", pthreadStackSize, PTHREAD_STACK_MIN);
    }
    if(pthreadStackSize < dwStackSize)
    {
        TRACE("setting stack size to %d\n", dwStackSize);
        pthread_attr_setstacksize(&pthreadAttr, dwStackSize);
    }

#if HAVE_SOLARIS_THREADS || HAVE_THREAD_SELF || HAVE__LWP_SELF
    /* Create new threads as "bound", so each pthread is permanently bound
       to an LWP.  Get/SetThreadContext() depend on this 1:1 mapping. */
    pthread_attr_setscope(&pthreadAttr, PTHREAD_SCOPE_SYSTEM);
#endif // HAVE_SOLARIS_THREADS || HAVE_THREAD_SELF || HAVE__LWP_SELF

#if PTHREAD_CREATE_MODIFIES_ERRNO
    storedErrno = errno;
#endif  // PTHREAD_CREATE_MODIFIES_ERRNO

    /* create a pthread */
    ret = pthread_create(&pthread, &pthreadAttr, THREADEntry, lpThreadObj);

#if PTHREAD_CREATE_MODIFIES_ERRNO
    if (ret == 0)
    {
        // Restore errno if pthread_create succeeded.
        errno = storedErrno;
    }
#endif  // PTHREAD_CREATE_MODIFIES_ERRNO

    if (0 != pthread_attr_destroy(&pthreadAttr))
    {
        WARN("pthread_attr_destroy() failed\n");
    }

    if ( 0 !=ret )
    {
        ERROR("pthread_create failed, error is %d (%s)\n", ret, strerror(ret));
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        PROCRemoveThread(lpThreadObj);
        CloseHandle(lpThreadObj->hBlockCreateThreadEvent);
        CloseHandle(DupThreadHandle);
        CloseHandle(lpThreadObj->hThread);
        goto EXIT;
    }
   
    /* Note: Current implementation uses the pthread_t as
       the thread ID which is a DWORD. It works fine on a 32 bits 
       architecture, but it will need to be tested on a 64
       bits architecture. */

    /* thread ID is set to the the pthread ID (pthread_self) */
    lpThreadObj->dwThreadId = (DWORD) pthread;
    
    /* Now we have to block here until THREADEntry() signals our event, which is 
       done when our new thread is properly initialized */
    if ( WaitForSingleObject (lpThreadObj->hBlockCreateThreadEvent, INFINITE) 
         != WAIT_OBJECT_0)
    {
         ASSERT("The new thread's initialization wasn't properly waited for\n");
         /* no point in cleaning up; if this happens, who knows which 
            resources are still valid? */
         goto EXIT;
    }

    /* We won't need this event anymore, so close the handle and NULL out the
       entry */
    if ( CloseHandle(lpThreadObj->hBlockCreateThreadEvent) == FALSE)
    {
        WARN("Was not able to close the CreateThread event handle\n");
    }
    
    lpThreadObj->hBlockCreateThreadEvent = 0;

    // Make sure THREADEntry was able to start up properly.
    if(TS_FAILED == lpThreadObj->thread_state)
    {
        ERROR("error occurred in THREADEntry, thread creation failed.\n");
        CloseHandle(DupThreadHandle);
    }
    else
    {
        if(lpThreadId)
        {
            *lpThreadId = (DWORD) pthread;
        }
        hRet = DupThreadHandle;
    }

EXIT:
    LOGEXIT("CreateThread returns HANDLE %p\n", hRet);
    PERF_EXIT(CreateThread);
    return hRet;
}



/*++
Function:
  ExitThread

See MSDN doc.
--*/
PAL_NORETURN
VOID
PALAPI
ExitThread(
       IN DWORD dwExitCode)
{
      
    ENTRY("ExitThread(dwExitCode=%u)\n", dwExitCode);
    PERF_ENTRY_ONLY(ExitThread);

    /* Call entry point functions of every attached modules to
       indicate the thread is exiting */
    /* note : no need to enter a critical section for serialization, the loader 
       will lock its own critical section */
    LOADCallDllMain(DLL_THREAD_DETACH, NULL);

    TerminateCurrentThread(dwExitCode);
    
    ASSERT("TerminateCurrentThread should not return!\n");
    for (;;);
}


/*++
Function:
  SuspendThread

See MSDN doc.
--*/
DWORD
PALAPI
SuspendThread(
          IN HANDLE hThread)
{
    THREAD *lpThread = NULL, *lpCurrentThread = NULL;
    DWORD  retval=-1;
    struct pollfd fds;
    int pollRet;
    DWORD WakeupCode;
    HANDLE hCurrentThread;

    PERF_ENTRY(SuspendThread);
    ENTRY("SuspendThread(hThread=%p)\n", hThread);

    /* First, we need to retrieve the thread storage for the passed 
       in thread handle */
    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    } 
    lpThread = (THREAD*) HMGRLockHandle2 ( hThread, HOBJ_THREAD);

    /* Next we need to get the thread storage for the current thread. */
    hCurrentThread = PROCGetRealCurrentThread();
    lpCurrentThread = (THREAD*) HMGRLockHandle2( hCurrentThread, HOBJ_THREAD );
    
    /* Finally, check the validity of the objects retrieved */
    if ( (lpThread == NULL) || (lpCurrentThread == NULL)  )
    {
        ERROR("Unable to access the thread data\n");
        retval = -1;
        SetLastError(ERROR_INVALID_HANDLE);
        goto SuspendThreadExit;
    }

    if(lpThread == lpCurrentThread)
    {
        /* suspending the current thread */
        TRACE ("Self-suspending thread handle %p with pipe read\n",
               hThread);
    
        lpThread->SuspendedWithPipeFlag = TRUE;
    
        if (lpThread->bCreateSuspended)
        {
            // If bCreateSuspended is TRUE, this call to SuspendThread
            // is happening on thread creation.  We need to notify the
            // thread that created this thread that creation was successful
            // and it should continue to run.  We can't do that until we've
            // set the SuspendedWithPipeFlag, though, since that's necessary
            // for ResumeThread to succeed.
            if (SetEvent(lpThread->hBlockCreateThreadEvent) == FALSE)
            {
                ASSERT("Unable to set event to resume CreateThread()!\n");
            }
            // Clear the flag so we don't end up in this block again for
            // this thread.
            lpThread->bCreateSuspended = FALSE;
        }

        fds.fd = lpThread->blockingPipe;
        fds.events = POLLIN;
        fds.revents = 0;
        
        /* Block here */
         while(1)
         {
             pollRet = poll(&fds, 1, INFTIM);
             if(-1 != pollRet)
             {
                 break;
             }
             if(EINTR != errno)
             {
                 ASSERT("poll() failed; errno is %d (%s)\n",
                        errno, strerror(errno));
                 break;
             }
             TRACE("poll() failed with EINTR; re-polling\n");
         }
    
        /* Clear out the pipe */
        if (read(lpThread->blockingPipe, &WakeupCode, 
            sizeof(DWORD)) != sizeof(DWORD))
        {
            ASSERT("Unable to clear the thread pipe\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            retval = -1;
            goto SuspendThreadExit;
        }
    
        if (WakeupCode != WAKEUPCODE)
        {
            ASSERT("Unexpected value read from the thread pipe\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            retval = -1;
            goto SuspendThreadExit;
        }
        if(lpThread->SuspendedWithPipeFlag)
        {
            /* thread that resumed us should have cleared this */
            ASSERT("thread got pipe-resumed, but pipesuspend flag is set!\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            retval = -1;
            goto SuspendThreadExit;
        }
        /* naturally, if the thread called SuspendThread to suspend itself, it 
           wasn't suspended, therefore the previous suspension count is 0 */
        retval = 0;
    }
    else
    {
        /* suspending another thread */

        /* take the spinlock */
        while(0 != InterlockedCompareExchange(&lpThread->suspend_spinlock,1,0))
        {
            sched_yield();
        }
        
        /* Check the suspend count.  If we are already at MAXIMUM_SUSPEND_COUNT, 
           return with an error */
        if (lpThread->dwSuspendCount == MAXIMUM_SUSPEND_COUNT)
        {
            ERROR("Suspend count exceeds MAXIMUM_SUSPEND_COUNT\n");
            SetLastError(ERROR_SIGNAL_REFUSED);
            retval = -1;
            /* release the spinlock */
            lpThread->suspend_spinlock = 0;
            goto SuspendThreadExit;
        }
        
        /* return the previous suspend count on successful execution */ 
        retval = lpThread->dwSuspendCount;
        
        /* Check out current suspension count.  If we are 0, then we need to 
           actually suspend the thread... */
        if (lpThread->dwSuspendCount ==  0)
        {
            int condwRet = 0;
            int cRet = 0;

            TRACE ("Suspending thread handle %p with pthread_suspend_np\n",
                   hThread);

            if (!THREADSuspendThread(lpThread))
            {
                ERROR("Unable to suspend the thread\n");
                SetLastError(ERROR_INVALID_HANDLE);
                retval = -1;
                /* release the spinlock */
                lpThread->suspend_spinlock = 0;
                goto SuspendThreadExit;
            }

            if (TS_DONE == lpThread->thread_state)
            {
                /* We suspended a thread that was already in TS_DONE state.
                   Since thread IDs are reused, the target thread may be already 
                   gone and we may have suspended the wrong thread. Let's 
                   restart it and return a failure
                */
                if (!THREADContinueThread(lpThread))
                {
                    ASSERT("Unable to continue thread to allow it release internal critical section(s)");
                }

                ERROR("Unable to suspend the thread\n");
                SetLastError(ERROR_INVALID_HANDLE);
                retval = -1;
                /* release the spinlock */
                lpThread->suspend_spinlock = 0;
                goto SuspendThreadExit;
            }

            while (PALIsInitialized() && 
                   (TS_DONE != lpThread->thread_state) &&
                   (0 != lpThread->critsec_count))
            {
                /* Note: in addition to critsec_count, we need to check for:
                   1) PALIsInitialized() (i.e. init_count > 0): once init_count has 
                      dropped to zero (or below), critsec_count value is no longer 
                      reliable: if a thread grabs an internal critical section when 
                      init_count==1 and realeases it when init_count==0, critsec_count
                      will still be equal to 1. That may cause SuspendThread to wait
                      forever in the pthread_cond_wait loop;
                   2) TS_DONE: since we are holding a reference to the handle 
                      associated to lpThread, the target thread may potentially be
                      already gone, and the THREAD object is still alive because of
                      our reference to it. In some error condition we may end up
                      waiting forever on a not existing thread. Besides thread IDs 
                      are reused and we may end up freezing/unfreezing/waiting for 
                      the wrong thread
                */              

                /* eep, suspended thread was holding some internal critical 
                   sections. we have to let it release them all */
                TRACE ("Thread {tid=%d lwpid=%d} still holding %d critsec: restarting it\n",
                       (int)lpThread->dwThreadId, (int)lpThread->dwLwpId, 
                       (int)lpThread->critsec_count);
    
                /* make sure the thread will wait for us to suspend it 
                   again */
                pthread_mutex_lock(&lpThread->suspension_mutex);
                pthread_mutex_lock(&lpThread->cond_mutex);
                   
                /* indicate our intention to suspend this thread */
                lpThread->suspend_intent = TRUE;
    
                /* let the thread run until it releases all its critical 
                   sections */
                if (!THREADContinueThread(lpThread))
                {
                    ASSERT("Unable to continue thread for internal critsec release");
                }
                    
                /* reset the condition predicate */
                lpThread->cond_predicate = FALSE;

                /* wait for the thread to tell us it has released all its 
                   critical sections */
                /* note : this will release the mutex until the condition is
                   signalled, but that's fine, since no one can try to take 
                   it in the interval */
                while ( (FALSE == lpThread->cond_predicate) && PALIsInitialized() && 
                        (TS_DONE != lpThread->thread_state) )
                {
                    /* note: a pthread_cond_wait (or pthread_timedcond_wait) should 
                       always be used in conjunction with a boolean predicate to  
                       filter out spurious wakeups (see general Unix documentation)
                    */
                    condwRet = pthread_cond_wait(&lpThread->suspender_cond,
                                                 &lpThread->cond_mutex);

                    if (condwRet)
                    {
                        ERROR ("pthread_cond_wait returned %d [errno=%d]\n", condwRet, errno);
						break;
                    }
                }

                if (TRUE == lpThread->cond_predicate)
                {
                    /* reset the condition predicate */
                    lpThread->cond_predicate = FALSE;
                }

                if (TS_DONE == lpThread->thread_state)
                {
                    WARN ("Giving up suspending this thread since it is in TS_DONE state\n");
                }

                /* all critical sections are released, target is now blocked 
                   waiting for the suspension mutex (or about to). it is now
                   safe to suspend it */
                if ((TS_DONE == lpThread->thread_state) || condwRet || !(cRet = THREADSuspendThread(lpThread)))
                {
                    ERROR("Unable to suspend the thread {tid=%d lwpid=%d} [condwRet=%d errno=%d thread_state=%d "
                                "THREADSuspendThread()=%d PALIsInitialized()=%d]\n", 
                                (int)lpThread->dwThreadId, (int)lpThread->dwLwpId, 
                                condwRet, errno, (int)lpThread->thread_state, 
                                cRet, (int)PALIsInitialized());
                    SetLastError(ERROR_INVALID_HANDLE);
                    retval = -1;
                    lpThread->suspend_intent = FALSE;
                    /* allow target thread to acquire the mutex and continue */
                    pthread_mutex_unlock(&lpThread->cond_mutex);
                    pthread_mutex_unlock(&lpThread->suspension_mutex);
                    /* release the spinlock */
                    lpThread->suspend_spinlock = 0;
                    goto SuspendThreadExit;
                }
                    
                lpThread->suspend_intent = FALSE;
                /* allow target thread to acquire the mutex and continue 
                   when it gets resumed */
                pthread_mutex_unlock(&lpThread->cond_mutex);
                pthread_mutex_unlock(&lpThread->suspension_mutex);
            }

            /* increment our suspend count */
            (lpThread->dwSuspendCount)++;
            TRACE ("Suspend count for thread handle %p is now %u\n", 
                   hThread, lpThread->dwSuspendCount);
        }
        else
        {
            /* !lpThread->dwSuspendCount == 0 */
            (lpThread->dwSuspendCount)++;
            TRACE ("Suspend count for thread handle %p is now %u (Thread was "
                   "already suspended)\n", 
                   hThread, lpThread->dwSuspendCount);
        }
        /* add one to the suspension count if the thread is also self-suspended. 
           do this after suspending, to avoid a race if target is in the process 
           of self-suspending */
        if(lpThread->SuspendedWithPipeFlag)
        {
            retval++;
        }

        /* release the spinlock */
        lpThread->suspend_spinlock = 0;
    }

SuspendThreadExit: 
    if (lpThread)
    {
        HMGRUnlockHandle(hThread,&lpThread->objHeader);
    }
    if(lpCurrentThread)
    {
        HMGRUnlockHandle(hCurrentThread,&lpCurrentThread->objHeader);
    }
    LOGEXIT("SuspendThread returns DWORD %u\n", retval);
    PERF_EXIT(SuspendThread);
    return (retval);
}


/*++
Function:
  ResumeThread

See MSDN doc.
--*/
DWORD
PALAPI
ResumeThread(
         IN HANDLE hThread)
{
    THREAD *lpThread = NULL;
    DWORD  retval=-1;

    PERF_ENTRY(ResumeThread);
    ENTRY("ResumeThread(hThread=%p)\n", hThread);
    
    /* First, we need to retrieve our local thread storage */
    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    }
    lpThread = (THREAD*) HMGRLockHandle2 ( hThread, HOBJ_THREAD );

    if (lpThread == NULL)
    {
        ERROR("Unable to access the thread data\n");
        SetLastError(ERROR_INVALID_HANDLE);
        retval = -1;
        goto ResumeThreadExit;
    }

    if(TS_DUMMY == lpThread->thread_state)
    {
        /* resuming a process created with CREATE_SUSPENDED */
        if(-1 == lpThread->blockingPipe)
        {
            /* most likely cause of this is application error, either 
              CREATE_SUSPENDED wasn't used of the process was already resumed */
            ERROR("trying to wake up dummy thread without a blocking pipe\n");
            SetLastError(ERROR_INVALID_HANDLE);
        }
        else
        {
            int ret;

            TRACE("sending WAKEUPCODE to dummy thread %p\n", hThread);
            ret = write(lpThread->blockingPipe, &WAKEUPCODE, sizeof(DWORD));
            if(sizeof(DWORD) != ret)
            {
                if(EPIPE == errno)
                {
                    /* most likely cause is that target process got killed 
                       before we got here, so the pipe got broken */
                    ERROR("write() failed with EPIPE\n");
                    SetLastError(ERROR_INVALID_HANDLE);
                }
                else
                {
                    /* no other errors are expected */
                    ASSERT("write() failed; error is %d (%s)\n", 
                           errno, strerror(errno));
                    SetLastError(ERROR_INTERNAL_ERROR);
                }
            }
            else
            {
                retval = 1;
            }
            close(lpThread->blockingPipe);
            lpThread->blockingPipe = -1;
        }
        goto ResumeThreadExit;
    }

    /* take the spinlock */
    while(0 != InterlockedCompareExchange(&lpThread->suspend_spinlock,1,0))
    {
        sched_yield();
    }
    
    /* see if the target thread has self-suspended itself. use InterlockedCE 
       even though we're holding the spinlock, because the target thread doesn't
       (and musn't) use the spinlock. Therefore we have to make the comparison 
       and set the new value atomically */
    if(InterlockedCompareExchange(&lpThread->SuspendedWithPipeFlag, FALSE,TRUE)
       == TRUE )
    {
        TRACE ("Resuming thread handle %p with a blocked pipe write\n",
                hThread);
        if (write(lpThread->blockingPipe, &WAKEUPCODE, sizeof(DWORD)) 
            != sizeof(DWORD))
        {
            ASSERT("Unable to write in the pipe to wakeup the thread\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            retval = -1;
            /* release the spinlock */
            lpThread->suspend_spinlock = 0;
            goto ResumeThreadExit;
        }
        /* previous suspend count is non-pipe suspend count pls 1 for the 
           pipe-suspension */
        retval = lpThread->dwSuspendCount+1;
    }
    else
    {
        /* thread isn't self-suspended */
        
        /* Check the suspend count.  If we are ALREADY at 0, return immediately
           with no error. */
        if (lpThread->dwSuspendCount == 0)
        {
            retval = 0;
            /* release the spinlock */
            lpThread->suspend_spinlock = 0;
            goto ResumeThreadExit;
        }
        
        /* Decrement the suspend count. */
        retval = lpThread->dwSuspendCount;   
        lpThread->dwSuspendCount--;
        TRACE ("Suspend count for thread handle %p is now %u\n",
                hThread, lpThread->dwSuspendCount);

        /* if we've reached zero, resume the thread */
        if (lpThread->dwSuspendCount == 0)
        {
            TRACE ("Resuming thread handle %p with pthread_resume_np()\n",
                    hThread);

            if (!THREADContinueThread(lpThread))
            {
                ERROR("Unable to resume the thread\n");
                SetLastError(ERROR_INVALID_HANDLE);
                retval = -1;
                /* release the spinlock */
                lpThread->suspend_spinlock = 0;
                goto ResumeThreadExit;
            } 
        }
    }
    /* release the spinlock */
    lpThread->suspend_spinlock = 0;

ResumeThreadExit:
    if (lpThread)
    {
        HMGRUnlockHandle(hThread,&lpThread->objHeader);
    }
    LOGEXIT("ResumeThread returns DWORD %u\n", retval);
    PERF_EXIT(ResumeThread);
    return (retval);
}


/*++
Function:
  TerminateCurrentThread

Does any necessary memory clean up, signals waiting threads, and then forces
the current thread to exit.
--*/
VOID
TerminateCurrentThread(
        IN DWORD dwExitCode)
{
    HANDLE hThread;
    THREAD *lpThread;
    ThreadWaitingList *pWaitingThread;
    ThreadWaitingList *pPrevThread;
    ThreadWaitingList *pTempThread;
    DWORD dwThreadId;

    /* Get the real handle for the current thread */
    hThread = PROCGetRealCurrentThread();

    MutexAbandonMutexes();

    /* Retreive the thread object */
    lpThread = (THREAD *) HMGRLockHandle2(hThread, HOBJ_THREAD);

    if ( lpThread == NULL)
    {
        ERROR("Thread handle is invalid\n");
        SetLastError(ERROR_INVALID_HANDLE);
        return;
    }


    /* store the exit code */
    lpThread->dwExitCode = dwExitCode;

    /* signal all waiting threads */
    SYNCEnterCriticalSection(&lpThread->thread_crit_section, TRUE);
    lpThread->thread_state = TS_DONE;
    
    pPrevThread = NULL;
    pWaitingThread = lpThread->waitingThreads;
    while(NULL != pWaitingThread)
    {
        /* only awaken the thread if no one else has awakened it yet. if the 
           timing is right, the thread may have awoken but not have had time 
           to remove itself from all waiting lists */
        if(!THREADInterlockedAwaken(pWaitingThread->state.pAwakened, FALSE))
        {
            TRACE("thread is already awake, skipping it\n");
            pPrevThread = pWaitingThread;
            pWaitingThread = pWaitingThread->ptr.Next;
        }
        else
        {                        
            /* remove the thread from the waiting list , wake up the thread */
            if(NULL == pPrevThread)
            {
                lpThread->waitingThreads = pWaitingThread->ptr.Next;
            }
            else
            {
                pPrevThread->ptr.Next = pWaitingThread->ptr.Next;
            }
            WakeUpThread(pWaitingThread->threadId, pWaitingThread->processId, 
                         pWaitingThread->blockingPipe, WUTC_SIGNALED);

            pTempThread = pWaitingThread;
            pWaitingThread = pWaitingThread->ptr.Next;
            free(pTempThread);
        }
    }              
    
    SYNCLeaveCriticalSection(&lpThread->thread_crit_section, TRUE);

    /* store the threadId in a local variable, since CloseHandle()
       could potentially delete the thread object(lpThread) */
    dwThreadId = lpThread->dwThreadId;

    // If this is the last thread then delete the process' data,
    // but don't exit because the application hosting the PAL
    // might have its own threads.
    if ( PROCGetNumberOfThreads() == 1 )
    {
        TRACE("Last thread is exiting\n");
        TerminateCurrentProcessNoExit(FALSE);
    }
    else
    {
        /* Do this ONLY if we aren't the last thread -> otherwise
           it gets done by TerminateProcess->
           PROCCleanupProcess->PALShutdown->PAL_Terminate */

        /* Remove thread for the thread list of the process 
           (don't do if this is the last thread -> gets handled by
            TerminateProcess->PROCCleanupProcess->PROCTerminateOtherThreads) */
        PROCRemoveThread(lpThread);

        /* close the handle, and destroy the thread object if it's
           the last opened handle on it */
        if (CloseHandle(lpThread->hThread) == 0)
        {
            HMGRUnlockHandle(hThread,&lpThread->objHeader);
            ASSERT("Unable to close the thread handle\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            return;
        }

        HMGRUnlockHandle(hThread,&lpThread->objHeader);
    }

    /* kill the thread (itself) */
    pthread_exit(NULL);
}

/*++
Function:
  QueueUserAPC

See MSDN doc.

NOTE: There are no error values defined for this function that can be retrieved
      by calling GetLastError (accroding to MSDN)
--*/
DWORD
PALAPI
QueueUserAPC(
             IN PAPCFUNC pfnAPC,
             IN HANDLE hThread,
             IN ULONG_PTR dwData)
{
    THREAD *lpThread = NULL;
    LPAPC_TRACKER tmpAPCTrack=NULL;
    int retval = 0; 
    DWORD *pThreadWaitState;

    PERF_ENTRY(QueueUserAPC);
    ENTRY("QueueUserAPC(pfnAPC=%p, hThread=%p, dwData=%#x)\n", 
          pfnAPC, hThread, dwData);
    
    /* NOTE: Windows does not check the validity of pfnAPC, even if it is 
       NULL.  It just does an access violation later on when the APC call 
       is attempted */                 

    /* We need to retrieve our local thread storage */
    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    }
    lpThread = (THREAD*) HMGRLockHandle2 (hThread, HOBJ_THREAD);
    if ( lpThread == NULL)
    {
        ERROR("Unable to access the thread data\n");
        goto done;
    }
    SYNCEnterCriticalSection (&(lpThread->thread_crit_section), TRUE);
    if(TS_DONE == lpThread->thread_state)
    {
        ERROR("Thread %#x has terminated; can't queue an APC on it\n",
              lpThread->dwThreadId);
        SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);
        goto done;
    }                     
    
    /* Now that we have the local thread storage structure, lets allocate a 
       new entry in the list of queued APCs */
    if (lpThread->lpAPCTracker == NULL)
    {
        lpThread->lpAPCTracker = (LPAPC_TRACKER) malloc(sizeof(APC_TRACKER));

        if (!lpThread->lpAPCTracker)
        {
            ERROR("Unable to create new entry in queued APCs linked list\n");
            SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);
            goto done;
        }

        tmpAPCTrack = lpThread->lpAPCTracker;
        memset(tmpAPCTrack, 0, sizeof(APC_TRACKER));
    }
    else
    {
        tmpAPCTrack = lpThread->lpAPCTracker;
        while (tmpAPCTrack->pNext != NULL)
        {
            tmpAPCTrack = tmpAPCTrack->pNext;
        }

        tmpAPCTrack->pNext = (LPAPC_TRACKER) malloc(sizeof(APC_TRACKER));

        if (!tmpAPCTrack->pNext)
        {
            ERROR("Unable to create new entry in queued APCs linked list\n");
            SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);
            goto done;
        }

        tmpAPCTrack = tmpAPCTrack->pNext;
        memset(tmpAPCTrack, 0, sizeof(APC_TRACKER));
    }

    
    /* Finally, now that we have an entry in the queued APC linked list, 
       add our new function pointer and its function parameter to the 
       list */
    tmpAPCTrack->pfnAPC = pfnAPC;
    tmpAPCTrack->pAPCData = dwData;
    TRACE ("APC %p with parameter %#x added to APC queue\n", 
          tmpAPCTrack->pfnAPC, tmpAPCTrack->pAPCData);
    
    /* Now that we have queued the APC(es), we need to wake up the
       thread if it is already blocked in WaitForMultipleObjects() 
       (in WaitThread()) and is alertable 
       Note : we can safely assume that our newly queued APC hasn't yet been 
       executed when we get here, since we are holding the thread's critical
       section
    */
    pThreadWaitState = SHMPTR_TO_PTR(lpThread->waitAwakened);

    if(THREADInterlockedAwaken(pThreadWaitState, TRUE))
    {
        WakeUpThread(lpThread->dwThreadId, GetCurrentProcessId(), 
                     lpThread->blockingPipe, WUTC_APC_QUEUED);
    }
    SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);
 
    retval = 1;

done:
    if(NULL!=lpThread)
    {
        HMGRUnlockHandle(hThread,&lpThread->objHeader);
    }
    LOGEXIT("QueueUserAPC returns DWORD %d\n",retval);
    PERF_EXIT(QueueUserAPC);
    return retval;
}


/*++
Function:
    THREADCallThreadAPCs

Abstract:
    Call any outstanding APC functions queued on the current thread  

Parameters:
    VOID

Return:
    Error:   -1
    Success: Number of APCs called.
--*/
int THREADCallThreadAPCs(void)
{
    THREAD *lpThread = NULL;
    LPAPC_TRACKER lpAPCTrack=NULL;
    int APCsCalled=0;

    /* Go through each entry in the linked list, call the function, 
       then free the memory */
    for (;;)
    {
        lpThread = PROCGetCurrentThreadObject();
        if (lpThread == NULL)
        {
            ASSERT("Unable to access the current thread\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            return (-1);
        }

        SYNCEnterCriticalSection (&(lpThread->thread_crit_section), TRUE);

        lpAPCTrack = lpThread->lpAPCTracker;
        if (lpAPCTrack != NULL)
        {
            /* unlink the first node of lpAPCTrack */
            lpThread->lpAPCTracker = lpAPCTrack->pNext;
        }

        /* Release thread CS before calling the APC call */
        SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);

        if (lpAPCTrack == NULL)
            break;

/* reset ENTRY nesting level back to zero while inside the callback... */
#if !_NO_DEBUG_MESSAGES_
    {
        int old_level;
        old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

        TRACE ("Calling APC %p with parameter %#x\n",
               lpAPCTrack->pfnAPC, lpAPCTrack->pAPCData);

        /* Actual APC call */
        lpAPCTrack->pfnAPC(lpAPCTrack->pAPCData);

/* ...and set nesting level back to what it was */
#if !_NO_DEBUG_MESSAGES_
        DBG_change_entrylevel(old_level);
    }
#endif /* !_NO_DEBUG_MESSAGES_ */

        /* free the memory */
        free(lpAPCTrack);

        APCsCalled++;
    }

    return (APCsCalled);
}

/*++
Function:
  GetThreadPriority

See MSDN doc.
--*/
int
PALAPI
GetThreadPriority(
          IN HANDLE hThread)
{
    THREAD *lpThread = NULL;
    int retval = THREAD_PRIORITY_ERROR_RETURN;
    
    PERF_ENTRY(GetThreadPriority);
    ENTRY("GetThreadPriority(hThread=%p)\n", hThread);

    /* First, we need to retrieve our local thread storage */
    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    }
    lpThread = (THREAD*) HMGRLockHandle2(hThread, HOBJ_THREAD);
    if ( lpThread == NULL)
    {
        ERROR("Unable to access the thread data\n");
        SetLastError(ERROR_INVALID_HANDLE);
        goto GetThreadPriorityExit;
    }
    
    SYNCEnterCriticalSection (&(lpThread->thread_crit_section), TRUE);

    retval = lpThread->threadPriority;

    SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);

GetThreadPriorityExit:
    if (lpThread)
    {
        HMGRUnlockHandle(hThread,&lpThread->objHeader);
    }
    LOGEXIT("GetThreadPriorityExit returns int %d\n", retval);
    PERF_EXIT(GetThreadPriority);
    return (retval);
}


/*++
Function:
  SetThreadPriority

See MSDN doc.
--*/
BOOL
PALAPI
SetThreadPriority(
          IN HANDLE hThread,
          IN int nPriority)
{
    THREAD *lpThread = NULL;
    BOOL retval = FALSE;
    int policy;
    struct sched_param schedParam;
    int max_priority;
    int min_priority;
    float posix_priority;


    PERF_ENTRY(SetThreadPriority);
    ENTRY("SetThreadPriority(hThread=%p, nPriority=%#x)\n", hThread, nPriority);

    /* First, we need to retrieve our local thread storage */
    if(hThread == hPseudoCurrentThread)
    {
        hThread = PROCGetRealCurrentThread();
    }
    lpThread = (THREAD*) HMGRLockHandle2 (hThread, HOBJ_THREAD);
    if (lpThread == NULL)
    {
        ERROR("Unable to access the thread data\n");
        SetLastError(ERROR_INVALID_HANDLE);
        goto SetThreadPriorityExit;
    }
    
    SYNCEnterCriticalSection (&(lpThread->thread_crit_section), TRUE);

    /* validate the requested priority */
    switch (nPriority)
    {
    case THREAD_PRIORITY_TIME_CRITICAL: /* fall through */
    case THREAD_PRIORITY_IDLE:          /* fall through */
        break;

    case THREAD_PRIORITY_HIGHEST:       /* fall through */
    case THREAD_PRIORITY_ABOVE_NORMAL:  /* fall through */
    case THREAD_PRIORITY_NORMAL:        /* fall through */
    case THREAD_PRIORITY_BELOW_NORMAL:  /* fall through */
    case THREAD_PRIORITY_LOWEST:        /* fall through */
#if PAL_IGNORE_NORMAL_THREAD_PRIORITY
        /* We aren't going to set the thread priority. Just record what it is,
           and exit */
        lpThread->threadPriority = nPriority;
        retval = TRUE;
        goto SetThreadPriorityExit;        
#endif
        break;

    default:
        ASSERT("Priority %d not supported\n", nPriority);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto SetThreadPriorityExit;
    }  

    /* check if the thread is still running */
    if ( TS_DONE == lpThread->thread_state )
    {
        /* the thread has exited, set the priority in the thread structure 
           and exit */
        lpThread->threadPriority = nPriority;
        retval = TRUE;
        goto SetThreadPriorityExit;        
    }

    /* get the previous thread schedule parameters.  We need to know the 
       scheduling policy to determine the priority range */
    if (pthread_getschedparam((pthread_t)lpThread->dwThreadId, &policy, 
        &schedParam) != 0)
    {
        ASSERT("Unable to get current thread scheduling information\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto SetThreadPriorityExit;
    }

#if HAVE_SCHED_GET_PRIORITY
    max_priority = sched_get_priority_max(policy);
    min_priority = sched_get_priority_min(policy);
    if( -1 == max_priority || -1 == min_priority)
    {
        ASSERT("sched_get_priority_min/max failed; error is %d (%s)\n", 
               errno, strerror(errno));
        SetLastError(ERROR_INTERNAL_ERROR);
        goto SetThreadPriorityExit;
    }
#else
    max_priority = PAL_THREAD_PRIORITY_MAX;
    min_priority = PAL_THREAD_PRIORITY_MIN;
#endif

    TRACE("Pthread priorities for policy %d must be in the range %d to %d\n", 
          policy, min_priority, max_priority);

    /* explanation for fancy maths below :
       POSIX doesn't specify the range of thread priorities that can be used 
       with pthread_setschedparam. Instead, one must use sched_get_priority_min
       and sched_get_priority_max to obtain the lower and upper bounds of this
       range. Since the PAL also uses a range of values (from Idle [-15] to 
       Time Critical [+15]), we have to do a mapping from a known range to an 
       unknown (at compilation) range. 
       We do this by :
       -substracting the minimal PAL priority from the desired priority. this 
        gives a value between 0 and the PAL priority range
       -dividing this value by the PAL priority range. this allows us to 
        express the desired priority as a floating-point value between 0 and 1
       -multiplying this value by the PTHREAD priority range. This gives a 
        value between 0 and the PTHREAD priority range
       -adding the minimal PTHREAD priority range. This will give us a value 
        between the minimal and maximla pthread priority, which should be 
        equivalent to the original PAL value. 
        
        example : suppose a pthread range 100 to 200, and a desired priority 
                  of 0 (halfway between PAL minimum and maximum)
            0 - (IDLE [-15]) = 15
            15 / (TIMECRITICAL[15] - IDLE[-15]) = 0.5
            0.5 * (pthreadmax[200]-pthreadmin[100]) = 50
            50 + pthreadmin[100] = 150 -> halfway between pthread min and max
    */
    posix_priority =  (nPriority - THREAD_PRIORITY_IDLE);
    posix_priority /= (THREAD_PRIORITY_TIME_CRITICAL - THREAD_PRIORITY_IDLE);
    posix_priority *= (max_priority-min_priority);
    posix_priority += min_priority;

    schedParam.sched_priority = (int)posix_priority;
    
    TRACE("PAL priority %d is mapped to pthread priority %d\n",
          nPriority, schedParam.sched_priority);

    /* Finally, set the new priority into place */
    if (pthread_setschedparam((pthread_t)lpThread->dwThreadId , policy, 
        &schedParam) != 0)
    {
#if SET_SCHEDPARAM_NEEDS_PRIVS
        if (EPERM == errno)
        {
            // ROTORTODO: Should log a warning to the event log
            TRACE("Caller does not have OS privileges to call pthread_setschedparam\n");
            lpThread->threadPriority = nPriority;
            retval = TRUE;
            goto SetThreadPriorityExit;
        }
#endif
        
        ASSERT("Unable to set thread priority (errno %d)\n", errno);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto SetThreadPriorityExit;
    }
    lpThread->threadPriority = nPriority;
    retval = TRUE;

SetThreadPriorityExit:
    if (lpThread)
    {
        SYNCLeaveCriticalSection (&(lpThread->thread_crit_section), TRUE);
        HMGRUnlockHandle(hThread,&lpThread->objHeader);
    }
    LOGEXIT("SetThreadPriority returns BOOL %d\n", retval);
    PERF_EXIT(SetThreadPriority);
    return (retval);
}


/*++
Function:
    THREADEntry

Abstract:
    Startup routine for every thread created with CreateThread function.

Parameter:
    IN lpThread: pointer to the object containing all thread information.

Return:
    void *
    
--*/
static 
void *
THREADEntry(
            void *lpParam)
{
    DWORD retValue;
    THREAD *lpThread;
    PROCESS *currentProcess;

    lpThread = (THREAD *) lpParam;

    pthread_cleanup_push((void (__cdecl *)(void *))TLSCacheFlush, lpThread);

    if(NULL == lpThread)
    {
        ASSERT("THREAD pointer is NULL!\n");
        goto fail;
    }

    lpThread->dwThreadId = (DWORD) pthread_self();
#if HAVE_THREAD_SELF
    lpThread->dwLwpId = (DWORD) thread_self();
#elif HAVE__LWP_SELF
    lpThread->dwLwpId = (DWORD) _lwp_self();
#else
    lpThread->dwLwpId = 0;
#endif

    /* We need to set the thread object in our thread-local storage,
       which we use to improve the performance of getting the current
       thread object. */
    currentProcess = pCurrentProcess;
    if (currentProcess == NULL)
    {
        ASSERT("Unable to access the current process\n");
        goto fail;
    }

    if (pthread_setspecific(thObjKey, lpThread))
    {
        ASSERT("Unable to set the thread object key's value\n");
        goto fail;
    }

    /* create the pipe used to block/unblock the thread, passing the 
       thread ID of the newly created thread. */
    lpThread->blockingPipe = createBlockingPipe(lpThread->dwThreadId);
    if(lpThread->blockingPipe == -1) 
    {
        ERROR("couldn't create the new thread's blocking pipe!\n");
        goto fail;
    }                                 


    // Check if the thread should be started suspended.
    if (lpThread->bCreateSuspended)
    {
        SuspendThread(lpThread->hThread);

        /* Now we need to call the APCs that are already queued up
           for the function by QueueUserAPC */
        if (THREADCallThreadAPCs() == -1)
        {
            ERROR("Failed in calling APCs for the current thread\n");
        }
    }
    else
    {
        // All startup operations that might have failed have succeeded,
        // so thread creation is successful. Let CreateThread return.
        if (SetEvent(lpThread->hBlockCreateThreadEvent) == FALSE)
        {
            ASSERT("Unable to set event to resume CreateThread()!\n");
        }
    }

    lpThread->thread_state = TS_RUNNING;
    /* Inform all loaded modules that a thread has been created */
    /* note : no need to take a critical section to serialize here; the loader 
       will take the module critical section */
    LOADCallDllMain(DLL_THREAD_ATTACH, NULL);

    /* call the startup routine */
    retValue = lpThread->lpStartAddress(lpThread->lpStartParameter);

    TRACE("Thread exited (%u)\n", retValue);
    ExitThread(retValue);

    /* Note: never get here */ 
    ASSERT("ExitThread failed!\n");
    for (;;);

fail:
    /* tell CreateThread() this thread failed to start, then exit */
    lpThread->thread_state = TS_FAILED;
    SetEvent(lpThread->hBlockCreateThreadEvent);
    PROCRemoveThread(lpThread);
    CloseHandle(lpThread->hThread);

    pthread_cleanup_pop(1);

    /* do not call ExitThread : we don't want to call DllMain(), and the thread 
       isn't in a clean state (e.g. lpThread isn't in TLS). the cleanup work 
       above should release all resources */
    return NULL;
}


/*++
Function:
    THREADCreateInitialThread

Abstract:
    Create the initial thread object
  
Parameters:
    void

Return:
   the THREAD object.
--*/
THREAD *
THREADCreateInitialThread(
    void)
{
    THREAD *lpThreadObj;
    LPBOOL pAwakened;
    
    /* Create the thread object */
    lpThreadObj = AllocTHREAD();

    if (!lpThreadObj) 
        return NULL;

    /* fill the thread structure */
    lpThreadObj->objHeader.type = HOBJ_THREAD;
    lpThreadObj->objHeader.close_handle = closeThreadHandle; 
    lpThreadObj->objHeader.dup_handle = dupThreadHandle;
    lpThreadObj->refCount = 1;
    lpThreadObj->thread_state = TS_RUNNING;
    lpThreadObj->hThread = HMGRGetHandle((HOBJSTRUCT *) lpThreadObj);
    lpThreadObj->critsec_count = 1;
    lpThreadObj->lastError = StartupLastError;
    lpThreadObj->minStack = (LPVOID)-1;
    lpThreadObj->safe_state = TRUE;
    // everything else is zero-filled by AllocTHREAD

    if ( INVALID_HANDLE_VALUE == lpThreadObj->hThread)
    {
        FreeTHREAD(lpThreadObj);
        return NULL;
    }

    lpThreadObj->waitAwakened = SHMalloc(sizeof(DWORD));
    pAwakened = SHMPTR_TO_PTR(lpThreadObj->waitAwakened);
    if(NULL == pAwakened)
    {
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        return NULL;
    }
    *pAwakened = TWS_ACTIVE;


    lpThreadObj->dwThreadId = (DWORD) pthread_self();
#if HAVE_THREAD_SELF
    lpThreadObj->dwLwpId = (DWORD) thread_self();
#elif HAVE__LWP_SELF
    lpThreadObj->dwLwpId = (DWORD) _lwp_self();
#else
    lpThreadObj->dwLwpId = 0;
#endif
    lpThreadObj->blockingPipe = createBlockingPipe(GetCurrentThreadId());
    lpThreadObj->threadPriority = THREAD_PRIORITY_NORMAL;
    lpThreadObj->dwSuspendCount = 0;
    lpThreadObj->SuspendedWithPipeFlag = FALSE;
    lpThreadObj->bCreateSuspended = FALSE;

    if (lpThreadObj->blockingPipe == -1)
    {
        ASSERT("Could not create a blocking pipe\n");
        SHMfree(lpThreadObj->waitAwakened);
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        return NULL;
    }                                      

    if (0 != SYNCInitializeCriticalSection(&(lpThreadObj->thread_crit_section)))
    {
        ERROR("SYNCInitializeCriticalSection failed!\n");
        SHMfree(lpThreadObj->waitAwakened);
        destroyBlockingPipe(lpThreadObj->blockingPipe, lpThreadObj->dwThreadId, TWS_NONE);
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        lpThreadObj = NULL;
    }

    if(!init_suspender(lpThreadObj))
    {
        ERROR("couldn't initialize suspender objects!\n");
        DeleteCriticalSection(&lpThreadObj->thread_crit_section);
        SHMfree(lpThreadObj->waitAwakened);
        destroyBlockingPipe(lpThreadObj->blockingPipe, lpThreadObj->dwThreadId, TWS_NONE);
        HMGRFreeHandle(lpThreadObj->hThread);
        FreeTHREAD(lpThreadObj);
        lpThreadObj = NULL;
    }                                                

    return lpThreadObj;
}


/*++
Function:
    THREADGetThreadPipeName

Abstract:
    Generate the name of a thread's event pipe fomr its PID and TID

Parameters:
    LPSTR dest  : buffer in which file name is placed
    int dest_size : size of destination buffer (should be at least MAX_PATH)
    DWORD pid   : process ID of thread
    DWORD tid   : thread ID of thread

Return:
    TRUE on success, FALSE on failure
--*/
BOOL THREADGetThreadPipeName(LPSTR dest, int dest_size, DWORD pid, DWORD tid)
{
    CHAR config_dir[MAX_PATH];
    int needed_size;

    if(NULL == dest)
    {
        ASSERT("destination pointer is NULL!\n");
        return FALSE;
    }
    if(0 >= dest_size)
    {
        ASSERT("invalid buffer size %d\n", dest_size);
        return FALSE;
    }
    if ( !PALGetPalConfigDir( config_dir, MAX_PATH ) )
    {
        ASSERT( "Unable to determine the PAL config directory.\n" );
        dest[ 0 ] = '\0';
        return FALSE;
    }
    needed_size = snprintf(dest, dest_size, "%s/%s-%u-%u", config_dir, 
                           THREADPIPE_PREFIX, pid, tid);
    if(needed_size >= dest_size)
    {
        ERROR("threadpipe name needs %d characters, buffer only has room for "
              "%d\n", needed_size, dest_size+1);
        return FALSE;
    }
    return TRUE;
}


/*++
Function:
  ThreadWaitOn

  Check if the thread has termimated. If still running, place
  the current thread to the list of waiting thread. This function is
  called by WaitXXXX functions when it's time to wait on a Thread.
  The caller is responsible for blocking the thread, this function does
  not wait on the thread.

Parameters
    IN hThread   thread checked to see if it is running
    SHMPTR wait_state : shared memory pointer to waiting thread's wait state

returns
    WAITON_CODE value
--*/
int
ThreadWaitOn(
    IN HANDLE hThread, SHMPTR wait_state)
{
    THREAD *pThread;
    BOOL ret;
    ThreadWaitingList *pWaitingThread;

    pThread = (THREAD *) HMGRLockHandle2(hThread, HOBJ_THREAD);

    if ( pThread == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        ASSERT("Invalid thread handle %p\n", hThread);
        return WOC_ERROR;
    }

    SYNCEnterCriticalSection(&pThread->thread_crit_section, TRUE);

    
    if ( TS_DONE == pThread->thread_state )
    {
        DWORD *pAwakenState;

        /* try to mark thread as awake; this fails of another object has been 
           signaled and awoke the thread before us. */
        pAwakenState = SHMPTR_TO_PTR(wait_state);
        
        if(!THREADInterlockedAwaken(pAwakenState,FALSE))
        {
            ret = WOC_INTERUPTED;
            TRACE("thread is already awake! race condition averted!\n");
        }
        else
        {
            ret = WOC_SIGNALED;
            TRACE("Thread %#x is terminated, no wait\n", pThread->dwThreadId);
        }
        goto done;
    }
    /* add the current thread to the list of waiting threads */
    pWaitingThread = (ThreadWaitingList *) 
                            malloc(sizeof(ThreadWaitingList));

    if (pWaitingThread == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        ERROR("Not enough memory to allocate a ThreadQueue\n");
        ret = WOC_ERROR;
        goto done;
    }
            
    pWaitingThread->threadId = GetCurrentThreadId();
    pWaitingThread->processId = GetCurrentProcessId();
    pWaitingThread->blockingPipe = THREADGetPipe();
    pWaitingThread->state.pAwakened = SHMPTR_TO_PTR(wait_state);

    TRACE("ThreadId=%#x will wait on thread=%#x\n", 
          pWaitingThread->threadId, pThread->dwThreadId);

    /* when a thread terminates, we wake up all threads waiting on it. there's 
       no need to maintain a FIFO for this, so we can save the new thread at 
       the head of the list. */
    pWaitingThread->ptr.Next = pThread->waitingThreads;
    pThread->waitingThreads = pWaitingThread;

    TRACE("Thread 0x%08x is still running; current thread added to the "
          "waiting list\n", pThread->dwThreadId);

    ret = WOC_WAITING;

done:
    SYNCLeaveCriticalSection(&pThread->thread_crit_section, TRUE);
    HMGRUnlockHandle(hThread,&pThread->objHeader);

    return ret;
}

/*++
Function:
  ThreadRemoveWaitingThread

  Remove the CurrentThreadId from the list of waiting thread. This
  function is called when the current thread stops waiting on a Thread
  for a different reason than the Thread terminating. (e.g. a timeout,
  or the thread was waiting on multiple objects and another object was
  signaled)

Parameters
    IN hThread   thread whose waiting thread list is to be updated

returns
    -1: an error occurred
    0: if the thread wasn't found in the list.
    1: if the thread was removed from the waiting list.
--*/
int
ThreadRemoveWaitingThread(
    IN HANDLE hThread)
{
    THREAD *pThread;
    int ret = 0;
    ThreadWaitingList *pWaitingThread;
    ThreadWaitingList *pNextWaitingThread;
    DWORD CurrentThreadId;

    pThread = (THREAD *) HMGRLockHandle2(hThread, HOBJ_THREAD);

    if (NULL == pThread)
    {
        ERROR("Invalid thread handle %p\n", hThread);
        return -1;
    }

    SYNCEnterCriticalSection(&pThread->thread_crit_section, TRUE);

    CurrentThreadId = GetCurrentThreadId();
    pWaitingThread = pThread->waitingThreads;

    if (pWaitingThread == NULL)
    {        
        ret = 0;  /* list is empty */
        goto RemoveThreadExit;
    }

    /* check if it is the first element in the list */
    if (pWaitingThread->threadId == CurrentThreadId)
    {
        pThread->waitingThreads = pWaitingThread->ptr.Next;
        free(pWaitingThread);
        ret = 1;
        goto RemoveThreadExit;
    }
    else
    {
        while (pWaitingThread->ptr.Next)
        {
            pNextWaitingThread = pWaitingThread->ptr.Next;

            if (pNextWaitingThread->threadId == CurrentThreadId)
            {
                /* found, so remove it */
                pWaitingThread->ptr.Next = pNextWaitingThread->ptr.Next;
                free (pNextWaitingThread);
                ret = 1;
                break;
            }

            pWaitingThread = pNextWaitingThread; 
        }
    }

RemoveThreadExit:
    SYNCLeaveCriticalSection(&pThread->thread_crit_section, TRUE);
    HMGRUnlockHandle(hThread,&pThread->objHeader);

    if (ret == 0)
    {
        TRACE("ThreadId=%#x was not waiting on hThread=%p\n",
             CurrentThreadId, hThread);
    }
    else
    {
        TRACE("ThreadId=%#x is no longer waiting on hThread=%p\n",
              CurrentThreadId, hThread);
    }

    return ret;
}



/* static function definitions ************************************************/

/*++
Function:
  createBlockingPipe

Abstract:
    Create a pipe used for blocking/unblocking the thread. The pipe
    has the following format:
    <pipe prefix>-<process ID>-<thread id>. 
    Having a fixed format gives the chance to any thread from any 
    process to accces the pipe.
  
Parameters:
    DWORD tid: The thread ID that the pipe is being made for. (this 
               is usually different from the current thread)

Return:
    created pipe 
    -1: error
--*/
static 
int 
createBlockingPipe(
      DWORD tid)
{
    int pipe;
    char PipeFilename[MAX_PATH];

    /* Create the blocking pipe */
    if(FALSE == THREADGetThreadPipeName(PipeFilename, MAX_PATH, 
                                        GetCurrentProcessId(),tid))
    {
        ERROR("couldn't get threadpipe's name\n");
        return -1;
    }

    /* create the pipe, with full access to the owner only */
    if ( mkfifo(PipeFilename, S_IRWXU ) == -1 )
    {
        if ( errno == EEXIST )
        {
            /* Some how no one deleted the pipe, perhaps it was left behind
            from a crash?? Delete the pipe and try again. */
            if ( -1 == unlink( PipeFilename ) )
            {
                ERROR( "Unable to delete the blocking pipe that was left behind.\n" );
                return -1;
            }
            else
            {
                if ( mkfifo(PipeFilename, S_IRWXU | S_IRWXG | S_IRWXO ) == -1 )
                {
                    ERROR( "Still unable to create the blocking pipe...giving up!\n" );
                    return -1;
                }
            }
        }
        else
        {
            ERROR( "Unable to create the blocking pipe.\n" );
            return -1;
        }
    }

    pipe = open(PipeFilename, O_RDWR | O_NONBLOCK);

    if (pipe == -1)
    {
        ERROR("Unable to open the thread pipe for read\n");
        unlink(PipeFilename);
        return -1;
    }

    return pipe;
}

/*++
Function:
  destroyBlockingPipe

Abstract:
    Destroy the pipe used for blocking/unblocking the thread. The pipe
    has the following format:
    <pipe prefix>-<process ID>-<thread id>. 
    Having a fixed format gives the chance to any thread from any 
    process to accces the pipe.
  
Parameters:
    int pipe:   pipe to close
    DWORD ThreadID:  thread owning the pipe
    THREAD_WAIT_STATE wait_state: Thread wait state

Return:
    VOID
--*/
static 
VOID 
destroyBlockingPipe(
      int pipe,
      DWORD ThreadId,
      THREAD_WAIT_STATE wait_state)
{
    char PipeFilename[MAX_PATH];

    /* Create the blocking pipe name */
    if(FALSE == THREADGetThreadPipeName(PipeFilename, MAX_PATH, 
                                        GetCurrentProcessId(), ThreadId))
    {
        /* if this fails createBlockingPipe should have failed too, this thread 
           shouldn't even exist */
        ASSERT("couldn't get the threadpipe's name\n");
        return;
    }

    /* if the thread is in early death state and
       close(pipe) will be blocked on outstanding syscall,
       do not close the pipe, otherwise PAL will hang during PAL_Terminate. 
       all outstanding pipes will be closed after the PAL process exits.
     */
#if CLOSE_BLOCKS_ON_OUTSTANDING_SYSCALLS
    if (TWS_EARLYDEATH != wait_state)
    {
#endif
        if (close(pipe) == -1)
        {
            ERROR("Unable to close the thread pipe\n");
            return;
        }
#if CLOSE_BLOCKS_ON_OUTSTANDING_SYSCALLS
    }
#endif

    if (unlink(PipeFilename) == -1)
    {
        ERROR("Unable to unlink the pipe file name errno=%d\n", errno);
    }
}


/*++
Function:
  dupThreadHandle

Abstract:
    Duplicate a thread handle
  
Parameters:
    IN  handle: thread handle to duplicate
    IN  handle_data: structure associated with this handle

Return:
    0: success
    -1: error
--*/
static 
int 
dupThreadHandle( 
      HANDLE handle, 
      HOBJSTRUCT *handle_data)
{
    THREAD *pThread;

    pThread = (THREAD *)handle_data;

    SYNCEnterCriticalSection(&pThread->thread_crit_section, TRUE);

    if (!isValidThreadObject(pThread))
    {
        SYNCLeaveCriticalSection(&pThread->thread_crit_section, TRUE);
        ASSERT("Invalid thread object\n");
        return -1;
    }

    pThread->refCount++;

    TRACE("Duplicating thread handle (srcHandle=%p).\n)", handle);

    SYNCLeaveCriticalSection(&pThread->thread_crit_section, TRUE);
    return 0;
}


/*++
Function:
  closeThreadHandle

Abstract:
    Close a thread handle. Only thread handles that have been duplicated
    could be close, other thread handles could not be closed.
  
Parameters:
    IN  handle: thread handle to close
    IN  handle_data: structure associated with this handle

Return:
    0: success
    -1: error
--*/
static 
int 
closeThreadHandle( 
      HOBJSTRUCT *handle_data)
{
    THREAD *pThread;
    DWORD *pWaitState;
    LPAPC_TRACKER lpAPCTrack=NULL;
    LPAPC_TRACKER tmpAPCTrack=NULL; 
    
    pThread = (THREAD *)handle_data;

    /* note : no need to lock the thread's critical section : the handle 
       manager ensures that we only get here if no other thread is using the 
       handle being closed; this in turn ensures that we'll only reach 
       refcount==0 if no other handle to the same object currently exists */

    if (!isValidThreadObject(pThread))
    {
        ASSERT("Invalid thread object\n");
        return -1;
    }

    pThread->refCount--;

    if (pThread->refCount == 0)
    {
        TRACE("Releasing thread object (handle=%p)\n", pThread->hThread);

        /* close the blocking pipe */
        if (-1 != pThread->blockingPipe)
        {
            pWaitState = SHMPTR_TO_PTR(pThread->waitAwakened);
            destroyBlockingPipe(pThread->blockingPipe, pThread->dwThreadId,
                *pWaitState);
        }

        SHMfree(pThread->waitAwakened);

        /* Release linked list of queued APC functions */
        lpAPCTrack = pThread->lpAPCTracker;
        while (lpAPCTrack)            
        {
            /* Get next entry and free the memory */
            tmpAPCTrack = lpAPCTrack;
            lpAPCTrack = lpAPCTrack->pNext;
            free(tmpAPCTrack);
        }
    
        /* Delete the thread's critical section */
        DeleteCriticalSection(&(pThread->thread_crit_section));

        /* WARNING: From this point on, it is no longer legal for this thread 
           to acquire any critical section */

        pthread_mutex_destroy(&pThread->suspension_mutex);
        pthread_mutex_destroy(&pThread->cond_mutex);
        pthread_cond_destroy(&pThread->suspender_cond);
        pThread->cond_predicate = FALSE;

        /* this assumes that the hostentBuffer was zero filled initially */
        SOCKFreePALHostentFields(&pThread->hostentBuffer);

        /* ensure there are no references to this THREAD object in TLS cache;
           this is necessary to handle the case when the THREAD object dies  
           before the physical THREAD is terminated */
        TLSCacheFlush(pThread);

        /* free the thread object */
        FreeTHREAD(pThread);
    }

    return 0;
}


/*++
Function:
    THREADCreateDummyThreadHandle

    Creates a dummy thread handle, that only supports being CloseHandle()ed

Parameters:
    VOID

Return value:
    HANDLE: no errors
    0:      otherwise
--*/
HANDLE
THREADCreateDummyThreadHandle(void)
{
    THREAD *lpThreadObj;
    HANDLE hThread = INVALID_HANDLE_VALUE;


    /* Create the thread object */
    lpThreadObj = AllocTHREAD();
    if (!lpThreadObj) 
    {
        ERROR("Not enough memory to create dummy thread handle\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto THREADCreateDummyThreadHandleExit;
    }

    /* fill the thread structure. Contains only the bare minimum required 
       to support CloseHandle() */
    lpThreadObj->objHeader.type = HOBJ_THREAD;
    lpThreadObj->objHeader.close_handle = closeDummyThreadHandle; 
    lpThreadObj->objHeader.dup_handle = dupDummyThreadHandle;
    lpThreadObj->refCount = 1;
    lpThreadObj->thread_state = TS_DUMMY;
    lpThreadObj->blockingPipe = -1;
    lpThreadObj->safe_state = TRUE;
    // everything else is zero-filled by AllocTHREAD
    
    hThread = HMGRGetHandle((HOBJSTRUCT *) lpThreadObj);
    if (hThread == INVALID_HANDLE_VALUE)
    {
        ERROR("Unable to create a handle\n");
        FreeTHREAD(lpThreadObj);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto THREADCreateDummyThreadHandleExit;
    }

THREADCreateDummyThreadHandleExit:
    return (hThread);
}


/*++
Function:
  dupDummyThreadHandle

Abstract:
    Duplicate the fake primary thread handle passed back by CreateProcess
  
Parameters:
    IN  handle: thread handle to duplicate
    IN  handle_data: structure associated with this handle

Return:
    0: success
    -1: error
--*/
static 
int 
dupDummyThreadHandle( 
      HANDLE handle, 
      HOBJSTRUCT *handle_data)
{
    THREAD *pThread;

    pThread = (THREAD *)handle_data;

    /* To avoid having a critical section for each thread,
       the refCount field is protected using 
       PROCProcessLock()/PROCProcessUnlock(). This shouldn't
       cause any performance issue. */
    PROCProcessLock();

    if (!isValidThreadObject(pThread))
    {
        ASSERT("Invalid thread object\n");
        PROCProcessUnlock();
        return -1;
    }

    pThread->refCount++;

    TRACE("Duplicating thread handle (srcHandle=%p).\n)", handle);

    PROCProcessUnlock();
    return 0;
}


/*++
Function:
  closeDummyThreadHandle

Abstract:
    Close the fake primary thread handle passed back by CreateProcess
  
Parameters:
    IN  handle: thread handle to close
    IN  handle_data: structure associated with this handle

Return:
    0: success
    -1: error
--*/
static 
int 
closeDummyThreadHandle( 
      HOBJSTRUCT *handle_data)
{
    THREAD *pThread;

    pThread = (THREAD *)handle_data;

    /* To avoid having a critical section for each thread,
       the refCount field is protected using 
       PROCProcessLock()/PROCProcessUnlock(). This shouldn't
       cause any performance issue */
    PROCProcessLock();

    if (!isValidThreadObject(pThread))
    {
        ASSERT("Invalid thread object\n");
        PROCProcessUnlock();
        return -1;
    }

    pThread->refCount--;

    if (pThread->refCount == 0)
    {
        TRACE("Releasing thread object (handle=%p)\n", pThread->hThread);

        /* if dummy thread still has a blocking pipe, it means its process is 
           still blocked in CreateProcess. it won't be possible to wake it up 
           once this handle is closed, so we may as well tell it to die */
        if(-1 != pThread->blockingPipe)
        {
            TRACE("dummy thread still blocked in CreateProcess; sending "
                  "giveupcode\n");
            write(pThread->blockingPipe, &GIVEUPCODE, sizeof(DWORD));
            close(pThread->blockingPipe);
        }

        /* free the thread object */
        FreeTHREAD(pThread);
    }

    PROCProcessUnlock();

    return 0;
}


static BOOL init_suspender(THREAD *pThread)
{
    int pthread_ret;

    pthread_ret = pthread_mutex_init(&pThread->suspension_mutex, NULL);
    if(0 != pthread_ret )
    {
        if(ENOMEM == pthread_ret)
        {
            ERROR("pthread_mutex_init() failed with ENOMEM\n");
        }
        else if( EAGAIN == pthread_ret)
        {
            ERROR("pthread_mutex_init() failed with EAGAIN\n");
        }
        else
        {
            ASSERT("pthread_mutex_init() returned %d (%s)\n", 
                   pthread_ret, strerror(pthread_ret));
        }
        return FALSE;
    }


    pthread_ret = pthread_mutex_init(&pThread->cond_mutex, NULL);

    if(0 != pthread_ret )
    {
        if(ENOMEM == pthread_ret)
        {
            ERROR("pthread_mutex_init() failed with ENOMEM\n");
        }
        else if( EAGAIN == pthread_ret)
        {
            ERROR("pthread_mutex_init() failed with EAGAIN\n");
        }
        else
        {
            ASSERT("pthread_mutex_init() returned %d (%s)\n", 
                   pthread_ret, strerror(pthread_ret));
        }
        pthread_mutex_destroy(&pThread->suspension_mutex);
        return FALSE;
    }
    
    pThread->cond_predicate = FALSE;
    pthread_ret = pthread_cond_init(&pThread->suspender_cond, NULL);
    if(0 != pthread_ret )
    {
        if(ENOMEM == pthread_ret)
        {
            ERROR("pthread_cond_init() failed with ENOMEM\n");
        }
        else if( EAGAIN == pthread_ret)
        {
            ERROR("pthread_cond_init() failed with EAGAIN\n");
        }
        else
        {
            ASSERT("pthread_cond_init() returned %d (%s)\n", 
                   pthread_ret, strerror(pthread_ret));
        }
        pthread_mutex_destroy(&pThread->suspension_mutex);
        pthread_mutex_destroy(&pThread->cond_mutex);
        return FALSE;
    }


    return TRUE;
}

/*++
Function:
  THREADInterlockedAwaken
  
  try to flag a thread's wait state as active, but only if it isn't already 
  active

Parameters :
    DWORD *state : pointer to wait state variable
    BOOL alert_only : if TRUE, only do the exchange if thread is in an alertable 
    state (TWS_ALERTABLE)

Return value :
    TRUE if state was changed, FALSE otherwise
--*/
BOOL THREADInterlockedAwaken(DWORD *pWaitState, BOOL alert_only)
{
    DWORD previous_state;

    previous_state = InterlockedCompareExchange(pWaitState, TWS_ACTIVE, 
                                                TWS_ALERTABLE);
    if(TWS_ALERTABLE != previous_state)
    {
        if(alert_only)
        {
            return FALSE;
        }

        previous_state = InterlockedCompareExchange(pWaitState, TWS_ACTIVE, 
                                                    TWS_WAITING);
        if(TWS_WAITING == previous_state)
        {
            return TRUE;
        }               
    }
    else
    {
        return TRUE;
    }   
    return FALSE;
}

BOOL THREADSuspendThread(THREAD *lpThread)
{
    int failed = 0;
#if HAVE_PTHREAD_SUSPEND || HAVE_PTHREAD_SUSPEND_NP || HAVE_MACH_THREADS || HAVE_SOLARIS_THREADS

    SPINLOCKAcquire(&free_threads_spinlock, 0);
#if HAVE_PTHREAD_SUSPEND_NP
    failed = (pthread_suspend_np((pthread_t)lpThread->dwThreadId) != 0);
#elif HAVE_PTHREAD_SUSPEND
        failed = (pthread_suspend((pthread_t)lpThread->dwThreadId) != 0);
#elif HAVE_MACH_THREADS
    failed = (thread_suspend(pthread_mach_thread_np((pthread_t) lpThread->dwThreadId))
              != KERN_SUCCESS);
#endif
    SPINLOCKRelease(&free_threads_spinlock);
#else
    #error "Don't know how to suspend threads on this platform!"
    return FALSE;
#endif

    if (failed)
    {
        ERROR ("[THREADSuspendThread] native suspend_thread call failed [lwpid=%d thread_state=%d errno=%d]\n",
            lpThread->dwThreadId, lpThread->dwLwpId, lpThread->thread_state, errno);
        return FALSE;
    }


    return TRUE;
}

BOOL THREADContinueThread(THREAD *lpThread)
{
#if HAVE_PTHREAD_CONTINUE || HAVE_PTHREAD_CONTINUE_NP || HAVE_PTHREAD_RESUME || HAVE_PTHREAD_RESUME_NP ||  HAVE_MACH_THREADS || HAVE_SOLARIS_THREADS
#if HAVE_PTHREAD_CONTINUE_NP
    if (pthread_continue_np((pthread_t)lpThread->dwThreadId) != 0)
#elif HAVE_PTHREAD_CONTINUE
    if (pthread_continue((pthread_t)lpThread->dwThreadId) != 0)
#elif HAVE_PTHREAD_RESUME_NP
    if (pthread_resume_np((pthread_t)lpThread->dwThreadId) != 0)
#elif HAVE_PTHREAD_RESUME
    if (pthread_resume((pthread_t)lpThread->dwThreadId) != 0)
#elif HAVE_MACH_THREADS
    if (thread_resume(pthread_mach_thread_np((pthread_t) lpThread->dwThreadId))
            != KERN_SUCCESS)
#endif
    {
        return FALSE;
    }
#else
    #error "Don't know how to resume threads on this platform!"
    return FALSE;
#endif

    return TRUE;
}
