/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/thread.h

Abstract:
    Header file for threads & processes utility functions.

--*/

#ifndef _PAL_THREAD_H_
#define _PAL_THREAD_H_

#include "pal/palinternal.h"
#include "pal/handle.h"
#include "pal/module.h"
#include "pal/virtual.h"
#include "pal/shmemory.h"

#include <pthread.h>

// optimize TLS access through TLS lookaside cache
#define USE_TLSLOOKASIDECACHE

#if __APPLE__

#if defined(USE_TLSLOOKASIDECACHE)
// optimize TLS access through optimized TLS getters - requires USE_TLSLOOKASIDECACHE
#define USE_OPTIMIZEDTLSGETTER
#endif

#endif


typedef struct _ThreadWaitingList {
    DWORD threadId;
    DWORD processId;
    int   blockingPipe;
    union
    {
        SHMPTR shmNext;
        struct _ThreadWaitingList *Next;
    }ptr;
    union
    {
        SHMPTR shmAwakened;
        LPBOOL pAwakened;
    }state;
} ThreadWaitingList;


/* WaitForMultipleObjects wakeup codes */
typedef enum
{
    WUTC_NONE,
    WUTC_SIGNALED,
    WUTC_ABANDONED,
    WUTC_APC_QUEUED,
    
    WUTC_LAST
} WAKEUPTHREAD_CODE;

/* return codes for *WaitOn functions */
typedef enum 
{
    WOC_ERROR,      /* internal error occurred */
    WOC_WAITING,    /* object was not signaled; thread added to waiting list */
    WOC_SIGNALED,   /* object was signaled; thread not added to list, object 
                       is now unsignaled (if aplicable)*/
    WOC_ABANDONED,  /* object was an unsignaled mutex whose owner thread has 
                       terminated. mutex is acquired, thread not added to 
                       waiting list */
    WOC_INTERUPTED, /* another object has already written a code to the pipe to 
                       wake up the thread */

    WOC_LAST        /* dummy value for validation */
} WAITON_CODE;

typedef enum
{
    TWS_ACTIVE,
    TWS_WAITING,
    TWS_ALERTABLE,
    TWS_EARLYDEATH,

    TWS_NONE
} THREAD_WAIT_STATE;




/* thread ID of thread that has initiated an ExitProcess (or TerminateProcess). 
   this is to make sure only one thread cleans up the PAL, and also to prevent 
   calls to CreateThread from succeeding once shutdown has started 
   [defined in process.c]
*/
extern volatile DWORD terminator;
extern LPWSTR pAppDir;

// The process ID of this process, so we can avoid excessive calls to getpid().
extern DWORD gPID;

/*++
Function:
  PROCGetRealCurrentProcess

This returns the real process handle, and not a pseudo handle like
GetCurrentProcess do.
--*/
HANDLE PROCGetRealCurrentProcess(VOID);

/*++
Function:
  PROCGetProcessIDFromHandle

Abstract
  Return the process ID from a process handle
--*/
DWORD PROCGetProcessIDFromHandle(HANDLE hProcess);

/*++
Function:
  PROCCreateInitialProcess

Abstract
  Initialize all the structures for the initial process.

Parameter
  lpwstrCmdLine:   Command line.
  lpwstrFullPath : Full path to executable

Return
  TRUE: if successful
  FALSE: otherwise

Notes :
    This function takes ownership of lpwstrCmdLine, but not of lpwstrFullPath
--*/
BOOL  PROCCreateInitialProcess(LPWSTR lpwstrCmdLine, LPWSTR lpwstrFullPath);

/*++
Function:
  PROCCleanupInitialProcess

Abstract
  Cleanup all the structures for the initial process.

Parameter
  VOID

Return
  VOID

--*/
VOID PROCCleanupInitialProcess(VOID);

/*++
Function:
  PROCCondemnOtherThreads

  Set the waiting state of other threads to TWS_EARLYDEATH; this will prevent 
  signaled objects from trying to wake up other threads during process termination

(no parameters, no return value)
--*/
void PROCCondemnOtherThreads(void);

/*++
Function:
  PROCSuspendOtherThreads

  Calls SuspendThread on all threads in the process, except the current 
  thread. Used by PAL_Terminate.

(no parameters, no return value)
--*/
void PROCSuspendOtherThreads(void);

