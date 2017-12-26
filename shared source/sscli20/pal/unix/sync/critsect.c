/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    critsect.c

Abstract:

    Implementation of critical section functions.

    Thread ID is used instead of the thread handle for storing the
    owning thread of the critical section. This is much faster to access
    the thread ID than the thread handle.

Note:
   Current implementation uses the pthread_mutex_t as
   the semaphore handle in the CRITICAL_SECTION structure. 
   It works fine on a 32 bits architecture with all existing UNIX flavors.
   It will have to be tested on a 64 bits platform.

   This implementation also supposes no PAL consumer will try to use
   the semaphore HANDLE in the CRITICAL_SECTION as a parameter of any
   semaphore APIs.

Revision History:

--*/
#include "pal/palinternal.h"

#include <pthread.h>
#include <errno.h>
#if HAVE_POLL
#include <poll.h>
#else
#include "pal/fakepoll.h"
#endif  // HAVE_POLL
#if HAVE_STROPTS_H
#include <stropts.h>
#endif  // HAVE_STROPTS_H

/* uncomment this line to enable reporting of critical section contention */
/* #define REPORT_LOCK_CONTENTION */

#ifdef REPORT_LOCK_CONTENTION
#if HAVE_DYLIBS
#include "dlcompat.h"
#else   // HAVE_DYLIBS
#include <dlfcn.h>
#endif  // HAVE_DYLIBS
#endif

#include "pal/critsect.h"
#include "pal/dbgmsg.h"
#include "pal/thread.h"
#include "pal/init.h"


SET_DEFAULT_DEBUG_CHANNEL(CRITSEC);

/*++
Function:
  EnterCriticalSection

See MSDN doc.
--*/
VOID 
PALAPI 
EnterCriticalSection(
         IN OUT LPCRITICAL_SECTION lpCriticalSection)
{
    PERF_ENTRY(EnterCriticalSection);
    ENTRY("EnterCriticalSection(lpCriticalSection=%p)\n", lpCriticalSection);

    SYNCEnterCriticalSectionCore(lpCriticalSection, FALSE,FALSE);

    LOGEXIT("EnterCriticalSection returns void\n");
    PERF_EXIT(EnterCriticalSection);
}

/*++
Function:
  SYNCEnterCriticalSection : Wrapper function for SYNCEnterCriticalSectionCore.

  See MSDN doc.
--*/
VOID SYNCEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection, 
                              BOOL pal_internal)
{
    SYNCEnterCriticalSectionCore(lpCriticalSection, pal_internal, FALSE);
}


