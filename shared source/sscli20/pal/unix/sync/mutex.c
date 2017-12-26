/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    mutex.c

Abstract:

    Implementation of mutex synchroniztion object as described in 
    the WIN32 API

Revision History:

--*/

#include "pal/palinternal.h"

#include "pal/dbgmsg.h"
#include "pal/mutex.h"
#include "pal/critsect.h"
#include "pal/shmemory.h"
#include "pal/thread.h"
#include "pal/init.h"

CRITICAL_SECTION mutex_critsec;

static PMUTEX_HANDLE_OBJECT pMutexHandle = NULL;

SET_DEFAULT_DEBUG_CHANNEL(MUTEX);


static BOOL MutexIsValidMutexHandle( HOBJSTRUCT * );
#if defined _DEBUG
static void MutexDisplaySharedMemory( void );
#endif
static void MutexDeleteSharedObject(SHMPTR shmPtr);
static HANDLE MutexAddObjectToHandleManager( SHMPTR shmObject );
/*++
Function :
    MutexInitialize

    Initialize the critical sections.

Return value:
    TRUE  if initialization succeeded
    FALSE otherwise
--*/
BOOL
MutexInitialize( void )
{
    TRACE( "Initializing the mutext critical section.\n" );
    return (0 == SYNCInitializeCriticalSection( &mutex_critsec ));
}

/*++
Function :
    MutexCleanup

    Deletes the critical sections.

--*/
void MutexCleanup( void )
{
    TRACE( "Deleting the mutext critical section.\n" );
    DeleteCriticalSection( &mutex_critsec );
}

/*++
Function :

    MutexAddThreadToList( pKernelObject );

    Adds the current thread to the end of the thread list.
    
    Takes a PGLOBAL_MUTEX_SYSTEM_OBJECT.
    
    Returns TRUE on success, FALSE on failure.
--*/
static BOOL MutexAddThreadToList( PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject,
                                  SHMPTR wait_state )
{
    ThreadWaitingList * pNewThread = NULL;
    SHMPTR ShmThread = 0;
    ThreadWaitingList * pThread = NULL;
    BOOL bRetVal = FALSE;

    TRACE( "Entered.\n" );
    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    SHMLock();

    ShmThread = SHMalloc( sizeof( ThreadWaitingList ) );
    
    if ( ShmThread != 0 )
    {
        pNewThread = (ThreadWaitingList*)SHMPTR_TO_PTR( ShmThread );
        pNewThread->blockingPipe = THREADGetPipe();
        pNewThread->processId = GetCurrentProcessId();
        pNewThread->threadId = GetCurrentThreadId();
        pNewThread->ptr.shmNext = 0;
        pNewThread->state.shmAwakened = wait_state;
    }
    else
    {
        ERROR( "Unable to allocate shared memory for the structure.\n" );
        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
        goto Exit;
    }
    
    if ( 0 != pKernelObject->ShmWaitingForThreadList )
    {
        pThread = 
            (ThreadWaitingList*)SHMPTR_TO_PTR( 
                pKernelObject->ShmWaitingForThreadList );
    }

    /* Add to the end of the list. */
    while ( pThread && pThread->ptr.shmNext )
    {
        pThread = (ThreadWaitingList*)SHMPTR_TO_PTR( pThread->ptr.shmNext );
    }

    if ( !pThread )
    {
        /* First thread. */
        pKernelObject->ShmWaitingForThreadList = ShmThread;
    }
    else
    {
        /* Added to end of list. */
        pThread->ptr.shmNext = ShmThread;
    }

    bRetVal = TRUE;
Exit:
    SHMRelease();
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
    return bRetVal;
}


/*++
Function : 
    
    MutexWaitOn:

    Check if the mutex is signaled. If not signaled place
    the current thread to the list of waiting thread. This function is
    called by WaitXXXX functions when it's time to wait on a Mutex.
    The caller is responsible for blocking the thread, this function does
    not wait on the mutex.

Parameters
    IN hMutex   mutex checked to see if it is signaled
    SHMPTR wait_state : shared memory pointer to waiting thread's wait state

returns
    WAITON_CODE value
--*/