/*++
Macro:
  THREADSilentGetCurrentThreadId

Abstract:
  Same as GetCurrentThreadId, but it doesn't output any traces.
  It is useful for tracing functions to display the thread ID
  without generating any new traces.

--*/
#define THREADSilentGetCurrentThreadId() (DWORD) pthread_self()

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
BOOL TlsInternalGetValue(DWORD dwTlsIndex, LPVOID *pValue);

/*++
Function:
  THREADGetThreadProcessId

returns process owner ID of the indicated hThread
--*/
DWORD THREADGetThreadProcessId(HANDLE hThread);



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
BOOL THREADGetThreadPipeName(LPSTR dest, int dest_size, DWORD pid, DWORD tid);


typedef struct _APC_TRACKER
{
    PAPCFUNC            pfnAPC;
    ULONG_PTR           pAPCData;
    struct _APC_TRACKER *pNext;
} APC_TRACKER, *LPAPC_TRACKER;


typedef enum
{
    TS_STARTING,
    TS_RUNNING,
    TS_FAILED,
    TS_DONE,
    TS_DUMMY
} THREAD_STATE;

/* This is the number of slots available for use in TlsAlloc().
   sTlsSlotFields in thread/localstorage.c must be this number
   of bits. */
#define TLS_SLOT_SIZE   64

/* In the windows CRT there is a constant defined for the max width
of a _ecvt conversion. That constant is 348. 348 for the value, plus
the exponent value, decimal, and sign if required. */
#define ECVT_MAX_COUNT_SIZE 348
#define ECVT_MAX_BUFFER_SIZE 357

/*STR_TIME_SIZE is defined as 26 the size of the
  return val by ctime_r*/
#define STR_TIME_SIZE 26

#ifdef _DEBUG
#define MAX_TRACKED_CRITSECS 8
#endif

typedef struct _THREAD
{
    HOBJSTRUCT   objHeader;
    struct _THREAD* next;
    HANDLE       hThread;
    DWORD        dwThreadId; 
    DWORD        dwLwpId; 
    DWORD        dwExitCode;
    THREAD_STATE thread_state;
    int          blockingPipe;
    INT          refCount;
    LPTHREAD_START_ROUTINE  lpStartAddress;
    CRITICAL_SECTION thread_crit_section;
    LPVOID       lpStartParameter;
    LPAPC_TRACKER lpAPCTracker;
    int          threadPriority;
    ThreadWaitingList *waitingThreads;
    DWORD        dwSuspendCount;
    BOOL         SuspendedWithPipeFlag; // If TRUE, the thread is blocked
                                        // on a pipe read from a self-suspend.
    BOOL         bCreateSuspended;      // If TRUE, the thread is suspended on
                                        // creation.
    BOOL         isInternal;            // If TRUE, the thread is an internal
                                        // PAL thread.
    SHMPTR       waitAwakened;
    HANDLE       hBlockCreateThreadEvent;  /* This event is used to prevent 
                                              CreateThread() from returning 
                                              before important initialization
                                              in THREADEntry() is completed */
    LPVOID       minStack;              // estimate of stack boundaries - used by the TLS cache
    LPVOID       maxStack;
    LPVOID       tlsSlots[TLS_SLOT_SIZE];

    DWORD        lastError;           /* Last Error code */

    PPAL_EXCEPTION_REGISTRATION bottom_frame; // Exception handling info
    EXCEPTION_RECORD       current_exception;
    BOOL safe_state;
    int signal_code;

    ULONG critsec_count;
    BOOL suspend_intent;
    pthread_mutex_t suspension_mutex;
    pthread_mutex_t cond_mutex;
    pthread_cond_t suspender_cond;
    BOOL cond_predicate;
    BOOL final_suspension;
    DWORD suspend_spinlock;

    // random buffers for C runtime functions
    CHAR *       strtokContext; // Context for strtok function
    WCHAR *      wcstokContext; // Context for wcstok function
    struct PAL_tm localtimeBuffer; // Buffer for localtime function
    CHAR         ctimeBuffer[ STR_TIME_SIZE ]; // Buffer for ctime function
    CHAR         ECVTBuffer[ ECVT_MAX_BUFFER_SIZE ]; // Buffer for _ecvt function.
    struct PAL_hostent hostentBuffer;

#ifdef _DEBUG
    CRITICAL_SECTION * owned_internal_critsecs[MAX_TRACKED_CRITSECS];
#endif

} THREAD;


extern pthread_key_t thObjKey;
extern DWORD StartupLastError;

