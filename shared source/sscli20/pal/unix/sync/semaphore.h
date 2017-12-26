/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    sync/semaphore.h

Abstract:

    Semaphore object structure definition.

--*/

#ifndef _PAL_SEMAPHORE_H_
#define _PAL_SEMAPHORE_H_

#include "pal/palinternal.h"
#include "pal/handle.h"
#include "pal/thread.h"

typedef struct _Semaphore
{
    HOBJSTRUCT   objHeader;

    INT          refCount;

    LONG         semCount;
    LONG         maximumCount;

    CRITICAL_SECTION critSection;
    ThreadWaitingList  *waitingThreads;
} Semaphore;


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
    -1: an error occurred, SetLastError is called in this function.
    0: the semaphore is signaled.
    1: if the semaphore is not signaled.
--*/
int SemaphoreWaitOn(IN HANDLE hSemaphore, SHMPTR wait_state);

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
int SemaphoreRemoveWaitingThread(IN HANDLE hSemaphore);


#endif //PAL_SEMAPHORE_H_










