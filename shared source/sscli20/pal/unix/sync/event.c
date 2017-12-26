/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    event.c

Abstract:

    Implementation of event synchroniztion object as described in 
    the WIN32 API

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/handle.h"
#include "pal/thread.h"
#include "pal/shmemory.h"
#include "event.h"
#include "pal/init.h"


/* ------------------- Definitions ------------------------------*/
SET_DEFAULT_DEBUG_CHANNEL(SYNC);

const int MAX_EVENT = MAX_PATH + 1;

/* ------------------- Static function prototypes ---------------------------*/
static int DupEventHandle( HANDLE handle, HOBJSTRUCT *handle_data);
static int CloseEventHandle( HOBJSTRUCT *handle_data);
static BOOL IsValidEventObject(Event *pEvent);

/*++
Function:
  CreateEventA

Note:
  Events could be used for cross-process synchroniztion.
  Security Attributes could be ignored.
  lpName could be non NULL.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateEventA(
         IN LPSECURITY_ATTRIBUTES lpEventAttributes,
         IN BOOL bManualReset,
         IN BOOL bInitialState,
         IN LPCSTR lpName)
{
    HANDLE hEvent;
    WCHAR pwName[MAX_EVENT];

    PERF_ENTRY(CreateEventA);
    ENTRY("CreateEventA(lpEventAttr=%p, bManualReset=%d, bInitialState=%d, lpName=%p (%s)\n",
          lpEventAttributes, bManualReset, bInitialState, lpName?lpName:"NULL", lpName?lpName:"NULL");
    
    if (lpName != NULL)
    {
        if (MultiByteToWideChar(CP_ACP, 0, lpName, -1, pwName, MAX_EVENT) == 0)
        {
            if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
            {
                ERROR("lpName is larger than MAX_EVENT (%d)!\n", MAX_EVENT);
                SetLastError(ERROR_FILENAME_EXCED_RANGE);
            }
            else
            {
                ERROR("MultiByteToWideChar failure! (error=%d)\n",
                      GetLastError());
                SetLastError(ERROR_INVALID_PARAMETER);
            }            
            LOGEXIT("CreateEventA returns HANDLE NULL\n");
            PERF_EXIT(CreateEventA);
            return NULL;
        }

        hEvent = CreateEventW(lpEventAttributes, bManualReset, bInitialState, pwName);
    }
    else
    {
        hEvent = CreateEventW(lpEventAttributes, bManualReset, bInitialState, NULL);
    }
    
    LOGEXIT("CreateEventA returns HANDLE %p\n", hEvent);
    PERF_EXIT(CreateEventA);
    return hEvent;
}


/*++
Function:
  CreateEventW

Note:
  Events could be used for cross-process synchroniztion.
  Security Attributes could be ignored.
  lpName could be non NULL.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateEventW(
         IN LPSECURITY_ATTRIBUTES lpEventAttributes,
         IN BOOL bManualReset,
         IN BOOL bInitialState,
         IN LPCWSTR lpName)
{
    HANDLE hEvent = NULL;
    Event *pEvent;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    SHMPTR newEvent;
    DWORD OldLastError;
    BOOL need_unlock = FALSE;
    LPCWSTR lpObjectName = NULL;
    WCHAR MangledObjectName[MAX_PATH];

    PERF_ENTRY(CreateEventW);
    ENTRY("CreateEventW(lpEventAttr=%p, bManualReset=%d, "
          "bInitialState=%d, lpName=%p (%S)\n", lpEventAttributes, bManualReset, 
           bInitialState, lpName?lpName:W16_NULLSTRING, lpName?lpName:W16_NULLSTRING);

    /* Validate parameters */

    if (lpEventAttributes != NULL)
    {
        ASSERT("lpEventAttributes must be NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto CreateEventExit;
    }

    if (lpName)
    {
        lpObjectName=lpName;
        if (!MangleObjectNameW(&lpObjectName, MangledObjectName)) 
        {
            goto CreateEventExit;
        } 
        
        /* take the shared memory lock to prevent other threads/processes from 
           creating/deleting the object behind our back*/
        SHMLock();
        need_unlock = TRUE;
        
        /* Verify if there's an Event already created with that name */
        OldLastError = GetLastError();

	/* Try to open the event, using its unmangled name, as OpenEventW
           will mangle it */
        hEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, lpName);

        if (hEvent != NULL)
        {
            TRACE("Event (%S) already exist.\n", lpName);
            SetLastError(ERROR_ALREADY_EXISTS);
            goto CreateEventExit;
        }

        if (GetLastError() == ERROR_INVALID_HANDLE)
        {
            ERROR("Another kernel object is using that name\n");
            SetLastError(ERROR_INVALID_HANDLE);
            goto CreateEventExit;
        }

        SetLastError(OldLastError);
    }

    /* Allocate the event object */
    pEvent = (Event *) malloc(sizeof(Event));

    if (pEvent == NULL)
    {
        ERROR("Unable to allocate memory\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto CreateEventExit;
    }

    /* Allocate the EventInfo in shared memory, so it can be accessible by
       other processes */

    newEvent = SHMalloc(sizeof(GLOBAL_EVENT_SYSTEM_OBJECT));

    if (newEvent == (SHMPTR) NULL)
    {
        ERROR("Unable to allocate shared memory\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        free( pEvent );
        goto CreateEventExit;
    }

    pEvent->objHeader.type = HOBJ_EVENT;
    pEvent->objHeader.close_handle = CloseEventHandle;
    pEvent->objHeader.dup_handle = DupEventHandle;

    pEvent->refCount = 1;
    pEvent->info = newEvent;

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(newEvent);
    pEventInfo->refCount = 1;
    pEventInfo->state = bInitialState;
    pEventInfo->manualReset = bManualReset;
    pEventInfo->waitingThreads = (SHMPTR) NULL;
    pEventInfo->next = (SHMPTR) NULL;
    
    pEventInfo->ShmHeader.ObjectType = SHM_NAMED_EVENTS;
    pEventInfo->ShmHeader.ShmSelf = newEvent;
    if (lpName)
    {
        if ((pEventInfo->ShmHeader.ShmObjectName = SHMWStrDup( lpObjectName )) == 0)
        {
            ERROR( "Unable to allocate shared memory!\n " );
            SetLastError(ERROR_INTERNAL_ERROR);
            SHMfree(newEvent);
            free(pEvent);
            goto CreateEventExit;
        }
    }
    else
    {
        pEventInfo->ShmHeader.ShmObjectName = 0;
    }

    hEvent = HMGRGetHandle((HOBJSTRUCT *) pEvent);

    if (hEvent == INVALID_HANDLE_VALUE)
    {
        if (lpName != NULL)
        {
            SHMfree(pEventInfo->ShmHeader.ShmObjectName);
        }
        SHMfree(newEvent);
        free(pEvent);
        ERROR("Unable to get a free handle\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto CreateEventExit;
    }

    /* Save the EventInfo into the global link list in shared memory. 
       (Only if it has a name) */
    if (lpName)
    {
        SHMAddNamedObject(newEvent);
    }

    TRACE("Event created (HANDLE=%p)\n", hEvent);

CreateEventExit:
    if(need_unlock)
    {
        /* release the lock taken before OpenEventW */
        SHMRelease();
    }
    LOGEXIT("CreateEventW returns HANDLE %p\n", hEvent);
    PERF_EXIT(CreateEventW);
    return hEvent;
}


/*++
Function:
  SetEvent

See MSDN doc.
--*/
BOOL
PALAPI
SetEvent(
     IN HANDLE hEvent)
{
    BOOL bRet = TRUE;
    Event *pEvent;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    SHMPTR pTemp;
    ThreadWaitingList *pWaitingThread;

    PERF_ENTRY(SetEvent);
    ENTRY("SetEvent(hEvent=%p)\n", hEvent);

    pEvent = (Event *) HMGRLockHandle2(hEvent, HOBJ_EVENT);
    if(NULL == pEvent)
    {
        ERROR("Unable to lock handle %p!\n", hEvent);
        SetLastError(ERROR_INVALID_HANDLE);
        LOGEXIT("SetEvent returns BOOL %d\n",FALSE);
        PERF_EXIT(SetEvent);
        return FALSE;
    }                                           

    SHMLock();

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(pEvent->info);

    if (pEventInfo == NULL)
    {
        ASSERT("Invalid shared memory pointer\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        bRet = FALSE;
        goto SetEventExit;
    }

    if (!pEventInfo->state)
    {
        DWORD *pAwakenState;
        SHMPTR shmWaitingThread;
        ThreadWaitingList *prevThread = NULL;

        pEventInfo->state = TRUE;

        shmWaitingThread = pEventInfo->waitingThreads;

        /* wake up waiting threads */
        while (shmWaitingThread)
        {
            pWaitingThread = SHMPTR_TO_PTR(shmWaitingThread);

            if (pWaitingThread == NULL)
            {
                ASSERT("Invalid shared memory pointer\n");
                SetLastError(ERROR_INTERNAL_ERROR);
                bRet = FALSE;
                goto SetEventExit;
            }

            /* check whether thread is already awake. this can happen if 
               another object already woke up the thread, but the thread hasn't
               yet had time to remove itself from all waiting lists. */
            pAwakenState = SHMPTR_TO_PTR(pWaitingThread->state.shmAwakened);

            if(!THREADInterlockedAwaken(pAwakenState, FALSE))
            {
                TRACE("thread is already awake, skipping it\n");
                prevThread = pWaitingThread;
                shmWaitingThread = pWaitingThread->ptr.shmNext;
                continue;
            }

            /* remove thread from waiting list */
            pTemp = shmWaitingThread;
            shmWaitingThread = pWaitingThread->ptr.shmNext;
            if(NULL == prevThread)
            {
                pEventInfo->waitingThreads = shmWaitingThread;
            }
            else
            {
                prevThread->ptr.shmNext = shmWaitingThread;
            }

            TRACE("Waking up thread(%#x) Event has been set (%p)\n",
                  pWaitingThread->threadId, hEvent);

            WakeUpThread(pWaitingThread->threadId, 
                         pWaitingThread->processId,
                         pWaitingThread->blockingPipe,
                         WUTC_SIGNALED );

            SHMfree(pTemp);

            /* if the event is auto-reset, we only want to wake up one thread, 
               so break out.*/
            if (pEventInfo->manualReset == FALSE)
            {
                pEventInfo->state = FALSE;
                break;
            }
        }
    }

SetEventExit:
    SHMRelease();
    HMGRUnlockHandle(hEvent, &pEvent->objHeader);
    
    LOGEXIT("SetEvent returns BOOL %d\n", bRet);
    PERF_EXIT(SetEvent);
    return bRet;
}


/*++
Function:
  ResetEvent

See MSDN doc.
--*/
BOOL
PALAPI
ResetEvent(
       IN HANDLE hEvent)
{
    BOOL bRet = TRUE;
    Event *pEvent;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;

    PERF_ENTRY(ResetEvent);
    ENTRY("ResetEvent(hEvent=%p)\n", hEvent);

    pEvent = (Event *) HMGRLockHandle2(hEvent, HOBJ_EVENT);

    SHMLock();

    if ( pEvent == NULL )
    {
        ERROR("Invalid handle\n");
        SetLastError(ERROR_INVALID_HANDLE);
        bRet = FALSE;
    }
    else
    {
        pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(pEvent->info);
        
        if (pEventInfo == NULL)
        {
            ASSERT("Invalid shared memory pointer\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            SHMRelease();
            HMGRUnlockHandle(hEvent, &pEvent->objHeader);
            LOGEXIT("ResetEvent returns BOOL FALSE\n");
            PERF_EXIT(ResetEvent);
            return FALSE;
        }

        pEventInfo->state = FALSE;

        TRACE("Event(%p) being reset.\n", hEvent);
    }

    SHMRelease();
    if( NULL != pEvent)
    {
        HMGRUnlockHandle(hEvent, &pEvent->objHeader);
    }
    
    LOGEXIT("ResetEvent returns BOOL %d\n", bRet);
    PERF_EXIT(ResetEvent);
    return bRet;
}


/*++
Function:
  OpenEventW

Note:
  dwDesiredAccess is always set to EVENT_ALL_ACCESS

See MSDN doc.
--*/
HANDLE
PALAPI
OpenEventW(
       IN DWORD dwDesiredAccess,
       IN BOOL bInheritHandle,
       IN LPCWSTR lpName)
{
    HANDLE hEvent = NULL;
    SHMPTR shmpEventInfo;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    Event *pEvent;
    LPCWSTR lpObjectName = NULL;
    WCHAR MangledObjectName[MAX_PATH];
    BOOL nameExists;

    PERF_ENTRY(OpenEventW);
    ENTRY("OpenEventW(dwDesiredAccess=%#x, bInheritHandle=%d, lpName=%p (%S))\n", 
          dwDesiredAccess, bInheritHandle, lpName?lpName:W16_NULLSTRING, lpName?lpName:W16_NULLSTRING);

    SHMLock();

    /* validate parameters */
    if (dwDesiredAccess != EVENT_ALL_ACCESS)
    {
        ASSERT("dwDesiredAccess should be EVENT_ALL_ACCESS\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto OpenEventWExit;
    }

    if (lpName == NULL)
    {
        ASSERT("name is NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto OpenEventWExit;            
    }
    lpObjectName=lpName;
    if (!MangleObjectNameW(&lpObjectName, MangledObjectName))
    {
        goto OpenEventWExit;
    }
    
    /* See if the name is already taken. */
    shmpEventInfo = SHMFindNamedObjectByName( lpObjectName, SHM_NAMED_EVENTS,
                                              &nameExists );

    if ( shmpEventInfo == 0 )
    {
        if (nameExists)
        {
            SetLastError(ERROR_INVALID_HANDLE);
        }
        else
        {
            SetLastError(ERROR_INVALID_NAME);
        }
        TRACE("Event (name = %S) not found\n", lpName);
        goto OpenEventWExit;
    }
    else 
    {
        /* build a Event structure */
        pEvent = (Event *) malloc(sizeof(Event));

        if (pEvent == NULL)
        {
            ERROR("Unable to allocate memory\n");
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto OpenEventWExit;
        }

        pEvent->objHeader.type = HOBJ_EVENT;
        pEvent->objHeader.close_handle = CloseEventHandle;
        pEvent->objHeader.dup_handle = DupEventHandle;
        pEvent->refCount = 1;
        pEvent->info = shmpEventInfo;

        /* get a handle for the event */
        hEvent = HMGRGetHandle((HOBJSTRUCT *) pEvent);

        if (hEvent == INVALID_HANDLE_VALUE)
        {
            ERROR("Unable to get a handle from the handle manager\n");
            hEvent = NULL;
            free(pEvent);
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto OpenEventWExit;
        }

        pEventInfo = SHMPTR_TO_PTR(shmpEventInfo);

        if (pEventInfo == NULL)
        {
            ASSERT("Invalid shared memory pointer\n");
            HMGRFreeHandle(hEvent);
            SetLastError(ERROR_INTERNAL_ERROR);
            goto OpenEventWExit;
        }

        /* increase the refCount in shared memory */
        pEventInfo->refCount += 1;

        TRACE("Event (name=%S) found\n", lpName);
    }

OpenEventWExit:
    SHMRelease();
    LOGEXIT("OpenEventW returns HANDLE %p\n", hEvent);
    PERF_EXIT(OpenEventW);

    return hEvent;
}

/*++
Function:
  EventWaitOn

  Check if the event is signaled. If not signaled place
  the current thread to the list of waiting thread. This function is
  called by WaitXXXX functions when it's time to wait on a Event.
  The caller is responsible for blocking the thread, this function does
  not wait on the event.

  This function set the last error, if an error occured.

Parameters:
    IN hEvent: handle of the event checked for the signaled state.
    SHMPTR wait_state : shared memory pointer to waiting thread's wait state
    
returns
    WAITON_CODE value (see thread.h)
--*/
int
EventWaitOn(
    IN HANDLE hEvent, SHMPTR wait_state)
{
    Event *pEvent;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    int ret;
    ThreadWaitingList *pWaitingThread;
    SHMPTR shmpWaitingThread;

    pEvent = (Event *) HMGRLockHandle2(hEvent, HOBJ_EVENT);

    SHMLock();

    if ( pEvent == NULL )
    {
        SetLastError(ERROR_INVALID_HANDLE);
        ERROR("Invalid event handle %p\n", hEvent);
        SHMRelease();
        return WOC_ERROR;
    }

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(pEvent->info);

    if (pEventInfo == NULL)
    {
        SetLastError(ERROR_INTERNAL_ERROR);
        ASSERT("Invalid shared memory pointer\n");
        SHMRelease();
        HMGRUnlockHandle(hEvent,&pEvent->objHeader);
        return WOC_ERROR;
    }

    if (pEventInfo->state)
    {
        DWORD *pAwakenState;

        /* try to flag the thread as awakened, don't add it to the list if it 
           already was. (this happens if an object is signaled while WFMO is 
           still calling WaitOn()) */
        pAwakenState = SHMPTR_TO_PTR(wait_state);

        if(!THREADInterlockedAwaken(pAwakenState,FALSE))
        {
            TRACE("thread is already awake; not waiting on it\n");
            ret = WOC_INTERUPTED;
        }
        else
        {
            TRACE("Event(%p) is signaled, not waiting\n", hEvent);
            ret = WOC_SIGNALED;
            
            /* reset the event, if it's not a manual reset event */
            if (pEventInfo->manualReset == FALSE)
            {
                pEventInfo->state = FALSE;
            }            
        }
    }
    else
    {
        /* this structure needs to be allocated in the shared memory in 
           term of supporting event for inter-process synchronization */
        /* add the current thread to the list of waiting threads */
        shmpWaitingThread = SHMalloc(sizeof(ThreadWaitingList));

        if (shmpWaitingThread == (SHMPTR) NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            ERROR("Not enough memory to allocate a ThreadWaitingList\n");
            ret = WOC_ERROR;
        }
        else
        {
            pWaitingThread = (ThreadWaitingList *)
                                 SHMPTR_TO_PTR(shmpWaitingThread);

            pWaitingThread->threadId = GetCurrentThreadId();
            pWaitingThread->processId = GetCurrentProcessId();
            pWaitingThread->blockingPipe = THREADGetPipe();
            pWaitingThread->ptr.shmNext = (SHMPTR) NULL;
            pWaitingThread->state.shmAwakened = wait_state;

            TRACE("ThreadId=%#x will wait on hEvent=%p\n", 
                  pWaitingThread->threadId, hEvent);

            /* place the thread at the end of the list */
            if (pEventInfo->waitingThreads == (SHMPTR) NULL)
            {
                pEventInfo->waitingThreads = shmpWaitingThread;
            }
            else
            {
                ThreadWaitingList *pIter;
                ThreadWaitingList *pNext;

                pIter = (ThreadWaitingList *)
                            SHMPTR_TO_PTR(pEventInfo->waitingThreads);

                while (pIter && pIter->ptr.shmNext)
                {
                    pNext = (ThreadWaitingList *)
                                 SHMPTR_TO_PTR(pIter->ptr.shmNext);
                    if (NULL == pNext)
                    {
                        ERROR("Invalid shared memory pointer\n");

                        /* the list is corrupted, but let's try to keep going
                           anyway with the part that's still good */
                        pIter->ptr.shmNext = 0;
                    }
                    else
                    {
                        pIter = pNext;
                    }
                }

                if(NULL == pIter)
                {
                    ERROR("Invalid shared memory pointer\n");

                    /* the list is completely corrupted... start a new one */
                    pEventInfo->waitingThreads = shmpWaitingThread;
                }
                else
                {
                    pIter->ptr.shmNext = shmpWaitingThread;
                }
            }

            TRACE("Event(%p) is not signaled,"
                  " current thread added to the waiting list\n",
                  hEvent);

            ret = WOC_WAITING;
        }
    }
    SHMRelease();
    HMGRUnlockHandle(hEvent,&pEvent->objHeader);

    return ret;
}

/*++
Function:
  EventRemoveWaitingThread

  Remove the CurrentThreadId from the list of waiting thread. This
  function is called when the current thread stops waiting on an Event
  for a different reason than the Event was signaled. (e.g. a timeout,
  or the thread was waiting on multiple objects and another object was
  signaled)

Parameters:
    IN hEvent:   event handle of the modify thread waiting list.    

returns
    -1: an error occurred
    0: if the thread wasn't found in the list.
    1: if the thread was removed from the waiting list.
--*/
int
EventRemoveWaitingThread(
    IN HANDLE hEvent)
{
    Event *pEvent;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    int ret = 0;
    ThreadWaitingList *pWaitingThread;
    ThreadWaitingList *pNextWaitingThread;
    SHMPTR shmpWaitingThread;
    DWORD CurrentThreadId;
    DWORD currentPID;

    pEvent = (Event *) HMGRLockHandle2(hEvent, HOBJ_EVENT);

    SHMLock();

    if ( (pEvent == NULL) || (!IsValidEventObject(pEvent)) )
    {
        ASSERT("Invalid event handle %p\n", hEvent);
        SHMRelease();
        return -1;
    }

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(pEvent->info);

    if (pEventInfo == NULL)
    {
        ASSERT("Invalid shared memory pointer\n");
        SHMRelease();
        HMGRUnlockHandle(hEvent,&pEvent->objHeader);
        return -1;
    }

    CurrentThreadId = GetCurrentThreadId();
    currentPID = GetCurrentProcessId();
    shmpWaitingThread = pEventInfo->waitingThreads;

    if (shmpWaitingThread == (SHMPTR) NULL)
    {        
        ret = 0;  /* list is empty */
        goto RemoveThreadExit;
    }

    pWaitingThread = (ThreadWaitingList *) SHMPTR_TO_PTR(shmpWaitingThread);

    if (pWaitingThread == NULL)
    {   
        ERROR("Invalid shared memory pointer\n");
        ret = 0;  /* list is empty */
        goto RemoveThreadExit;
    }

    /* check if it is the first element in the list */
    if (pWaitingThread->threadId == CurrentThreadId &&
        pWaitingThread->processId == currentPID)
    {
        pEventInfo->waitingThreads = pWaitingThread->ptr.shmNext;
        SHMfree(shmpWaitingThread);
        ret = 1;
        goto RemoveThreadExit;
    }
    else
    {
        while (pWaitingThread->ptr.shmNext)
        {
            pNextWaitingThread = (ThreadWaitingList *) 
                SHMPTR_TO_PTR(pWaitingThread->ptr.shmNext);

            if (pNextWaitingThread == NULL)
            {
                ERROR("Invalid shared memory pointer\n");
                ret = 0;
                break;
            }

            if (pNextWaitingThread->threadId == CurrentThreadId &&
                pNextWaitingThread->processId == currentPID)
            {
                /* found, so remove it */
                SHMPTR pTemp;
                pTemp = pWaitingThread->ptr.shmNext;
                pWaitingThread->ptr.shmNext = pNextWaitingThread->ptr.shmNext;
                SHMfree (pTemp);
                ret = 1;
                break;
            }

            pWaitingThread = pNextWaitingThread; 
        }        
    }

RemoveThreadExit:
    SHMRelease();
    HMGRUnlockHandle(hEvent,&pEvent->objHeader);

    if (ret == 0)
    {
        TRACE("ThreadId=%#x was not waiting on hEvent=%p\n",
             CurrentThreadId, hEvent);
    }
    else if (ret == 1)
    {
        TRACE("ThreadId=%#x is no longer waiting on hEvent=%p\n",
              CurrentThreadId, hEvent);
    }

    return ret;
}


/*++
Function:
  CloseEventHandle

parameters, return value : see definition of CLOSEHANDLEFUNC in handle.h
--*/
int 
CloseEventHandle( 
    HOBJSTRUCT *handle_data)
{
    Event *pEvent = (Event *) handle_data;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    SHMPTR pTemp;
    ThreadWaitingList *pWaitingThread;
    SHMPTR shmevent;

    SHMLock();

    if (!IsValidEventObject(pEvent))
    {
        SHMRelease();
        ASSERT("Invalid event handle\n");
        return -1;
    }

    shmevent = pEvent->info;

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(shmevent);

    if (pEventInfo == NULL)
    {
        SHMRelease();
        ERROR("Invalid shared memory pointer\n");
        return -2;
    }

    /* we need two refCount, because other processes could refere to
       GLOBAL_EVENT_SYSTEM_OBJECT struct in shared memory */
    pEvent->refCount--;
    pEventInfo->refCount--;

    if (pEvent->refCount == 0)
    {
        pEvent->objHeader.type = HOBJ_INVALID;
        free(pEvent);
    }

    if (pEventInfo->refCount > 0)
    {
        SHMRelease();
        TRACE("Removed reference to event object %p, refcount is still >0.\n",
              handle_data);
        return 0; /* there's still other handles referencing this object */
    }

    if (pEventInfo->ShmHeader.ShmObjectName != (SHMPTR) NULL)
    {
        /* Remove the named event from the global link list */
        SHMRemoveNamedObject( pEvent->info );
    }                                 

    /* free the thread waiting list */
    while(pEventInfo->waitingThreads)
    {
        pTemp = pEventInfo->waitingThreads;

        pWaitingThread = (ThreadWaitingList *) SHMPTR_TO_PTR(pTemp);

        if (pWaitingThread == NULL)
        {
            ASSERT("Invalid shared memory pointer\n");
            pEventInfo->waitingThreads = 0;
            break;
        }
        pEventInfo->waitingThreads = pWaitingThread->ptr.shmNext;

        SHMfree(pTemp);        
    }

    SHMfree(shmevent);

    SHMRelease();

    TRACE("Last reference to event object %p released; object destroyed.\n",
          handle_data);

    return 0;
}


/*++
Function:
  DuplicateEventHandle

parameters, return value : see definition of DUPHANDLEFUNC in handle.h
--*/
static 
int 
DupEventHandle( 
    HANDLE handle, 
    HOBJSTRUCT *handle_data)
{
    Event *pEvent = (Event *) handle_data;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;

    SHMLock();

    if (!IsValidEventObject(pEvent))
    {
        SHMRelease();
        ASSERT("Invalid event handle\n");
        return -1;
    }

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR(pEvent->info);

    if (pEventInfo == NULL)
    {
        SHMRelease();
        ERROR("Invalid shared memory pointer\n");
        return -2;
    }

    /* we need two refCount, because other processes could refere to
       GLOBAL_EVENT_SYSTEM_OBJECT struct in shared memory */
    pEvent->refCount++;
    pEventInfo->refCount++;

    SHMRelease();

    return 0;
}

/*++
Function:
  EventLocalToRemote

Parameters, 
    HOBJSTRUCT*       : Event Object Structure
    HREMOTEOBJSTRUCT* : Remote Object Structure

Return value : TRUE if functions succeeded, FALSE otherwise.

--*/
BOOL EventLocalToRemote(HOBJSTRUCT* handle_data,
                        HREMOTEOBJSTRUCT *remote_handle_data)
{
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo;
    Event* pEvent;

    remote_handle_data->type = handle_data->type;
    
    pEvent = (Event *)handle_data;

    if (!IsValidEventObject(pEvent))
    {
    
        ASSERT("Invalid event handle\n");
        return  FALSE;
    }

    remote_handle_data->ShmKernelObject =  pEvent->info;
    
    /* increment shared memory reference count */
    SHMLock();
    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*)
                 SHMPTR_TO_PTR( remote_handle_data->ShmKernelObject );
    if (pEventInfo == NULL)
    {
        SHMRelease();
        ERROR("Invalid shared memory pointer\n");
        return FALSE;
    }
    pEventInfo->refCount++;
    SHMRelease();

    return TRUE;
}

/*++
Function :
    EventRemoteToLocal

    Creates a even handle using a Remote Handle from another process

Parameters, 
    HREMOTEOBJSTRUCT* : Remote Object Structure
    
Returns
    Handle to the instantiated event object

--*/
HANDLE EventRemoteToLocal(HREMOTEOBJSTRUCT *remote_handle_data)
{
    HANDLE hEvent = INVALID_HANDLE_VALUE;
    Event* pEvent = NULL;
    GLOBAL_EVENT_SYSTEM_OBJECT *pEventInfo = NULL;
    SHMPTR shmpEventInfo = remote_handle_data->ShmKernelObject;

    pEventInfo = (GLOBAL_EVENT_SYSTEM_OBJECT*) SHMPTR_TO_PTR( shmpEventInfo );
    if (pEventInfo == NULL)
    {
        ASSERT("Invalid shared memory pointer\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto Exit;
    }

    /* build a Event structure */
    pEvent = (Event *) malloc(sizeof(Event));
    if (pEvent == NULL)
    {
        ERROR("Unable to allocate memory\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Exit;
    }

    pEvent->objHeader.type = HOBJ_EVENT;
    pEvent->objHeader.close_handle = CloseEventHandle;
    pEvent->objHeader.dup_handle = DupEventHandle;
    pEvent->refCount = 1;
    pEvent->info = shmpEventInfo;

    /* get a handle for the event */
    hEvent = HMGRGetHandle((HOBJSTRUCT *) pEvent);
    if (hEvent == INVALID_HANDLE_VALUE)
    {
        ERROR("Unable to get a handle from the handle manager\n");
        free(pEvent);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Exit;
    }        

Exit:
    return hEvent;    
}


/*++
Function:
    isValidEventObject

Abstract:
    Verify if the object is really an event by checking the magic field
  
Parameters:
    IN  pEvent: Event object

Return:
    TRUE: if valid
    FALSE: otherwise
--*/
static 
BOOL
IsValidEventObject(
      Event *pEvent)
{
    return (pEvent->objHeader.type = HOBJ_EVENT);
}