/* codes written to a thread's blocking pipe */
extern CONST DWORD WAKEUPCODE; /* used by ResumeThread*/
extern CONST DWORD GIVEUPCODE; /* used by closeDummyThreadHandle */

/*++
Function:
  CreateInternalThread

Abstract:
    An internal version of CreateThread for PAL threads.
--*/
HANDLE
PALAPI
CreateInternalThread(
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN DWORD dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId);

/*++
Function:
    THREADCreateInitialThread

Abstract:
    Create the initial thread object
--*/
THREAD *THREADCreateInitialThread(void);

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
int THREADCallThreadAPCs(void);

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
THREADCreateDummyThreadHandle(void);


/*++
Function:
  ThreadWaitOn

  Check if the thread has terminated. If not, place
  the current thread to the list of waiting thread. This function is
  called by WaitXXXX functions when it's time to wait on a Thread.
  The caller is responsible for blocking the thread, this function does
  not wait on the thread.

  This function sets the last error, if an error occured.

Parameters:
    IN hThread: handle of the thread to check for termination.
    SHMPTR wait_state : shared memory pointer to waiting thread's wait state

returns
    -1: an error occurred, SetLastError is called in this function.
    0: the thread has terminated.
    1: the thread is still running
--*/
int ThreadWaitOn(IN HANDLE hThread, SHMPTR wait_state);


/*++
Function:
  ThreadRemoveWaitingThread

  Remove the CurrentThreadId from the list of waiting thread. This
  function is called when the current thread stops waiting on a Thread
  for a different reason than the Thread having terminated. (e.g. a timeout,
  or the thread was waiting on multiple objects and another object was
  signaled)

Parameters:
    IN hThread:   thread handle whose thread waiting list is to be modified

returns
    -1: an error occurred, SetLastError is called in this function.
    0: if the thread wasn't found in the list.
    1: if the thread was removed from the waiting list.
--*/
int ThreadRemoveWaitingThread(IN HANDLE hThread);

/*++
Function:
    WakeUpThread

    Unblock a waiting thread

Parameters:
    DWORD ThreadId    Thread to wake up
    DWORD ProcessId   Process of the thread to wake up
    int   ThreadPipe  Blocking pipe associated with the thread to wake up.
    WAKEUPTHREAD_CODE WakeUpCode Code to use when waking up the thread

Return value:
    VOID

Notes :
    -This function lives in pal/Unix/sync/, not in pal/Unix/thread/
--*/
VOID
WakeUpThread( DWORD ThreadId, DWORD ProcessId,
              int ThreadPipe, WAKEUPTHREAD_CODE WakeUpCode );

/*++
Function:
  PROCProcessLock

Abstract
  Enter the critical section associated to the current process
--*/
VOID PROCProcessLock(VOID);


/*++
Function:
  PROCProcessUnlock

Abstract
  Leave the critical section associated to the current process
--*/
VOID PROCProcessUnlock(VOID);

/*++
Function:
    PROCSetExitCode

Abstract:
    Save the exit code of a process in its object's structure

Parameters:
    HANDE hProcess : handle of process to modify
    DWORD exit_code : exit code of process (waitpid)
    
Return value :
    TRUE on success
    
Notes :
    -The reason for this is that waitpid() only succeeds on a particular PID 
     the first time it is called, so we must save the exit code it returns for 
     future reference. (specifically, it is common to call WaitForSingleObject()
     on a process, followed by GetExitCodeProcess(); both would normally use 
     waitpid())
--*/
BOOL PROCSetProcessExitCode( HANDLE hProcess, DWORD exit_code);


/*++
Function:
  TerminateCurrentThread

Does any necessary memory clean up, signals waiting threads, and then forces 
the current thread to exit.

--*/
VOID
TerminateCurrentThread(IN DWORD dwExitCode);

/*++
Function:
  PROCCleanupProcess
  
  Do all cleanup work for TerminateProcess, but don't terminate the process.
  If bTerminateUnconditionally is TRUE, we exit as quickly as possible.

(no return value)
--*/
void PROCCleanupProcess(BOOL bTerminateUnconditionally);

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
BOOL THREADInterlockedAwaken(DWORD *pWaitState, BOOL alert_only);

/*++
Function :
    ProcessLocalToRemote

    Makes a Process handle accessible to another process using a Remote Handle

Parameters, 
    IN handle_data         : structure associated with this handle
    IN remote_handle_data  : structure associated with the remote handle

Return value : TRUE if functions succeeded, FALSE otherwise.

--*/
BOOL ProcessLocalToRemote(HOBJSTRUCT *handle_data, 
                          HREMOTEOBJSTRUCT *remote_handle_data);

