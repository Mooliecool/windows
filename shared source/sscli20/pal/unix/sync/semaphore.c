/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    semaphore.c

Abstract:

    Implementation of semaphore synchroniztion object as described in 
    the WIN32 API

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/critsect.h"
#include "pal/dbgmsg.h"
#include "pal/handle.h"
#include "pal/thread.h"
#include "semaphore.h"


/* ------------------- Definitions ------------------------------*/
SET_DEFAULT_DEBUG_CHANNEL(SYNC);


/* ------------------- Static function prototypes ---------------------------*/
static int DupSemaphoreHandle( HANDLE handle, HOBJSTRUCT *handle_data);
static int CloseSemaphoreHandle( HOBJSTRUCT *handle_data);
static BOOL IsValidSemaphoreObject(Semaphore *pSem);


/*++
Function:
  CreateSemaphoreA

Note:
  Semaphore are used only inside the same process.
  Security attributes could be ignored.
  lpName is always NULL.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateSemaphoreA(
		 IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
		 IN LONG lInitialCount,
		 IN LONG lMaximumCount,
		 IN LPCSTR lpName)
{
    HANDLE h;

    PERF_ENTRY(CreateSemaphoreA);
    ENTRY("CreateSemaphoreA(lpSemaphoreAttributes=%p, lInitialCount=%d, "
          "lMaximumCount=%d, lpName=%p (%s))\n",
          lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName?lpName:"NULL", lpName?lpName:"NULL");

    /* lpName should be set to NULL */
    if (lpName != NULL)
    {
        ASSERT("lpName must be NULL\n");
        SetLastError(ERROR_INVALID_HANDLE);
        LOGEXIT("CreateSemaphoreA returns HANDLE NULL\n");
        PERF_EXIT(CreateSemaphoreA);
        return NULL;
    }

    h = CreateSemaphoreW(lpSemaphoreAttributes, lInitialCount, 
			lMaximumCount, NULL);

    LOGEXIT("CreateSemaphoreA returns HANDLE %p\n", h);
    PERF_EXIT(CreateSemaphoreA);

    return h;
}


/*++
Function:
  CreateSemaphoreW

Note:
  Semaphore are used only inside the same process.
  Security attributes could be ignored.
  lpName is always NULL.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateSemaphoreW(
		 IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
		 IN LONG lInitialCount,
		 IN LONG lMaximumCount,
		 IN LPCWSTR lpName)
{
    HANDLE hSemaphore = NULL;
    DWORD LastError = 0;
    Semaphore *pSemaphore;
    
    PERF_ENTRY(CreateSemaphoreW);
    ENTRY("CreateSemaphoreW(lpSemaphoreAttributes=%p, lInitialCount=%d, "
          "lMaximumCount=%d, lpName=%p (%S))\n",
          lpSemaphoreAttributes, lInitialCount, lMaximumCount, 
          lpName?lpName:W16_NULLSTRING, 
          lpName?lpName:W16_NULLSTRING);

    /* Validate parameters */

    if (lpName != NULL)
    {
        ASSERT("lpName must be NULL\n");
        LastError = ERROR_INVALID_PARAMETER;
        goto CreateSemaphoreExit;
    }

    if (lpSemaphoreAttributes != NULL)
    {
        ASSERT("lpSemaphoreAttributes must be NULL\n");
        LastError = ERROR_INVALID_PARAMETER;
        goto CreateSemaphoreExit;
    }

    if (lMaximumCount <= 0)
    {
        ERROR("lMaximumCount is invalid (%d)\n", lMaximumCount);
        LastError = ERROR_INVALID_PARAMETER;
        goto CreateSemaphoreExit;
    }

    if ((lInitialCount < 0) || (lInitialCount > lMaximumCount))
    {
        ERROR("lInitialCount is invalid (%d)\n", lInitialCount);
        LastError = ERROR_INVALID_PARAMETER;
        goto CreateSemaphoreExit;
    }

    pSemaphore = (Semaphore *) HeapAlloc(GetProcessHeap(), 
                                         HEAP_ZERO_MEMORY, 
                                         sizeof(Semaphore));

    if (pSemaphore == NULL)
    {
        ERROR("Not enough memory to allocate a semaphore\n");
        LastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateSemaphoreExit;
    }

    pSemaphore->objHeader.type = HOBJ_SEMAPHORE;
    pSemaphore->objHeader.close_handle = CloseSemaphoreHandle;
    pSemaphore->objHeader.dup_handle = DupSemaphoreHandle;
    pSemaphore->refCount = 1;
    pSemaphore->semCount = lInitialCount;
    pSemaphore->maximumCount = lMaximumCount;
    pSemaphore->waitingThreads = NULL;

    if (0 != SYNCInitializeCriticalSection(&pSemaphore->critSection))
    {
        HeapFree(GetProcessHeap(), 0, pSemaphore);
        ERROR("Unable initialize critical section\n");
        LastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateSemaphoreExit;
    };

    hSemaphore = HMGRGetHandle((HOBJSTRUCT *) pSemaphore);
    if (hSemaphore == INVALID_HANDLE_VALUE)
    {
        DeleteCriticalSection(&pSemaphore->critSection);
        HeapFree(GetProcessHeap(), 0, pSemaphore);
        ERROR("Unable to get a free handle\n");
        LastError = ERROR_NOT_ENOUGH_MEMORY;
        goto CreateSemaphoreExit;
    }