/*++
Function:
  SYNCEnterCriticalSectionCore : 'true' implementation of SYNCEnterCriticalSection, 
  distinguishing between PAL and application calls

Notes :
    extra work for "internal" calls is necessary to avoid suspending a thread 
    (with SuspendThread) while it holds a critical section the application 
    doesn't know about (and therefore can't protect itself against)

    SYNCEnterCriticalSectionCore has the functionality of the old 
    SYNCEnterCriticalSection, but can be called directly to prevent the 
    possibility of infinite recursion. One instance of infinite recursion
    was SyncEnterCriticalSection throwing an ASSERT, which would call 
    DBG_printf_gcc, which would call SYNCEnterCriticalSection, which would 
    throw an ASSERT, which would call DBG_printf_gcc, and so on...
    The recursion can broken by calling SYNCEnterCriticalSectionCore
    with noDebug set to TRUE. Note that it is OK to not print debug messages,
    by setting noDebug to true, since the first call to SYNCEnterCriticalSectionCore
    should come from SYNCEnterCriticalSection, which sets noDebug to false.
    Thus, there will already be debug info for the problem, so we can focus on
    breaking the recursion once we have the info.

    Don't call debug macros unconditionally from within this function!
--*/
VOID SYNCEnterCriticalSectionCore(LPCRITICAL_SECTION lpCriticalSection, 
                              BOOL pal_internal, BOOL noDebug)
{
    int Ret;
    THREAD *pCurrentThread;
    DWORD thread_id;

    /* we need a simplified behavior if the PAL is not yet fully operationnal */
    if(PALIsInitialized())
    {
        pCurrentThread = PROCGetCurrentThreadObject();
        thread_id = pCurrentThread->dwThreadId;
    }
    else
    {
        thread_id = THREADSilentGetCurrentThreadId();
        pCurrentThread = NULL;
    }

    /* check if the current thread already owns the criticalSection */
    if (lpCriticalSection->OwningThread == (HANDLE) UintToPtr(thread_id) )
    {
        lpCriticalSection->RecursionCount++;
    }
    else
    {
        if (lpCriticalSection->LockSemaphore == NULL)
        {
            /* When this is called from DBG_printf, don't call debug macros. */
            if(!noDebug){
                ASSERT("Critical section hasn't been initialized properly\n");
            }
            RaiseException(STATUS_INVALID_HANDLE, 0, 0, NULL);
            return;
        }

        if(pal_internal && NULL != pCurrentThread)
        {
            /* remember that the thread is holding a PAL critical section */
#ifdef _DEBUG
            if (pCurrentThread->critsec_count < MAX_TRACKED_CRITSECS)
                pCurrentThread->owned_internal_critsecs[pCurrentThread->critsec_count] = lpCriticalSection;
#endif
            pCurrentThread->critsec_count++;
        }

        /* acquire the mutex */
#ifdef REPORT_LOCK_CONTENTION
        if (pal_internal) 
    {
            Ret = pthread_mutex_trylock((pthread_mutex_t *)
                        lpCriticalSection->LockSemaphore);
            if (Ret)
            {
            /* failed to acquire a PAL-internal critsect, probably due to
                   lock contention.  Report it. */
            InterlockedIncrement((LONG volatile *)
                         &lpCriticalSection->DebugInfo);
            Ret = pthread_mutex_lock((pthread_mutex_t *)
                         lpCriticalSection->LockSemaphore);
        }
    }
        else
    {
        Ret = pthread_mutex_lock((pthread_mutex_t *)
                     lpCriticalSection->LockSemaphore);
    }
#else
        Ret = pthread_mutex_lock((pthread_mutex_t *) 
                                 lpCriticalSection->LockSemaphore);
#endif

        switch (Ret)
        {
        case 0:
            /* mutex was acquired successfuly */
            if (lpCriticalSection->OwningThread != NULL)
            {
                /* When this is called from DBG_printf, don't call debug macros. */
                if(!noDebug){
                    ASSERT("CriticalSection structure is corrupted.\n");   
                }
            }

            if(!pal_internal)
            {
                /* When this is called from DBG_printf, don't call debug macros. */
                if(!noDebug){
                    TRACE("Thread %#x acquires CriticalSection %p\n",
                        pCurrentThread->dwThreadId, lpCriticalSection);
                }
            }
            lpCriticalSection->OwningThread = (HANDLE) UintToPtr(thread_id);
            lpCriticalSection->RecursionCount = 1;
            break;

        case EINVAL:
            /* When this is called from DBG_printf, don't call debug macros. */			
            if(!noDebug){
                ERROR("Critical section hasn't been initialized properly\n");
            }
            RaiseException(STATUS_INVALID_HANDLE, 0, 0, NULL);
            break;

        case EDEADLK:
            /* this situation doesn't warrant an ASSERT, since it can be caused
               by the application rather than the PAL. But returning from the 
               function is a bad idea, since the failure can't be reported to 
               the caller. so let's simulate the deadlock by sleeping forever */
               
            /* When this is called from DBG_printf, don't call debug macros. */              
            if(!noDebug){
                ERROR("pthread_mutex_lock failed with EDEADLK\n");
            }
            poll(NULL,0,INFTIM);
            break;

        default:
            /* When this is called from DBG_printf, don't call debug macros. */			
            if(!noDebug){
                ASSERT("pthread_mutex_lock() failed with unexpected error %d (%s)\n",
                    errno, strerror(errno));
            }
            RaiseException(STATUS_INVALID_HANDLE, 0, 0, NULL);
            break;
        }

    }
}