/*++
Function :
    ProcessRemoteToLocal

    Creates a Process handle using a Remote Handle from another process

Parameters, 
    IN remote_handle_data : structure associated with the remote handle
    
Returns an handle to the instantiated event object

--*/
HANDLE ProcessRemoteToLocal(HREMOTEOBJSTRUCT *remote_handle_data);

/*++
Function:
  WFMOInitialize

  Initializes internal data structured needed for wait object support  

Return value:
    TRUE if initialization succeeded
    FALSE otherwise
--*/
BOOL WFMOInitialize(void);

/*++
Function:
  WFMOCleanup
  
  Terminate the WFMO worker thread (if it's active), and cleans up
  wait object support data structures

(no parameters, no return value)
--*/
void WFMOCleanup(void);
void WFMOPostCleanup(void);

/*++
Function:
  PROCGetCurrentThreadObject

This returns a pointer to the current THREAD object.

IMPORTANT : this must not lock any critical sections
--*/
#if defined(USE_TLSLOOKASIDECACHE)

typedef THREAD *PTHREAD;
typedef PTHREAD THREADHINTS[0x100];
extern volatile THREADHINTS thread_hints;

extern volatile ULONG flush_counter;

#if defined(USE_OPTIMIZEDTLSGETTER)

THREAD* PROCGetCurrentThreadObjectSlow(size_t sp, THREAD** ppThreadHint);

extern PAL_POPTIMIZEDTLSGETTER fnPROCGetCurrentThreadObject;
#define PROCGetCurrentThreadObject() ((*fnPROCGetCurrentThreadObject)())

// Magic index of optimized TLS getter for PAL THREAD object. Used to 
// ask TLSMakeOptimizedGetter to create an optimized getter for the internal PAL THREAD object.
#define THREAD_OBJECT_TLS_INDEX ((DWORD)-1)

/*++
Function:
    TLSMakeOptimizedGetter

    Creates a platform-optimized version of TlsGetValue compiled
    for a particular index.
--*/
PAL_POPTIMIZEDTLSGETTER
TLSMakeOptimizedGetter(
        IN DWORD dwTlsIndex);

/*++
Function:
    TLSFreeOptimizedGetter

    Frees a function created by TLSMakeOptimizedGetter().
--*/
VOID
TLSFreeOptimizedGetter(
        IN PAL_POPTIMIZEDTLSGETTER pOptimizedTlsGetter);

#else

THREAD* PROCGetCurrentThreadObjectInternal();

#define PROCGetCurrentThreadObject() (PROCGetCurrentThreadObjectInternal())

#endif

#else

#define PROCGetCurrentThreadObject() (pthread_getspecific(thObjKey))

#endif

/*++
Function:
    TLSInitialize

    Initialize the TLS subsystem
--*/
BOOL TLSInitialize();

/*++
Function:
    TLSCleanup

    Shutdown the TLS subsystem
--*/
VOID TLSCleanup();

/*++
Function:
    TLSCacheInitialize

    Initialize the TLS cache
--*/
VOID TLSCacheInitialize();

/*++
Function:
    TLSCacheFlush

    Notify TLS cache about THREAD structure reuse or pthread_setspecific(thObjKey, ...)
--*/
VOID TLSCacheFlush(THREAD* pThread);

/*++
Function:
  PROCGetRealCurrentThread

This returns the real thread handle, and not a pseudo handle like
GetCurrentThread does.
--*/
static inline HANDLE
PROCGetRealCurrentThread(
    VOID)
{
    THREAD *curr_thread;

    curr_thread = (THREAD*)PROCGetCurrentThreadObject();
    if(curr_thread == NULL) return NULL;
    return  curr_thread->hThread;
}


/*++
Function:
  THREADGetPipe

Parameters:
    VOID

Return value:
    The pipe associated with the thread. This pipe is used to
    block/unblock the thread.
--*/
static inline int THREADGetPipe(VOID)
{
    THREAD *lpThread;

    /* Retreive the thread object */
    lpThread = (THREAD*)PROCGetCurrentThreadObject();
    
    if (lpThread == NULL)
    {
        return 0;
    }
    return lpThread->blockingPipe;
}

#endif /* _PAL_THREAD_H_ */