CreateSemaphoreExit:
    if (LastError)
        SetLastError(LastError);

    LOGEXIT("CreateSemaphoreW returns HANDLE %p\n", hSemaphore);
    PERF_EXIT(CreateSemaphoreW);
    return (HANDLE) hSemaphore;
}


/*++
Function:
  ReleaseSemaphore

See MSDN doc.
--*/
BOOL
PALAPI
ReleaseSemaphore(
		 IN HANDLE hSemaphore,
		 IN LONG lReleaseCount,
		 OUT LPLONG lpPreviousCount)
{
    Semaphore *pSemaphore = NULL;
    DWORD dwLastError = 0;
    BOOL bRet = TRUE;
    ThreadWaitingList *pWaitingThread;
    ThreadWaitingList *pPrevThread;
    ThreadWaitingList *pTempThread;

    PERF_ENTRY(ReleaseSemaphore);
    ENTRY("ReleaseSemaphore(hSemaphore=%p, lReleaseCount=%d, "
          "lpPreviousCount=%p)\n",
          hSemaphore, lReleaseCount, lpPreviousCount);

    if (lReleaseCount <= 0)
    {
        dwLastError = ERROR_INVALID_PARAMETER;
		bRet = FALSE;
		goto ReleaseSemaphoreExit;
    }
    pSemaphore = (Semaphore *) HMGRLockHandle2(hSemaphore, HOBJ_SEMAPHORE);

	if ( pSemaphore == NULL)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		bRet = FALSE;
		goto ReleaseSemaphoreExit;
	}
	
    SYNCEnterCriticalSection(&pSemaphore->critSection, TRUE);

	if (pSemaphore->semCount + lReleaseCount > pSemaphore->maximumCount)
	{
		SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);
		dwLastError = ERROR_INVALID_PARAMETER;
		bRet = FALSE;
		goto ReleaseSemaphoreExit;
	}

    if(NULL != lpPreviousCount)
    {
	    *lpPreviousCount = pSemaphore->semCount;
    }
	pSemaphore->semCount += lReleaseCount;

    pPrevThread = NULL;
    pWaitingThread = pSemaphore->waitingThreads;
    while(NULL !=pWaitingThread && 0 != pSemaphore->semCount )
    {
        if(!THREADInterlockedAwaken(pWaitingThread->state.pAwakened, FALSE))
        {
            TRACE("thread is already awake, skipping it\n");
            pPrevThread = pWaitingThread;
            pWaitingThread = pWaitingThread->ptr.Next;
            continue;
        }

        /* waking up thread : unchain it, wake it up, decrease semaphore count*/

        if(NULL == pPrevThread)
        {
            pSemaphore->waitingThreads = pWaitingThread->ptr.Next;
        }
        else
        {
            pPrevThread->ptr.Next = pWaitingThread->ptr.Next;
        }
        
        TRACE("Waking up thread(%#x) Sem. has been released(%p)\n",
              pWaitingThread->threadId, hSemaphore);

        WakeUpThread(pWaitingThread->threadId, 
                     pWaitingThread->processId,
                     pWaitingThread->blockingPipe,
                     WUTC_SIGNALED );
        pTempThread = pWaitingThread;
        pWaitingThread = pWaitingThread->ptr.Next;
        free(pTempThread);
            
        pSemaphore->semCount--;
    }

	SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);