/*++
Function:
  TryEnterCriticalSection

See MSDN doc.
--*/
BOOL 
PALAPI 
TryEnterCriticalSection(IN OUT LPCRITICAL_SECTION lpCriticalSection)
{
    BOOL bRet = FALSE;
    int Ret;
    DWORD current_thread;

    PERF_ENTRY(TryEnterCriticalSection);
    ENTRY("TryEnterCriticalSection(lpCriticalSection=%p)\n", lpCriticalSection);

    current_thread = THREADSilentGetCurrentThreadId();

    /* check if the current thread already owns the criticalSection */
    if (lpCriticalSection->OwningThread == (HANDLE) UintToPtr(current_thread) )
    {
        lpCriticalSection->RecursionCount++;
        bRet = TRUE;
    }
    else
    {
        if (lpCriticalSection->LockSemaphore == NULL)
        {
            ASSERT("Critical section haven't been initialized properly\n");
            RaiseException(STATUS_INVALID_HANDLE, 0, 0, NULL);
            return FALSE;
        }

        /* acquire the mutex */
        Ret = pthread_mutex_trylock((pthread_mutex_t *) 
                                 lpCriticalSection->LockSemaphore);

        switch (Ret)
        {
        case 0:
            /* mutex was acquired successfuly */
            if (lpCriticalSection->OwningThread != NULL)
            {
                ASSERT("CriticalSection structure is corrupted.\n");   
            }

            TRACE("Thread: %#x acquires CriticalSection %p\n",
                  current_thread, lpCriticalSection);
            lpCriticalSection->OwningThread = (HANDLE) UintToPtr(current_thread);
            lpCriticalSection->RecursionCount = 1;
            bRet = TRUE;
            break;

        case EBUSY:
            TRACE("CriticalSection is owned by another thread\n");
            break;

        case EINVAL:
            ASSERT("Critical section haven't been initialized properly\n");
            RaiseException(STATUS_INVALID_HANDLE, 0, 0, NULL);
            break;

        default:
            ASSERT("pthread_mutex_trylock() failed with unexpected error %d "
                   "(%s)\n", errno, strerror(errno));
            RaiseException(STATUS_INVALID_HANDLE, 0, 0, NULL);
            break;
        }

    }
    
    LOGEXIT("TryEnterCriticalSection returns BOOL %d\n", bRet);
    PERF_EXIT(TryEnterCriticalSection);

    return bRet;
}


/*++
Function:
  LeaveCriticalSection

See MSDN doc.
--*/
VOID 
PALAPI 
LeaveCriticalSection(
         IN OUT LPCRITICAL_SECTION lpCriticalSection)
{
    PERF_ENTRY(LeaveCriticalSection);
    ENTRY("LeaveCriticalSection(lpCriticalSection=%p)\n", lpCriticalSection);
    
    SYNCLeaveCriticalSectionCore(lpCriticalSection,FALSE,FALSE);

    LOGEXIT("LeaveCriticalSection returns void\n");
    PERF_EXIT(LeaveCriticalSection);
}


/*++
Function:
  SYNCLeaveCriticalSection : Wrapper function for SYNCLeaveCriticalSectionCore.

  See MSDN doc.
--*/
VOID SYNCLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection, 
                              BOOL pal_internal)
{
    SYNCLeaveCriticalSectionCore(lpCriticalSection, pal_internal, FALSE);
}


