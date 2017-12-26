/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    sync/event.h

Abstract:

    Event object structure definition.

--*/

#ifndef _PAL_EVENT_H_
#define _PAL_EVENT_H_

#include "pal/palinternal.h"
#include "pal/handle.h"
#include "pal/thread.h"

typedef struct _Event
{
    HOBJSTRUCT   objHeader;

    SHMPTR       info;     /* shared mem pointer on EventInfo structure */

    INT          refCount;
} Event;

/* Global Data.
This structure contains the data needed to control access to the event in shared
memory.
*/
typedef struct _GESO 
{
    SHM_NAMED_OBJECTS   ShmHeader;
    
    INT          refCount;
    BOOL         state;
    BOOL         manualReset;
    SHMPTR       waitingThreads;    
    SHMPTR       next;

}GLOBAL_EVENT_SYSTEM_OBJECT, * PGLOBAL_EVENT_SYSTEM_OBJECT;

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
	-1: an error occurred, SetLastError is called in this function.
	0: the event is not signaled.
    1: if the event is signaled.
--*/
int EventWaitOn(IN HANDLE hEvent, SHMPTR wait_state);

/*++
Function:
  EventRemoveWaitingThread

  Remove the CurrentThreadId from the list of waiting thread. This
  function is called when the current thread stop waiting on an Event
  for a different reason than the Event was signaled. (e.g. a timeout,
  or the thread was waiting on multiple objects and another object was
  signaled)

Parameters:
    IN hEvent:   event handle of the modify thread waiting list.    

returns
	-1: an error occurred, SetLastError is called in this function.
	0: if the thread wasn't found in the list.
    1: if the thread was removed from the waiting list.
--*/
int EventRemoveWaitingThread(IN HANDLE hEvent);


#endif //PAL_EVENT_H_