ReleaseSemaphoreExit:
    if(NULL!=pSemaphore)
    {
        HMGRUnlockHandle(hSemaphore,&pSemaphore->objHeader);
    }
	if (dwLastError)
	{
		SetLastError(dwLastError);
	}

	LOGEXIT ("ReleaseSemaphore returns BOOL %d\n", bRet);
      PERF_EXIT(ReleaseSemaphore);
    return bRet;
}


/*++
Function:
  SemaphoreWaitOn

  Check if the semaphore is signaled. If not signaled place
  the current thread to the list of waiting thread. This function is
  called by WaitXXXX functions when it's time to wait on a Semaphore.
  The caller is responsible for blocking the thread, this function does
  not wait on the semaphore.

Parameters
    IN hSemaphore   semaphore checked to see if it is signaled
    SHMPTR wait_state : shared memory pointer to waiting thread's wait state

returns
    WAITON_CODE value (see thread.h)
--*/
int
SemaphoreWaitOn(
    IN HANDLE hSemaphore, SHMPTR wait_state)
{
    Semaphore *pSemaphore;
	BOOL ret;
	ThreadWaitingList *pWaitingThread;

    pSemaphore = (Semaphore *) HMGRLockHandle2(hSemaphore, HOBJ_SEMAPHORE);

	if ( pSemaphore == NULL )
	{
        SetLastError(ERROR_INVALID_HANDLE);
		ASSERT("Invalid semaphore handle %p\n", hSemaphore);
        ret = WOC_ERROR;
        goto done;
	}
    
    SYNCEnterCriticalSection(&pSemaphore->critSection, TRUE);
    
    if (pSemaphore->semCount)
    {
        DWORD *pAwakenState;

        /* try to flag the thread as awakened. this can fail if the thread has 
           already been awakened by another object */
        pAwakenState = SHMPTR_TO_PTR(wait_state);

        if(!THREADInterlockedAwaken(pAwakenState, FALSE))
        {
            ret = WOC_INTERUPTED;
            TRACE("thread is already awake; not waiting on it\n");
        }
        else
        {
            ret = WOC_SIGNALED;
            pSemaphore->semCount--;
            TRACE("Semaphore(%p) is signaled, no wait\n", hSemaphore);
        }
	}
	else
	{
		/* add the current thread to the list of waiting threads */
		pWaitingThread = (ThreadWaitingList *) 
                                malloc(sizeof(ThreadWaitingList));

		if (pWaitingThread == NULL)
		{
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            ERROR("Not enough memory to allocate a ThreadQueue\n");
			ret = WOC_ERROR;
		}
		else
		{
            pWaitingThread->threadId = GetCurrentThreadId();
            pWaitingThread->processId = GetCurrentProcessId();
            pWaitingThread->blockingPipe = THREADGetPipe();
            pWaitingThread->ptr.Next = NULL;
            pWaitingThread->state.pAwakened = SHMPTR_TO_PTR(wait_state);

            TRACE("ThreadId=%#x will wait on hSemaphore=%p\n", 
                  pWaitingThread->threadId, hSemaphore);

			/* place the thread at the end of the list */
			if (pSemaphore->waitingThreads == NULL)
			{
				pSemaphore->waitingThreads = pWaitingThread;
			}
			else
			{
                ThreadWaitingList *pIter;

                pIter = pSemaphore->waitingThreads;

                while (pIter->ptr.Next)
                {
                    pIter = pIter->ptr.Next;
                }

                pIter->ptr.Next = pWaitingThread;
			}

			ret = WOC_WAITING;
            
            TRACE("Semaphore(%p) is not signaled,"
                  " current thread added to the waiting list\n",
                  hSemaphore);
		}
	}
    SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);

done:
    if(NULL!=pSemaphore)
    {
        HMGRUnlockHandle(hSemaphore,&pSemaphore->objHeader);
    }

	return ret;
}