/*++
Function:
  SYNCLeaveCriticalSectionCore : 'true' implementation of SYNCLeaveCriticalSection, 
  distinguishing between PAL and application calls

Notes :
    extra work for "internal" calls is necessary to avoid suspending a thread 
    (with SuspendThread) while it holds a critical section the application 
    doesn't know about (and therefore can't protect itself against)
    
    SYNCLeaveCriticalSectionCore has the functionality of the old 
    SYNCLeaveCriticalSection, but can be called directly to prevent the 
    possibility of infinite recursion. One instance of infinite recursion
    was SYNCLeaveCriticalSection throwing an ASSERT, which would call 
    DBG_printf_gcc, which would call SYNCLeaveCriticalSection, which would 
    throw an ASSERT, which would call DBG_printf_gcc, and so on...
    The recursion can broken by calling SYNCLeaveCriticalSectionCore
    with noDebug set to TRUE. Note that it is OK to not print debug messages,
    by setting noDebug to true, since the first call to SYNCEnterCriticalSectionCore
    should come from SYNCEnterCriticalSection, which sets noDebug to false.
    Thus, there will already be debug info for the problem, so we can focus on
    breaking the recursion once we have the info.

    Don't call debug macros unconditionally from within this function!
--*/
VOID SYNCLeaveCriticalSectionCore(LPCRITICAL_SECTION lpCriticalSection, 
                              BOOL pal_internal, BOOL noDebug)
{
    int Ret;
    THREAD *pCurrentThread;
    DWORD thread_id;

    /* need simplified behavior if PAL isn't fully initialized */
    if(PALIsInitialized())
    {
        pCurrentThread = PROCGetCurrentThreadObject();
        thread_id = pCurrentThread->dwThreadId;
    }
    else
    {
        thread_id = THREADSilentGetCurrentThreadId();
        pCurrentThread = NULL;
    }

    /* first check the current thread is owning the CriticalSection */
    if (lpCriticalSection->OwningThread == (HANDLE) UintToPtr(thread_id))
    {
        lpCriticalSection->RecursionCount--;
        if (lpCriticalSection->RecursionCount == 0)
        {
            lpCriticalSection->OwningThread = NULL;

            if(!pal_internal && pCurrentThread)
            {
                /* When this is called from DBG_printf, don't call debug macros. */
                if(!noDebug){
                    TRACE("Thread %#x leaves CriticalSection %p\n",
                        pCurrentThread->dwThreadId, lpCriticalSection);
                }
            }
            Ret = pthread_mutex_unlock((pthread_mutex_t *)
                                 lpCriticalSection->LockSemaphore);

            if (Ret != 0)
            {
                /* When this is called from DBG_printf, don't call debug macros. */
                if(!noDebug){
                    ASSERT("pthread_mutex_lock() returned %d (%s)\n", 
                        Ret, strerror(Ret));
                }
            }
            if(pal_internal && NULL != pCurrentThread)
            {
                pCurrentThread->critsec_count--;

                if(pCurrentThread->suspend_intent && 
                   0 == pCurrentThread->critsec_count)
                {
                    /* someone wants to suspend us, and we just left the last 
                       critical section we were holding */

                    /* wait until the suspender thread is calling 
                       pthread_cond_wait */
                    pthread_mutex_lock(&pCurrentThread->cond_mutex);

                    /* set the condition predicate */
                    pCurrentThread->cond_predicate = TRUE;

                    /* let suspender thread suspend us */
                    pthread_cond_signal(&pCurrentThread->suspender_cond);

                    pthread_mutex_unlock(&pCurrentThread->cond_mutex);

                    /* wait for the suspender thread to unlock this AND for the 
                       resumer thread to resume us */
                    pthread_mutex_lock(&pCurrentThread->suspension_mutex);

                    /* release the mutex, we don't actually need to keep it.
                       note : we only get here after we get resumed */
                    pthread_mutex_unlock(&pCurrentThread->suspension_mutex);
                }
            }
        }
    }
    else
    {     
        /* When this is called from DBG_printf, don't call debug macros. */
        if(!noDebug){
            ERROR("CurrentThread is not owning the CriticalSection.\n");
        }
    }
}