INT MutexWaitOn( IN HANDLE hMutex, SHMPTR wait_state )
{
    PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject = NULL;
    PMUTEX_HANDLE_OBJECT pHandleObject = NULL;
    INT RetVal = -1;
    pHandleObject = NULL; 
    
    TRACE( "Entered.\n" );

    pHandleObject = (PMUTEX_HANDLE_OBJECT)HMGRLockHandle2( hMutex , HOBJ_MUTEX);

    if ( NULL==pHandleObject )
    {
        ASSERT( "Not a valid mutex handle.\n" );
        SetLastError( ERROR_INVALID_HANDLE );
        return WOC_ERROR;
    }
    
    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    SHMLock();

    pKernelObject = 
        (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( pHandleObject->ShmKernelObject );

    if ( pKernelObject )
    {
        DWORD *pAwakenState;
        
        if ( pKernelObject->Owner.ProcessId == GetCurrentProcessId() &&
             pKernelObject->Owner.ThreadId == GetCurrentThreadId() )
        {
            /* try to flag the thread as awakened, don't increment the mutex 
               lock count if it's already awake */
            pAwakenState = SHMPTR_TO_PTR(wait_state);
            
            if(!THREADInterlockedAwaken(pAwakenState, FALSE))
            {   
                TRACE("thread is already awake; not giving it the mutex\n");
                RetVal = WOC_INTERUPTED;
            }
            else
            {
                TRACE( "Incrementing the mutex count.\n" );
                pKernelObject->Mutex_Count++;
                RetVal = WOC_SIGNALED;
            }
        }
        else if ( pKernelObject->Owner.ProcessId == 0 &&
                  pKernelObject->Owner.ThreadId == 0 )
        {

            /* try to flag the thread as awakened, don't give it mutex 
               ownership if it's already awake */
            pAwakenState = SHMPTR_TO_PTR(wait_state);
            
            if(!THREADInterlockedAwaken(pAwakenState, FALSE))
            {   
                TRACE("thread is already awake; not giving it the mutex\n");
                RetVal = WOC_INTERUPTED;
            }
            else
            {
                TRACE( "Setting the current owner to Process=%d, Thread=%d.\n",
                       GetCurrentProcessId(), GetCurrentThreadId() );
    
                pKernelObject->Owner.ProcessId = GetCurrentProcessId();
                pKernelObject->Owner.ThreadId = GetCurrentThreadId();
                pKernelObject->Mutex_Count = 1;
                if(pKernelObject->abandoned)
                {
                    RetVal = WOC_ABANDONED;
                    pKernelObject->abandoned = FALSE;
                }
                else
                {
                    RetVal = WOC_SIGNALED;
                }
            }
        }
        else
        {
            /* Add the thread to the waiting thread list. */
            if ( MutexAddThreadToList( pKernelObject, wait_state ) )
            {
                RetVal = 1;
            }
            else
            {
                ASSERT( "Unable to add the thread to the waiting thread list!\n" );
                SetLastError( ERROR_INTERNAL_ERROR );
                RetVal = -1;
            }
        }
    }
    else
    {
        ASSERT( "Something went wrong while attempting to retrieve the mutex "
               "object from the shared memory store.\n " );
        SetLastError( ERROR_INTERNAL_ERROR );
        RetVal = -1;
    }

    SHMRelease();
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
    HMGRUnlockHandle(hMutex,&pHandleObject->HandleHeader);

#if defined _DEBUG
    MutexDisplaySharedMemory();
#endif
    
    TRACE("returning %d\n",RetVal);
    return RetVal;
}

/*++
Function :

    MutexRemoveWaitingThread

        Remove the CurrentThreadId from the list of waiting thread. This
        function is called when the current thread stops waiting on a Mutex
        for a different reason than the Mutex was signaled. (e.g. a timeout,
        or the thread was waiting on multiple objects and another object was
        signaled)

Parameters
    
        IN hMutex   mutex which the waiting thread list is updated

Returns
        -1: an error occurred
        0: if the thread wasn't found in the list.
        1: if the thread was removed from the waiting list.
--*/

INT MutexRemoveWaitingThread( IN HANDLE hMutex )
{
    PMUTEX_HANDLE_OBJECT pMutexHandleObject = NULL;
    PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject = NULL;
    
    ThreadWaitingList *pWaitingThread = NULL;
    DWORD CurrentThreadId = -1;
    DWORD CurrentProcessId = -1;
    INT RetVal = -1;

    TRACE( "Entered.\n" );

    pMutexHandleObject = (PMUTEX_HANDLE_OBJECT)HMGRLockHandle2( hMutex, HOBJ_MUTEX );

    if ( pMutexHandleObject == NULL)
    {
        ASSERT( "Invalid mutex handle %p\n", hMutex );
        return -1;
    }

    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    SHMLock();

    /* The waiting thread list is stored in shared memory, as mutex's are
    shared across processes. */
    pKernelObject = 
        (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( 
            pMutexHandleObject->ShmKernelObject );

    if ( pKernelObject == 0 )
    {
        ASSERT( "Unable to retrieve the waiting thread list from shared memory.\n" );
        goto RemoveThreadExit;
    }
    
    pWaitingThread = 
        (ThreadWaitingList*)SHMPTR_TO_PTR( pKernelObject->ShmWaitingForThreadList );

    CurrentThreadId = GetCurrentThreadId();
    CurrentProcessId = GetCurrentProcessId();
    
    if ( pWaitingThread == NULL )
    {        
        RetVal = 0;  /* List is empty */
        goto RemoveThreadExit;
    }

    /* Check if it is the first element in the list. */
    if ( pWaitingThread->threadId == CurrentThreadId && 
         pWaitingThread->processId == CurrentProcessId )
    {
        SHMPTR temp = pKernelObject->ShmWaitingForThreadList;
        pKernelObject->ShmWaitingForThreadList = pWaitingThread->ptr.shmNext;
        SHMfree( temp );
        RetVal = 1;
        goto RemoveThreadExit;
    }
    else
    {
        ThreadWaitingList * pNextWaitingThread = NULL;

        /* return 0 if not found */
        RetVal = 0;

        while ( pWaitingThread && pWaitingThread->ptr.shmNext )
        {
            pNextWaitingThread = 
                (ThreadWaitingList*)SHMPTR_TO_PTR( pWaitingThread->ptr.shmNext );

            if ( pNextWaitingThread->threadId == CurrentThreadId && 
                 pNextWaitingThread->processId == CurrentProcessId ) 
            {
                /* Found it so remove it */
                SHMPTR shmTemp = 0;

                shmTemp = pWaitingThread->ptr.shmNext;
                pWaitingThread->ptr.shmNext = pNextWaitingThread->ptr.shmNext;
                
                SHMfree ( shmTemp );
                RetVal = 1;
                break;
            }

            pWaitingThread = pNextWaitingThread; 
        }
    }

RemoveThreadExit:
    SHMRelease();
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
    HMGRUnlockHandle(hMutex,&pMutexHandleObject->HandleHeader);

    if ( RetVal == 0 )
    {
        TRACE( "ThreadId=%#x was not waiting on hMutex=%p\n",
             CurrentThreadId, hMutex );
    }
    else
    {
        TRACE( "ThreadId=%#x is no longer waiting on hMutex=%p\n",
              CurrentThreadId, hMutex );
    }

    return RetVal;
}


/*++
Function :

    MutexIsValidMutexHandle
    
    Returns TRUE if the handle is a mutex handle.
    
    FALSE otherwise.
--*/
static BOOL MutexIsValidMutexHandle( HOBJSTRUCT * pHandleData )
{
    if ( pHandleData && pHandleData->type == HOBJ_MUTEX )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


#if defined _DEBUG

/*++
Function :

    MutexDisplaySharedMemory

    Displays all the mutex kernel objects that are in memory.
--*/
static void MutexDisplaySharedMemory()
{
    PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject = NULL;
    SHMPTR shmptr;

    DBGOUT( "======================== SHARED MEMORY ========================\n" );

    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    SHMLock();

    shmptr = SHMGetInfo( SIID_NAMED_OBJECTS );
    if(0 != shmptr)
    {
        pKernelObject = (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR(shmptr );
    }
    else
    {
        pKernelObject = NULL;
    }
    while ( pKernelObject )
    {
        if ( pKernelObject->ShmHeader.ObjectType == SHM_NAMED_MUTEXS )
        {
            INT StringSize = 0;
            LPWSTR source = NULL;
            LPSTR dest = NULL;
            ThreadWaitingList * threadList;

            if( 0 != pKernelObject->ShmWaitingForThreadList)
            {
                threadList = SHMPTR_TO_PTR(
                    pKernelObject->ShmWaitingForThreadList );
            }
            else
            {
                threadList = NULL;
            }

            if ( 0 != pKernelObject->ShmHeader.ShmObjectName )
            {
                source = 
                    (LPWSTR)SHMPTR_TO_PTR( pKernelObject->ShmHeader.ShmObjectName );
            }

            StringSize = WideCharToMultiByte(CP_ACP, 0, source, -1, NULL, 0,
                                             NULL,NULL);
            if(StringSize <= 0)
            {
                ERROR( "Invalid Parameter.\n" );
                goto done;
            }
            dest = (LPSTR)malloc( StringSize );
            if (!dest )
            {
                ERROR( "Unable to allocate memory.\n" );
                goto done;
            }
            WideCharToMultiByte(CP_ACP,0,source,-1,dest,StringSize,NULL,NULL);

            DBGOUT( "Mutex's name is \t\t%s.\n", dest );

            DBGOUT( "Initial Owner : \n \t\t ProcessId: \t%d \n\t\t ThreadId:"
                    " \t%d\n",pKernelObject->Owner.ProcessId,
                    pKernelObject->Owner.ThreadId );

            DBGOUT( "Mutex count \t\t\t%d.\n", pKernelObject->Mutex_Count );
            DBGOUT( "Reference count \t\t%d.\n", pKernelObject->Ref_Count );

            DBGOUT( "Waiting Thread List : \n" );

            while ( threadList )
            {
                DBGOUT( "\t\t\t\tBlocking pipe =\t%d.\n", threadList->blockingPipe );
                DBGOUT( "\t\t\t\tProcess ID =\t%d.\n", threadList->processId );
                DBGOUT( "\t\t\t\tThread ID =\t%d.\n", threadList->threadId );

                if ( 0 != threadList->ptr.shmNext )
                {
                    threadList = 
                        (ThreadWaitingList*)SHMPTR_TO_PTR( threadList->ptr.shmNext );
                }
                else
                {
                    threadList = NULL;
                }
            }
            free(dest);
        }
        if(0 != pKernelObject->ShmHeader.ShmNext)
        {
            pKernelObject = SHMPTR_TO_PTR( pKernelObject->ShmHeader.ShmNext );
        }
        else
        {
            pKernelObject = NULL;
        }
    }
done:
    DBGOUT( "\n\n" );
    SHMRelease();
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
}
/*++
Function :

    MutexDisplayMapHandleObjects

    Displays all the mutex handle objects.
--*/
static void MutexDisplayMapHandleObjects()
{
    PMUTEX_HANDLE_OBJECT pObject = NULL;
    LPWSTR mutex_name;
    PSHM_NAMED_OBJECTS shm_object;

    DBGOUT( "======================== HANDLE OBJECTS ========================\n" );

    SYNCEnterCriticalSection( &mutex_critsec , TRUE);

    pObject = pMutexHandle;

    while ( pObject )
    {
        SHMLock();
        shm_object = SHMPTR_TO_PTR( pObject->ShmKernelObject );
        if( 0 != shm_object->ShmObjectName)
        {
            mutex_name = SHMPTR_TO_PTR(shm_object->ShmObjectName);
            DBGOUT( "Mutex object %p is named\t %S.\n", pObject, mutex_name);
        }
        else
        {
            DBGOUT( "Mutex object %p is not named.\n", pObject);
        }

        
        SHMRelease();

        DBGOUT( "Reference count \t\t\t\t %d.\n", pObject->Ref_Count );
        DBGOUT( "Next element \t\t\t\t\t %p.\n", pObject->pNext );
        DBGOUT( "Previous element \t\t\t\t %p.\n", pObject->pPrev );

        pObject = pObject->pNext;
    }

    DBGOUT( "\n\n" );
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
}
#endif


/*++
Function :
    MutexDuplicateHandle

    Duplicates the mutex handle.

parameters, return value : see definition of DUPHANDLEFUNC in handle.h

--*/
static INT MutexDuplicateHandle( HANDLE Handle, HOBJSTRUCT * pHandleData )
{
    INT retval;
    PMUTEX_HANDLE_OBJECT pHandleInfo = (PMUTEX_HANDLE_OBJECT)pHandleData;
    
    TRACE( "Entered.\n" );

    if ( MutexIsValidMutexHandle( pHandleData ) )
    {
        PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject = NULL;

        SYNCEnterCriticalSection( &mutex_critsec , TRUE);
        (pHandleInfo->Ref_Count)++;
        
        /* Increment the kernel object reference counter. */
        SHMLock();
        pKernelObject = 
            (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( 
                pHandleInfo->ShmKernelObject );
        if(NULL == pKernelObject)
        {
            ASSERT("Shared memory pointer %08x is invalid!\n",
                  pHandleInfo->ShmKernelObject );
            retval = -2;
        }
        else
        {
            (pKernelObject->Ref_Count)++;
            TRACE("Object's reference count is now %d\n", 
                  pKernelObject->Ref_Count );
            retval = 0;
        }

        SHMRelease();

        SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
        
#if defined _DEBUG
        MutexDisplaySharedMemory();
        MutexDisplayMapHandleObjects();
#endif

        return retval;
    }
    else
    {
        ASSERT( "pHandleData %p is not a mutex object. \n", pHandleData );
        return -1;
    }
}

/*++
Function :
    MutexLocalToRemote

    Makes a mutex handle accessible to another process using a Remote Handle

Parameters, 
    IN handle_data         : structure associated with this handle
    IN remote_handle_data  : structure associated with the remote handle

Return value : TRUE if functions succeeded, FALSE otherwise.

--*/
BOOL MutexLocalToRemote(HOBJSTRUCT *handle_data, 
                        HREMOTEOBJSTRUCT *remote_handle_data)
{
    PGLOBAL_MUTEX_SYSTEM_OBJECT pMutexObject;

    if (!MutexIsValidMutexHandle(handle_data))
    {
        ASSERT("Invalid event handle\n");
        return FALSE;
    }

    remote_handle_data->type = handle_data->type;
    remote_handle_data->ShmKernelObject = 
                        ((PMUTEX_HANDLE_OBJECT) handle_data)->ShmKernelObject;

    /* Increment the mutex's shared memory reference count */
    SHMLock();
    pMutexObject = (PGLOBAL_MUTEX_SYSTEM_OBJECT)
                   SHMPTR_TO_PTR( remote_handle_data->ShmKernelObject );
    if (pMutexObject == NULL)
    {    
        SHMRelease();
        ERROR("Invalid shared memory pointer\n");
        return FALSE;
    }
    (pMutexObject->Ref_Count)++;
    SHMRelease();
 
    return TRUE;
}


/*++
Function :
    MutexLocalToRemote

    Creates a mutex handle using a Remote Handle from another process

Parameters, 
    IN remote_handle_data  : structure associated with the remote handle
    
Returns
    Handle to the instantiated mutex object

--*/
HANDLE MutexRemoteToLocal(HREMOTEOBJSTRUCT* remote_handle_data)
{
    HANDLE hMutex = 
        MutexAddObjectToHandleManager(remote_handle_data->ShmKernelObject);
    if( NULL == hMutex )
    {
        hMutex = INVALID_HANDLE_VALUE;
    }
    return hMutex;
}

/*++
Function :
    MutexCloseHandle

    Closes the mutex handle.

parameters, return value : see definition of CLOSEHANDLEFUNC in handle.h

--*/
static INT MutexCloseHandle( HOBJSTRUCT * pHandleData )
{
    PMUTEX_HANDLE_OBJECT pHandleObject = (PMUTEX_HANDLE_OBJECT)pHandleData;

    TRACE( "Entered.\n" );
    if ( MutexIsValidMutexHandle( pHandleData ) )
    {
        PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject = 0;
        
        SYNCEnterCriticalSection( &mutex_critsec , TRUE);
        (pHandleObject->Ref_Count)--;

        SHMLock();

        pKernelObject = 
            (PGLOBAL_MUTEX_SYSTEM_OBJECT) SHMPTR_TO_PTR( pHandleObject->ShmKernelObject );
                

        if(NULL == pKernelObject)
        {
            ASSERT("Shared memory pointer %08x is invalid!\n",
                  pHandleObject->ShmKernelObject );
            SHMRelease();
            return -2;
        }

        TRACE( "Mutex count is %d.\n", pKernelObject->Mutex_Count );

        /* If there are no more references to the handle, 
        then get rid of the handle. */
        if ( pHandleObject->Ref_Count == 0)
        {
            TRACE("Releasing last reference to mutex in this process : "
                  "removing mutex object from process\n");

            /* note : it is not necessary to release the mutex before closing 
               it, Windows doesn't. */
            (pKernelObject->Ref_Count)--;

            if ( pKernelObject->Ref_Count == 0 )
            {
                TRACE("Releasing last mutex reference system-wide : \n"
                      "removing mutex from shared memory\n");

                if ( pKernelObject->Mutex_Count > 0 )
                {
                    WARN( "There are no more references to the mutex object "
                          "but the mutex count is not 0.\n" );
                }
                MutexDeleteSharedObject(pKernelObject->ShmHeader.ShmSelf);
            }

            TRACE( "Closing and removing the mutex object %p from the list.\n",
                   pHandleObject );

            /* Patch the list. */
            if ( pHandleObject->pPrev )
            {
                pHandleObject->pPrev->pNext = pHandleObject->pNext;
            }
            if ( pHandleObject->pNext )
            {
                pHandleObject->pNext->pPrev = pHandleObject->pPrev;
            }
            if ( pHandleData == (HOBJSTRUCT *)pMutexHandle )
            {
                TRACE( "Removing the first element of the list.\n" );
                pMutexHandle = pHandleObject->pNext;
            }
            free( pHandleObject );
            pHandleObject = NULL;
        }
        SHMRelease();
        SYNCLeaveCriticalSection( &mutex_critsec , TRUE);

#if defined _DEBUG
        MutexDisplaySharedMemory();
        MutexDisplayMapHandleObjects();
#endif
        TRACE( "MutexCloseHandle is exiting.\n" );
        return 0;
    }
    else
    {
        ASSERT( "pHandleData %p is not a mutex object. \n", pHandleData );
        return -1;
    }
}


/*++
Function :

    MutexAddObjectToSystem
    
    Adds a new kernel level mutex object, to the shared memory list.
    
    Returns a SHMPTR to the kernel object, or 0 on failure.
--*/
static SHMPTR MutexAddObjectToSystem( LPCWSTR lpName,
                                      BOOL bInitialOwner )
{
    SHMPTR ShmNewKernelObject = 0;
    PGLOBAL_MUTEX_SYSTEM_OBJECT pNewKernelObject = NULL;
    
    TRACE( "Entered.\n" );

    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    SHMLock();

    ShmNewKernelObject = SHMalloc( sizeof( GLOBAL_MUTEX_SYSTEM_OBJECT ) );

    if ( ShmNewKernelObject )
    {
        pNewKernelObject = 
            (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( ShmNewKernelObject );

        if ( pNewKernelObject )
        {
            /* Fill default values. */
            if ( bInitialOwner )
            {
                /* Current process/thread is the owner. */
                pNewKernelObject->Owner.ProcessId = GetCurrentProcessId();
                pNewKernelObject->Owner.ThreadId = GetCurrentThreadId();
                pNewKernelObject->Mutex_Count = 1;
            }
            else
            {
                /* No owner. */
                pNewKernelObject->Owner.ProcessId = 0;
                pNewKernelObject->Owner.ThreadId = 0;
                pNewKernelObject->Mutex_Count = 0;
            }

            pNewKernelObject->ShmWaitingForThreadList = 0;
            pNewKernelObject->Ref_Count = 0;
            pNewKernelObject->ShmHeader.ObjectType = SHM_NAMED_MUTEXS;
            pNewKernelObject->abandoned = FALSE;
            pNewKernelObject->ShmHeader.ShmSelf = ShmNewKernelObject;
            
            if ( lpName )
            {
                pNewKernelObject->ShmHeader.ShmObjectName = SHMWStrDup( lpName );
                if (0 != pNewKernelObject->ShmHeader.ShmObjectName)
                {
                    SHMAddNamedObject( ShmNewKernelObject );
                }
                else
                {
                    ERROR( "Unable to allocate shared memory for object name.\n" );
                    SHMfree( ShmNewKernelObject );
                    ShmNewKernelObject = 0;
                }
            }
            else
            {
                pNewKernelObject->ShmHeader.ShmObjectName = 0;
            }
        }
        else
        {
            ASSERT( "An error occurred while converting the SHMPTR to a"
                   " real useable pointer. " );
            SHMfree( ShmNewKernelObject );
            ShmNewKernelObject = 0;
        }
    }
    else
    {            
        ERROR( "Unable to allocate any shared memory.\n" );
    }

    SHMRelease();
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
    
    return ShmNewKernelObject;
}
/*++ 
Function :

    MutexAddObjectToHandleManager
    
    Adds a new object to the handle manager.
     
    Returns the handle to the newly inserted object., NULL on failure
--*/
static HANDLE MutexAddObjectToHandleManager( SHMPTR shmObject)
                                              
{
    PMUTEX_HANDLE_OBJECT pMutexHandleObject = NULL;
    HANDLE hRetVal = NULL;

    TRACE( "Entered.\n" );
    
    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    pMutexHandleObject = 
        (PMUTEX_HANDLE_OBJECT)malloc( sizeof( MUTEX_HANDLE_OBJECT ) );

    if ( pMutexHandleObject )
    {        
        pMutexHandleObject->HandleHeader.close_handle = MutexCloseHandle;
        pMutexHandleObject->HandleHeader.dup_handle = MutexDuplicateHandle;
        pMutexHandleObject->HandleHeader.type = HOBJ_MUTEX;
        
        pMutexHandleObject->Ref_Count = 1;
        pMutexHandleObject->ShmKernelObject = shmObject;
        
        hRetVal = HMGRGetHandle( (HOBJSTRUCT*)pMutexHandleObject );
        
        if(hRetVal == INVALID_HANDLE_VALUE)
        {
            WARN("couldn't get a handle for mutex object\n");
            /* we return NULL, not INVALID_HANDLE_VALUE */
            hRetVal = NULL;
            free(pMutexHandleObject);
            goto done;
        }            

        /* Add to start of the list. */
        pMutexHandleObject->pNext = pMutexHandle;
        pMutexHandleObject->pPrev = NULL;
        if(pMutexHandleObject->pNext)
        {
            pMutexHandleObject->pNext->pPrev = pMutexHandleObject;
        }
        pMutexHandle = pMutexHandleObject;
    }
    else
    {
        ERROR( "Unable to allocate memory for the handle object.\n" );
    }
done:
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
    return hRetVal;
}
/*++
Function :
    
    OpenMutexW
    
        Opens a named mutex, given the mangled name.  The mutex_critsec
        must be locked by the caller.
    
--*/
HANDLE OpenMutexW( LPCWSTR lpName )
{
    HANDLE hRetVal = NULL;
    SHMPTR shmObject = 0;
    BOOL nameExists;

    SHMLock();

    // See if the name is already taken.
    shmObject = SHMFindNamedObjectByName( lpName, SHM_NAMED_MUTEXS,
                                          &nameExists );
    if ( shmObject == 0 )
    {
        if (nameExists)
        {
            SetLastError(ERROR_INVALID_HANDLE);
        }
        else
        {
            SetLastError(ERROR_FILE_NOT_FOUND);
        }
        goto ExitOpenMutex;
    }
    else
    {
        PGLOBAL_MUTEX_SYSTEM_OBJECT pMutexObject = NULL;

        /* Every parameter but the name is ignored. So all we do is clone the 
        handle and return it.  We don't have to worry about changing the access 
        rights or anything. */
        hRetVal = MutexAddObjectToHandleManager( shmObject );        

        if ( hRetVal == NULL )
        {
            ERROR( "An error occured while duplicating the mutex's handle.\n" );
            SetLastError(ERROR_INVALID_HANDLE);
            goto ExitOpenMutex;
        }
        else
        {
            /* Increment the mutex's reference count */
            pMutexObject = (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( shmObject );
            (pMutexObject->Ref_Count)++;
        }
    }

ExitOpenMutex:
    SHMRelease();

    TRACE( "Exiting OpenMutex. Returning %p\n", hRetVal );
    return hRetVal;
}

/*++
Function:
  CreateMutexA

Note:
  Mutexes could be used for cross-process synchroniztion.
  Security Attributes could be ignored.
  lpName could be non NULL.

See MSDN doc.
--*/

HANDLE
PALAPI
CreateMutexA (
    IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
    IN BOOL bInitialOwner,
    IN LPCSTR lpName)
{
    HANDLE hRetVal = NULL;
    LPWSTR lpNameW = NULL;
    PERF_ENTRY(CreateMutexA);
    ENTRY("CreateMutexA(lpMutexAttr=%p, bInitialOwner=%d, lpName=%p (%s)\n",
          lpMutexAttributes, bInitialOwner, lpName?lpName:"", lpName?lpName:"");
    
    if (lpName) {
        int n = MultiByteToWideChar (CP_ACP, 0, lpName, -1, NULL, 0);
        if (!n) {
            ASSERT ("MultiByteToWideChar failed!\n");
            SetLastError (ERROR_INTERNAL_ERROR);
            goto done;
        }
        lpNameW = (LPWSTR)malloc (sizeof (WCHAR) * n);
        if (!lpNameW) {
            ERROR ("malloc() failed\n");
            SetLastError (ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
        MultiByteToWideChar (CP_ACP, 0, lpName, -1, lpNameW, n);
    }

    hRetVal = CreateMutexW (lpMutexAttributes, bInitialOwner, lpNameW);

done:
    free (lpNameW);
    
    LOGEXIT( "CreateMutexA returns HANDLE %p\n", hRetVal );
    PERF_EXIT(CreateMutexA);
    return hRetVal;
}

/*++
Function:
  CreateMutexW

Note:
  Mutexes could be used for cross-process synchroniztion.
  Security Attributes could be ignored.
  lpName could be non NULL.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateMutexW(
    IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
    IN BOOL bInitialOwner,
    IN LPCWSTR lpName)
{
    HANDLE hRetVal = NULL;
    SHMPTR ShmNewObject = 0;
    LPCWSTR lpObjectName = NULL;
    WCHAR MangledObjectName[MAX_PATH];

    PERF_ENTRY(CreateMutexW);
    ENTRY("CreateMutexW(lpMutexAttr=%p, bInitialOwner=%d, lpName=%p (%S)\n",
          lpMutexAttributes, bInitialOwner, lpName?lpName:W16_NULLSTRING, lpName?lpName:W16_NULLSTRING);

    lpObjectName=lpName;
    if (!MangleObjectNameW(&lpObjectName, MangledObjectName))
    {
        LOGEXIT( "CreateMutexW returns HANDLE %p\n", NULL );
        PERF_EXIT(CreateMutexW);
        return NULL;
    }

    if ( lpMutexAttributes != NULL )
    {
        WARN( "Ignoring lpMutexAttributes.\n" );
    }
    
    SYNCEnterCriticalSection( &mutex_critsec , TRUE);

    /* See if another named object has this name already. */
    if(lpName)
    {
        DWORD old_last_error;

        old_last_error = GetLastError();

        hRetVal = OpenMutexW( lpObjectName );
        if( NULL == hRetVal )
        {
            /* OpenMutex failed, let's see why */
            switch(GetLastError())
            {
            case ERROR_INVALID_HANDLE:
                /* OpenMutexW sets this if it found an object with the 
                   specified name, but it wasn't a mutex */
                WARN("mutex name is already in use by non-mutex object\n");
                goto ExitCreateMutex;
            case ERROR_FILE_NOT_FOUND:
                /* OpenMutexW sets this if it did not find any object with 
                   the specified name */
                TRACE("mutex name is not in use\n");
                break;
            case ERROR_INVALID_PARAMETER:
                ASSERT("OpenMutexW complains of an invalid parameter, this "
                       "shouldn't have happened.\n");
                SetLastError(ERROR_INTERNAL_ERROR);
                goto ExitCreateMutex;
            default:
                /* OpenMutexW shouldn't set any other error codes */
                ASSERT("OpenMutexW() failed, LastError has unexpected value "
                       "%#x\n", GetLastError());
                SetLastError(ERROR_INTERNAL_ERROR);
                goto ExitCreateMutex;
            }
            SetLastError(old_last_error);
        }
        else
        {
            /* OpenMutex succeeded :we're done */
            SetLastError(ERROR_ALREADY_EXISTS);
            goto ExitCreateMutex;
        }                                      
    }
    else
    {
        hRetVal = NULL;
    }

    /* Create the kernel object for the new mutex. */
    ShmNewObject = MutexAddObjectToSystem( lpObjectName, bInitialOwner );

    if ( ShmNewObject )
    {
        PGLOBAL_MUTEX_SYSTEM_OBJECT pMutexObject = NULL;
        hRetVal = MutexAddObjectToHandleManager( ShmNewObject );
        if(NULL == hRetVal)
        {
            ASSERT( "Unable to add the mutex object to the handle manager.\n" );
            MutexDeleteSharedObject(ShmNewObject);
            ShmNewObject = 0;
            SetLastError( ERROR_INTERNAL_ERROR );
        }
        else
        {
            /* Increment the mutex's reference count */
            SHMLock();
            pMutexObject = 
                (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( ShmNewObject );
            (pMutexObject->Ref_Count)++;
            SHMRelease();
        }
    }
    else
    {
        ASSERT( "Unable to add the mutex object to the system.\n" );
        SetLastError( ERROR_INTERNAL_ERROR );
    }

ExitCreateMutex:
    
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);
    
#if defined _DEBUG
    MutexDisplaySharedMemory();
    MutexDisplayMapHandleObjects();
#endif

    LOGEXIT( "CreateMutexW returns HANDLE %p\n", hRetVal );
    PERF_EXIT(CreateMutexW);
    return hRetVal;
}

/*++
Function:
  ReleaseMutex

See MSDN doc.
--*/
BOOL
PALAPI
ReleaseMutex( IN HANDLE hMutex )
{
    BOOL retval;
    MUTEX_HANDLE_OBJECT *pMutex;

    PERF_ENTRY(ReleaseMutex);
    ENTRY("ReleaseMutex(hMutex=%p)\n", hMutex);

    pMutex = (PMUTEX_HANDLE_OBJECT)HMGRLockHandle2( hMutex, HOBJ_MUTEX );

    if ( NULL==pMutex )
    {
        ERROR( "The handle %p is not a valid mutex handle.\n", hMutex );
        SetLastError( ERROR_INVALID_HANDLE );
        retval = FALSE;
    }
    else
    {
        retval = MutexReleaseMutex(pMutex,WUTC_SIGNALED, FALSE);
        HMGRUnlockHandle(hMutex,&pMutex->HandleHeader);
    }

    LOGEXIT("ReleaseMutex returns BOOL %d\n",retval);
    PERF_EXIT(ReleaseMutex);
    return retval;
}


/*++
Function:
  MutexReleaseMutex

    "Real" implementation of ReleaseMutex, with extra parameter to distinguish 
    between "real" releases and releases caused by ExitThread
                  
Paramaters:
    MUTEX_HANDLE_OBJECT *pMutex : pointer to mutex object to release
    WAKEUPTHREAD_CODE WakeUpCode: code to pass to WakeUpThread, either 
                                  WUTC_SIGNALED or WUTC_ABANDONED
    BOOL process_abandon : if TRUE, the entire process must abandon the mutex; 
                           threads of the current process will be skipped when 
                           looking for a new owner
    
Return value : same as ReleaseMutex    

--*/
BOOL
MutexReleaseMutex( MUTEX_HANDLE_OBJECT *pMutex, WAKEUPTHREAD_CODE WakeUpCode,
                   BOOL process_abandon )
{
    PGLOBAL_MUTEX_SYSTEM_OBJECT pKernelObject = NULL;
    BOOL bRetVal = FALSE;    

    TRACE("releasing mutex %p; wakeup code is %d\n", pMutex, WakeUpCode);

    SYNCEnterCriticalSection( &mutex_critsec , TRUE);
    SHMLock();
    
    pKernelObject = 
        (PGLOBAL_MUTEX_SYSTEM_OBJECT)SHMPTR_TO_PTR( 
            pMutex->ShmKernelObject );

    if ( !pKernelObject )
    {
        ASSERT( "An error occured while attempting to retrieve the mutex from "
               "shared memory.\n" );
        SetLastError( ERROR_INTERNAL_ERROR );
        bRetVal = FALSE;
        goto ExitReleaseMutex;
    }

    if ( pKernelObject->Owner.ProcessId == GetCurrentProcessId() &&
         (pKernelObject->Owner.ThreadId  == GetCurrentThreadId() || 
          process_abandon) )
    {
        (pKernelObject->Mutex_Count)--;

        if ( pKernelObject->Mutex_Count == 0 )
        {
            ThreadWaitingList * pThreadWaitingList = NULL;
            ThreadWaitingList *pPrevThread;
            DWORD *pAwakenState;
            SHMPTR shmWaitingThread;

            pKernelObject->Owner.ProcessId = 0;
            pKernelObject->Owner.ThreadId = 0;
            pKernelObject->Mutex_Count = 0;

            /* find a thread to wake up */
            pPrevThread = NULL;
            shmWaitingThread = pKernelObject->ShmWaitingForThreadList;
            while(0!=shmWaitingThread)
            {
                pThreadWaitingList = SHMPTR_TO_PTR(shmWaitingThread);
                pAwakenState = SHMPTR_TO_PTR(pThreadWaitingList->state.shmAwakened);
                
                if(!THREADInterlockedAwaken(pAwakenState, FALSE))
                {
                    TRACE("thread is already awake, skipping it\n");
                    pPrevThread = pThreadWaitingList;
                    shmWaitingThread = pThreadWaitingList->ptr.shmNext;
                    continue;
                }

                if(process_abandon && 
                        pThreadWaitingList->processId == GetCurrentProcessId())
                {
                    SHMPTR shmTemp;

                    TRACE("thread is in the current process; removing it from "
                          "list, skipping it\n");

                    /* remove thread from the waiting list, we don't want other 
                       processes to give ownership back to us */
                    shmTemp = shmWaitingThread;
                    if(NULL == pPrevThread)
                    {
                        pKernelObject->ShmWaitingForThreadList = 
                            pThreadWaitingList->ptr.shmNext;
                    }
                    else
                    {
                        pPrevThread->ptr.shmNext = 
                            pThreadWaitingList->ptr.shmNext;
                    }
                    shmWaitingThread = pThreadWaitingList->ptr.shmNext;
                    SHMfree(shmTemp);
                }
                else
                {
                    break;
                }   
            }
            if(0!=shmWaitingThread)
            {
                /* Reset the owner, and mutex count information. */
                pKernelObject->Owner.ProcessId = pThreadWaitingList->processId;
                pKernelObject->Owner.ThreadId = pThreadWaitingList->threadId;
                pKernelObject->Mutex_Count = 1;
                /* mutex ownership is being transferred, mutex isn't abandoned 
                   anymore */
                pKernelObject->abandoned = FALSE;
        
                /* remove thread from the waiting list */
                if(NULL == pPrevThread)
                {
                    pKernelObject->ShmWaitingForThreadList = 
                        pThreadWaitingList->ptr.shmNext;
                }
                else
                {
                    pPrevThread->ptr.shmNext = pThreadWaitingList->ptr.shmNext;
                }
        
                WakeUpThread( pThreadWaitingList->threadId, 
                              pThreadWaitingList->processId,
                              pThreadWaitingList->blockingPipe,
                              WakeUpCode );
                SHMfree(shmWaitingThread);
            }
            else
            {
                /* No waiting threads. */
                TRACE( "No waiting threads.\n" );
            }
        }
        bRetVal = TRUE;
    }
    else
    {
        ERROR( "The current thread does NOT own the mutex.\n" );
        SetLastError( ERROR_NOT_OWNER );
        bRetVal = FALSE;
    }

ExitReleaseMutex:
    SHMRelease();
    SYNCLeaveCriticalSection( &mutex_critsec , TRUE);

#if defined _DEBUG
    MutexDisplaySharedMemory();
    MutexDisplayMapHandleObjects();
#endif

    return bRetVal;
}

/*++
Function:
  MutexAbandonMutexes

    Releases all mutexes held by the current thread, indicating that they were 
    abandoned by a terminating thread 
                  
(no parameters, no return value)
--*/
void MutexAbandonMutexes(void)
{
    DWORD proc_id;
    DWORD thread_id;
    MUTEX_HANDLE_OBJECT *mutex_list;
    GLOBAL_MUTEX_SYSTEM_OBJECT *shm_mutex;

    TRACE("now releasing all mutexes held by this thread\n");
    proc_id = GetCurrentProcessId();
    thread_id = GetCurrentThreadId();

    SYNCEnterCriticalSection(&mutex_critsec, TRUE);
    SHMLock();

    mutex_list = pMutexHandle;
    while(mutex_list)
    {
        shm_mutex = SHMPTR_TO_PTR(mutex_list->ShmKernelObject);
        if(NULL == shm_mutex)
        {
            WARN("couldn't access shared memory info for mutex %#x\n", 
                 mutex_list->ShmKernelObject);
        }
        else if(shm_mutex->Owner.ProcessId == proc_id && 
           shm_mutex->Owner.ThreadId == thread_id)
        {
            TRACE("abandoning mutex object %p\n",mutex_list);
            shm_mutex->abandoned = TRUE;
            while(0<shm_mutex->Mutex_Count && 
                  shm_mutex->Owner.ProcessId == proc_id && 
                  shm_mutex->Owner.ThreadId == thread_id)
            {
                MutexReleaseMutex(mutex_list,WUTC_ABANDONED, FALSE);
            }
        }   
        mutex_list = mutex_list->pNext;
    }
    SHMRelease();
    SYNCLeaveCriticalSection(&mutex_critsec, TRUE);
}

/*++
Function:
  MutexAbandonAllMutexes

    Releases all mutexes held by threads in the current process, indicating 
    that they were abandoned by a terminating thread 
                  
(no parameters, no return value)
--*/
void MutexAbandonAllMutexes(void)
{
    DWORD proc_id;
    MUTEX_HANDLE_OBJECT *mutex_list;
    GLOBAL_MUTEX_SYSTEM_OBJECT *shm_mutex;

    TRACE("now releasing all mutexes held by this process\n");
    proc_id = GetCurrentProcessId();

    /* note : this isn't entirely necessary,all other threads should be 
       suspended at this point */
    SYNCEnterCriticalSection(&mutex_critsec, TRUE);

    SHMLock();

    mutex_list = pMutexHandle;
    while(mutex_list)
    {
        shm_mutex = SHMPTR_TO_PTR(mutex_list->ShmKernelObject);
        if(NULL == shm_mutex)
        {
            WARN("couldn't access shared memory info for mutex %#x\n", mutex_list->ShmKernelObject);
        }
        else if(shm_mutex->Owner.ProcessId == proc_id)
        {
            TRACE("abandoning mutex object %p\n",mutex_list);
            shm_mutex->abandoned = TRUE;
            while(0<shm_mutex->Mutex_Count && 
                  shm_mutex->Owner.ProcessId == proc_id)
            {
                MutexReleaseMutex(mutex_list,WUTC_ABANDONED, TRUE);
            }
        }   
        mutex_list = mutex_list->pNext;
    }
    SHMRelease();
    SYNCLeaveCriticalSection(&mutex_critsec, TRUE);
}

/*++
Function:
  MutexDeleteSharedObject

    Release the shared memory pointer associated with a mutex object, along 
    with all pointers it contains
                  
Parameters:
    SHMPTR shmPtr : shared memory object to release
                                                                   
(no return value)
--*/
static void MutexDeleteSharedObject(SHMPTR shmPtr)
{
    PGLOBAL_MUTEX_SYSTEM_OBJECT pShmMutex;
    SHMPTR shmWaitingList;
    SHMPTR temp;
    ThreadWaitingList* waitThList;

    pShmMutex = SHMPTR_TO_PTR(shmPtr);
    if(NULL == pShmMutex)
    {
        ASSERT("mutex SHMPTR %#x is invalid!\n", shmPtr);
        return;
    }
    if(pShmMutex->ShmHeader.ShmObjectName)
    {
        SHMRemoveNamedObject(shmPtr);
        SHMfree(pShmMutex->ShmHeader.ShmObjectName);
    }

    /* The thread list _should_ be empty, but just in case it is not
    we should delete any entries. */
    shmWaitingList = pShmMutex->ShmWaitingForThreadList;

    while ( shmWaitingList != 0 )
    {
        temp = shmWaitingList;
        waitThList = SHMPTR_TO_PTR(shmWaitingList);
        if (waitThList == NULL)
        {
            ASSERT("Got invalid SHMPTR value ...\n");
            break;
        }
        shmWaitingList = waitThList->ptr.shmNext;
        SHMfree( temp );
    }
    SHMfree( shmPtr );
}

/* Basic spinlock implementation */
void SPINLOCKAcquire (volatile LONG * lock, unsigned int flags)
{
    int loop_seed = 1, loop_count = 0;

    if (flags & SYNCSPINLOCK_F_ASYMMETRIC)
    {
        loop_seed = ((int)pthread_self() % 10) + 1;
    }
    while (InterlockedCompareExchange(lock, 1, 0))
    {
        if (!(flags & SYNCSPINLOCK_F_ASYMMETRIC) || (++loop_count % loop_seed))
            sched_yield();
    }
}

void SPINLOCKRelease (volatile LONG * lock)
{
    *lock = 0;
}
