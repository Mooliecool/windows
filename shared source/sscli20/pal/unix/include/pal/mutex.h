/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/mutex.h

Abstract:
    
    Header file for the mutex functions.
    
--*/

#ifndef _PAL_MUTEX_H_
#define _PAL_MUTEX_H_

#include "pal/handle.h"
#include "pal/shmemory.h"
#include "pal/thread.h"

/*++
Function :
    MutexInitialize

    Initialize the critical sections.

Return value:
    TRUE  if initialization succeeded
    FALSE otherwise        
--*/
BOOL MutexInitialize( void );

/*++
Function :
    MutexCleanup

    Deletes the critical sections.

--*/
void MutexCleanup( void );

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
    -1: an error occurred, SetLastError is called in this function.
    0: the mutex is signaled.
    1: if the mutex is not signaled.
    2: the mutex is not signaled, but was abandoned by the thread that was 
       holding it
--*/

INT MutexWaitOn( IN HANDLE hMutex, SHMPTR wait_state );

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

int MutexRemoveWaitingThread( IN HANDLE hMutex );

typedef struct _MHO 
{
    HOBJSTRUCT  HandleHeader;

    /* 'Private' mutex specific data. */
    UINT            Ref_Count;
    SHMPTR          ShmKernelObject;    /* Shared memory pointer */
                                        /* to the kernel object. */
    struct _MHO *   pNext;
    struct _MHO *   pPrev;

}MUTEX_HANDLE_OBJECT, * PMUTEX_HANDLE_OBJECT;


/* Global Data.
This structure contains the data needed to control access to the mutex.
*/
typedef struct _GMSO 
{
    SHM_NAMED_OBJECTS   ShmHeader;
    
    SHMPTR              ShmWaitingForThreadList;  /* List of waiting threads.*/    
    UINT                Ref_Count;
    UINT                Mutex_Count;            
    BOOL                abandoned;

    struct
    {
        DWORD ProcessId;
        DWORD ThreadId;
    }Owner;

}GLOBAL_MUTEX_SYSTEM_OBJECT, * PGLOBAL_MUTEX_SYSTEM_OBJECT;


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
                   BOOL process_abandon );

/*++
Function:
  MutexAbandonMutexes

    Releases all mutexes held by the current thread, indicating that they were 
    abandoned by a terminating thread 
                  
(no parameters, no return value)
--*/
void MutexAbandonMutexes(void);


/*++
Function:
  MutexAbandonAllMutexes

    Releases all mutexes held by threads in the current process, indicating 
    that they were abandoned by a terminating thread 
                  
(no parameters, no return value)
--*/
void MutexAbandonAllMutexes(void);


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
                        HREMOTEOBJSTRUCT *remote_handle_data);


/*++
Function :
    MutexLocalToRemote

    Creates a mutex handle using a Remote Handle from another process

Parameters, 
    IN remote_handle_data  : structure associated with the remote handle
    
Returns
    Handle to the instantiated mutex object

--*/
HANDLE MutexRemoteToLocal(HREMOTEOBJSTRUCT *remote_handle_data);

#define SYNCSPINLOCK_F_ASYMMETRIC  1

#define SPINLOCKInit(lock) (*(lock) = 0)
#define SPINLOCKDestroy SPINLOCKInit

void SPINLOCKAcquire (volatile LONG * lock, unsigned int flags);
void SPINLOCKRelease (volatile LONG * lock);

#endif /* _PAL_MUTEX_H_ */