/*++
Function:
  SemaphoreRemoveWaitingThread

  Remove the CurrentThreadId from the list of waiting thread. This
  function is called when the current thread stop waiting on a Semaphore
  for a different reason than the Semaphore was signaled. (e.g. a timeout,
  or the thread was waiting on multiple objects and another object was
  signaled)

Parameters
    IN hSemaphore   semaphore which the waiting thread list is updated

returns
    -1: an error occurred
    0: if the thread wasn't found in the list.
    1: if the thread was removed from the waiting list.
--*/
int
SemaphoreRemoveWaitingThread(
	IN HANDLE hSemaphore)
{
    Semaphore *pSemaphore;
	int ret = 0;
	ThreadWaitingList *pWaitingThread;
	ThreadWaitingList *pNextWaitingThread;
    DWORD CurrentThreadId;

    pSemaphore = (Semaphore *) HMGRLockHandle2(hSemaphore, HOBJ_SEMAPHORE);

	if ( pSemaphore == NULL)
	{
		ASSERT("Invalid semaphore handle %p\n", hSemaphore);
		return -1;
	}

	SYNCEnterCriticalSection(&pSemaphore->critSection, TRUE);

    CurrentThreadId = GetCurrentThreadId();
    pWaitingThread = pSemaphore->waitingThreads;

    if (pWaitingThread == NULL)
    {        
        ret = 0;  /* list is empty */
        goto RemoveThreadExit;
    }

    /* check if it is the first element in the list */
    if (pWaitingThread->threadId == CurrentThreadId)
    {
        pSemaphore->waitingThreads = pWaitingThread->ptr.Next;
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
                ThreadWaitingList *pTemp;
                pTemp = pWaitingThread->ptr.Next;
                pWaitingThread->ptr.Next = pNextWaitingThread->ptr.Next;
                free (pTemp);
                ret = 1;
                break;
            }

            pWaitingThread = pNextWaitingThread; 
        }
    }

RemoveThreadExit:
	SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);
    HMGRUnlockHandle(hSemaphore,&pSemaphore->objHeader);

    if (ret == 0)
    {
        TRACE("ThreadId=%#x was not waiting on hSemaphore=%p\n",
             CurrentThreadId, hSemaphore);
    }
    else
    {
        TRACE("ThreadId=%#x is no more waiting on hSemaphore=%p\n",
              CurrentThreadId, hSemaphore);
    }

    return ret;
}


/*++
Function:
  CloseSemaphoreHandle

Parameters
    IN handle_data   Semaphore object being closed

Returns:
    0 if success
    -1 otherwise

--*/
static int 
CloseSemaphoreHandle( 
	HOBJSTRUCT *handle_data)
{
	Semaphore *pSemaphore = (Semaphore *) handle_data;

    if (!IsValidSemaphoreObject(pSemaphore))
    {
        ASSERT("Not a valid semaphore (%p)\n", pSemaphore);
        return -1;
    }

	SYNCEnterCriticalSection(&pSemaphore->critSection, TRUE);

    pSemaphore->refCount--;

    if (pSemaphore->refCount > 0)
    {
        /* there's still other handles referencing this object */
        SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);

        TRACE("Semaphore handle closed, object not deleted [refCount > 0]\n");

        return 0;
    }

	pSemaphore->objHeader.type = HOBJ_INVALID;

	SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);
	DeleteCriticalSection(&pSemaphore->critSection);

	HeapFree(GetProcessHeap(), 0, pSemaphore);

    TRACE("Semaphore object closed, object %p destroyed\n", pSemaphore);

	return 0;
}


/*++
Function:
  DuplicateSemaphoreHandle

Parameters
    IN handle   Semaphore handle being closed
    IN handle_data   Semaphore object being closed

Returns:
    0 if success
    -1 otherwise

--*/
static 
int 
DupSemaphoreHandle( 
	HANDLE handle, 
	HOBJSTRUCT *handle_data)
{
    Semaphore *pSemaphore = (Semaphore *) handle_data;

    if (!IsValidSemaphoreObject(pSemaphore))
    {
        ASSERT("Not a valid semaphore handle(%p)\n", handle);
        return -1;
    }

	SYNCEnterCriticalSection(&pSemaphore->critSection, TRUE);

    pSemaphore->refCount++;

	SYNCLeaveCriticalSection(&pSemaphore->critSection, TRUE);

    return 0;
}


/*++
Function:
    isValidSemaphoreObject

Abstract:
    Verify if the object is a semaphore by checking the magic field
  
Parameters:
    IN  pSemaphore: Semaphore object

Return:
    TRUE: if valid
    FALSE: otherwise
--*/
static 
BOOL
IsValidSemaphoreObject(
      Semaphore *pSemaphore)
{
    return (pSemaphore->objHeader.type = HOBJ_SEMAPHORE);
}