/*++
Function:
  InitializeCriticalSection

See MSDN doc.
--*/
VOID 
PALAPI 
InitializeCriticalSection(
         OUT LPCRITICAL_SECTION lpCriticalSection)
{
    DWORD dwExceptionCode;
    PERF_ENTRY(InitializeCriticalSection);
    ENTRY("InitializeCriticalSection(lpCriticalSection=%p)\n", lpCriticalSection);

    dwExceptionCode = SYNCInitializeCriticalSection(lpCriticalSection);

    if (dwExceptionCode)
    {
        RaiseException(dwExceptionCode, 0, 0, NULL);
    }

    LOGEXIT("InitializeCriticalSection returns void\n");
    PERF_EXIT(InitializeCriticalSection);
}


/*++
Function :
  SYNCInitializeCriticalSection

Initialize the critical section without throwing exceptions upon error. 
To be used internally instead of InitializeCriticalSection.

Return value : 
    0                 if succesful or
    STATUS_NO_MEMORY  (exception code) in low memory conditions.

--*/
DWORD 
SYNCInitializeCriticalSection(
         OUT LPCRITICAL_SECTION lpCriticalSection)
{
    pthread_mutex_t *pMutex;
    DWORD dwRet = STATUS_NO_MEMORY;
    
    TRACE("SYNCInitializeCriticalSection(lpCriticalSection=%p)\n", lpCriticalSection);

    lpCriticalSection->DebugInfo = NULL;
    lpCriticalSection->LockCount = 0;
    lpCriticalSection->RecursionCount = 0;
    lpCriticalSection->SpinCount = 0;
    lpCriticalSection->OwningThread = NULL;

    /* use a pthread mutex instead of a semaphore */
    pMutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

    if (pMutex == NULL)
    {
        ERROR("Unable to create the Mutex.\n");
        goto done;
    }
    else
    {
        if (pthread_mutex_init(pMutex, NULL))
        {
            ERROR("Unable to create the Mutex.\n");
            free(pMutex);
            goto done;
        }
        else
        {      
            lpCriticalSection->LockSemaphore = (HANDLE) pMutex;
            dwRet = 0;
        }
    }

done:
    TRACE("SYNCInitializeCriticalSection returns %d\n", dwRet);
    return dwRet;
}


/*++
Function:
  DeleteCriticalSection

See MSDN doc.
--*/
VOID 
PALAPI 
DeleteCriticalSection(
         IN OUT LPCRITICAL_SECTION lpCriticalSection)
{
    PERF_ENTRY(DeleteCriticalSection);
    ENTRY("DeleteCriticalSection(lpCriticalSection=%p)\n", lpCriticalSection);
    pthread_mutex_destroy((pthread_mutex_t *) lpCriticalSection->LockSemaphore);

    free((void *) lpCriticalSection->LockSemaphore);

    /* check if the critical section is owned by a thread */
    if ((lpCriticalSection->OwningThread != NULL) ||
        (lpCriticalSection->RecursionCount != 0))
    {
        ERROR("CriticalSection still owned (%p)\n", lpCriticalSection);
    }
 
    lpCriticalSection->OwningThread = NULL;    
    lpCriticalSection->RecursionCount = 0;    
    lpCriticalSection->LockSemaphore = NULL;

#ifdef REPORT_LOCK_CONTENTION
    if (*(LONG*)&lpCriticalSection->DebugInfo > 1)
    {
        Dl_info info;
        int dlret;

        dlret = dladdr(lpCriticalSection, &info);
        if (dlret && info.dli_sname && *info.dli_sname)
        {
        fprintf(stderr,
                    "PAL critsect %s!%s (%p) had a high contention "
                    "count of %d\n",
            info.dli_fname, info.dli_sname, lpCriticalSection,
            *(LONG *)&lpCriticalSection->DebugInfo);
        }
        else
        {
            fprintf(stderr, 
                    "PAL critsect %p had a high contention count of %d\n",
                    lpCriticalSection, *(LONG *)&lpCriticalSection->DebugInfo);
        }
    }
#endif

    LOGEXIT("DeleteCriticalSection returns void\n");
    PERF_EXIT(DeleteCriticalSection);
}
