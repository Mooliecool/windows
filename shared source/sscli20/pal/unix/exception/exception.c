/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    exception.c

Abstract:

    Implementation of exception API functions.

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/thread.h"
#include "pal/critsect.h"
#include "pal/seh.h"
#include "pal/debug.h"
#include "signal.h"
#include "pal/init.h"

#if HAVE_ALLOCA_H
#include "alloca.h"
#endif

#include <errno.h>
#include <string.h>
#if HAVE_MACH_EXCEPTIONS
#include "machexception.h"
#else
#include <signal.h>
#endif
#include <setjmp.h>
#include <unistd.h>
#include <pthread.h>

SET_DEFAULT_DEBUG_CHANNEL(EXCEPT);

/* Constant ant type definitions **********************************************/

/* Bit 28 of exception codes is reserved. */
const UINT RESERVED_SEH_BIT = 0x800000;

typedef struct CTRL_HANDLER_LIST
{
    PHANDLER_ROUTINE handler;
    struct CTRL_HANDLER_LIST *next;
} CTRL_HANDLER_LIST;


/* Static variables ***********************************************************/

/* for manipulating process control-handler list, etc */
CRITICAL_SECTION exception_critsec;
static LPTOP_LEVEL_EXCEPTION_FILTER pTopFilter;
static CTRL_HANDLER_LIST * pCtrlHandler;
static int nCtrlHandlerListLength;

/* Static function declarations ***********************************************/

void SEHUnwind(LPCONTEXT contextRecord);

/* PAL function definitions ***************************************************/

/*++
Function:
  RaiseException

See MSDN doc.
--*/
VOID
PALAPI
RaiseException(
           IN DWORD dwExceptionCode,
           IN DWORD dwExceptionFlags,
           IN DWORD nNumberOfArguments,
           IN CONST ULONG_PTR *lpArguments)
{
    EXCEPTION_RECORD record;
    EXCEPTION_POINTERS exceptionPointers;
    CONTEXT context;


/*  
    PERF_ENTRY_ONLY is used here because RaiseException may or may not 
    return. We can not get latency data without PERF_EXIT. For this reason,
    PERF_ENTRY_ONLY is used to profile frequency only. 
*/
    PERF_ENTRY_ONLY(RaiseException);
    ENTRY("RaiseException (dwCode=%#x, dwFlags=%#x, nArgs=%u, lpArgs=%p)\n",
          dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);

    /* Validate parameters */
    if( dwExceptionCode & RESERVED_SEH_BIT)
    {
        WARN("Exception code %08x has bit 28 set : clearing it.\n",
             dwExceptionCode);
        dwExceptionCode ^= RESERVED_SEH_BIT;
    }

    if( dwExceptionFlags && ( dwExceptionFlags != EXCEPTION_NONCONTINUABLE ) )
    {
        ERROR("Unkown exception flag %08x (ignoring)\n", dwExceptionFlags);
    }

    /* Build exception record */

    TRACE("Building exception record %p\n", &record);

    record.ExceptionCode = dwExceptionCode;
    record.ExceptionAddress = RaiseException;
    record.ExceptionRecord = NULL;

    if( lpArguments != NULL )
    {
        if(nNumberOfArguments>EXCEPTION_MAXIMUM_PARAMETERS)
        {
            WARN("Number of arguments (%d) exceeds the limit "
                 "EXCEPTION_MAXIMUM_PARAMETERS (%d); truncing the extra"
                 "parameters.\n",
                  nNumberOfArguments, EXCEPTION_MAXIMUM_PARAMETERS);

            nNumberOfArguments = EXCEPTION_MAXIMUM_PARAMETERS;
        }

        memcpy(record.ExceptionInformation, lpArguments,
               nNumberOfArguments*sizeof(ULONG_PTR));
    }

    record.NumberParameters = nNumberOfArguments;

    /* from rotor_pal.doc :
       "all explicitly raised exceptions may be considered non-continuable". */
    record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

    /* from rotor_pal.h : 
      On non-Win32 platforms, the CONTEXT pointer in the
      PEXCEPTION_POINTERS will contain at least the CONTEXT_CONTROL registers.
    */
    ZeroMemory(&context, sizeof context);
    context.ContextFlags = CONTEXT_CONTROL;
    if(!GetThreadContext(GetCurrentThread(),&context))
    {
        WARN("GetThreadContext failed!! ignoring...\n");
    }

    exceptionPointers.ExceptionRecord = &record;
    exceptionPointers.ContextRecord = &context;
    
    /* Let SEHRaiseException do the work */
    SEHRaiseException(&exceptionPointers, 0);

    LOGEXIT("RaiseException returns.\n");
}

LPTOP_LEVEL_EXCEPTION_FILTER
PALAPI
SetUnhandledExceptionFilter(
                IN LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
    LPTOP_LEVEL_EXCEPTION_FILTER old_filter;

    PERF_ENTRY(SetUnhandledExceptionFilter);
    ENTRY("SetUnhandledExceptionFilter (lpFilter=%p)\n",
          lpTopLevelExceptionFilter);

    /* This load/store combination is not thread-safe.  However,
       the Win32 implementation isn't thread-safe either. */    
    old_filter = pTopFilter;
    pTopFilter = lpTopLevelExceptionFilter;

    LOGEXIT("SetUnhandledExceptionFilter returns LPTOP_LEVEL_EXCEPTION_FILTER "
          "%p\n", old_filter);

    PERF_EXIT(SetUnhandledExceptionFilter);
    return old_filter;
}


/*++
Function:
  SetConsoleCtrlhandler

See MSDN doc.

--*/
BOOL
PALAPI
SetConsoleCtrlHandler(
              IN PHANDLER_ROUTINE HandlerRoutine,
              IN BOOL Add)
{
    BOOL retval = FALSE;
    CTRL_HANDLER_LIST *handler;

    PERF_ENTRY(SetConsoleCtrlHandler);
    ENTRY("SetConsoleCtrlHandler(HandlerRoutine=%p, Add=%d\n", 
          HandlerRoutine, Add);

    SYNCEnterCriticalSection(&exception_critsec, TRUE);
    
    if(NULL == HandlerRoutine)
    {
        ASSERT("HandlerRoutine may not be NULL, control-c-ignoration is not "
               "supported\n");
        goto done;
    }


    if(Add)
    {
        handler = (CTRL_HANDLER_LIST *)malloc(sizeof(CTRL_HANDLER_LIST));
        if(!handler)
        {
            ERROR("malloc failed! error is %d (%s)\n", errno, strerror(errno));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
        handler->handler = HandlerRoutine;
        /* From MSDN : 
           "handler functions are called on a last-registered, first-called 
           basis". So we can add the new handler at the head of the list. */
        handler->next = pCtrlHandler;
        pCtrlHandler = handler;
        nCtrlHandlerListLength++;

        TRACE("Adding Control Handler %p\n", HandlerRoutine);
        retval = TRUE;
    }
    else
    {
        CTRL_HANDLER_LIST *temp_handler;

        handler = pCtrlHandler;
        temp_handler = handler;
        while(handler)
        {
            if(handler->handler == HandlerRoutine)
            {
                break;
            }
            temp_handler = handler;
            handler = handler->next;
        }
        if(handler)
        {
            /* temp_handler it the item before the one to remove, unless it was
               first in the list, in which case handler == temp_handler */
            if(handler == temp_handler)
            {
                /* handler to remove was first in the list... */
                nCtrlHandlerListLength--;
                pCtrlHandler = handler->next;

                free(handler);
                TRACE("Removing Control Handler %p from head of list\n", 
                      HandlerRoutine );
            }
            else
            {
                /* handler was not first in the list... */
                nCtrlHandlerListLength--;
                temp_handler->next = handler->next;
                free(handler);
                TRACE("Removing Control Handler %p (not head of list)\n", 
                      HandlerRoutine );                 
            }                 
            retval = TRUE;
        }
        else
        {
            WARN("Trying to remove unknown Control Handler %p\n", 
                 HandlerRoutine);
            SetLastError(ERROR_INVALID_PARAMETER);
        }
    }
done:
    SYNCLeaveCriticalSection(&exception_critsec, TRUE);

    LOGEXIT("SetConsoleCtrlHandler returns BOOL %d\n", retval);
    PERF_EXIT(SetConsoleCtrlHandler);
    return retval;
}

/*++
Function:
  GenerateConsoleCtrlEvent

See MSDN doc.

PAL specifics :
    dwProcessGroupId must be zero
              
--*/
BOOL
PALAPI
GenerateConsoleCtrlEvent(
    IN DWORD dwCtrlEvent,
    IN DWORD dwProcessGroupId
    )
{
    int sig;
    BOOL retval = FALSE;

    PERF_ENTRY(GenerateConsoleCtrlEvent);
    ENTRY("GenerateConsoleCtrlEvent(dwCtrlEvent=%d, dwProcessGroupId=%#x\n",
        dwCtrlEvent, dwProcessGroupId);

    if(0!=dwProcessGroupId)
    {
        ASSERT("dwProcessGroupId is not 0, this is not supported by the PAL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    switch(dwCtrlEvent)
    {
    case CTRL_C_EVENT :
        sig = SIGINT;
        break;
    case CTRL_BREAK_EVENT:
        /* Map control-break on SIGQUIT */
        sig = SIGQUIT;
        break;
    default:
        TRACE("got unknown control event\n");
        goto done;
    }

    TRACE("sending signal %d to process %d\n", sig, gPID);
    if(-1 == kill(gPID, sig))
    {
        ASSERT("kill() failed; errno is %d (%s)\n",errno, strerror(errno));
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }
    retval = TRUE;
done:
    LOGEXIT("GenerateConsoleCtrlEvent returns BOOL %d\n",retval);
    PERF_EXIT(GenerateConsoleCtrlEvent);
    return retval;
}



/*++
Function:
  PAL_TryHelper

Abstract:
  Set up an exception frame (call setjmp, complete the exception registration
  structure and add it to thread's list

Parameters :
    PPAL_EXCEPTION_REGISTRATION pRegistration : exception registration
                structure with valid Handler and pvFilterParameter values

(no return value)
--*/
void
PALAPI
PAL_TryHelper(
    IN OUT PPAL_EXCEPTION_REGISTRATION pRegistration)
{
#if !DISABLE_EXCEPTIONS
    PPAL_EXCEPTION_REGISTRATION old_frame;
    THREAD* lpThread = NULL;

    if( !pRegistration )
    {
        ASSERT("Invalid exception registration pointer!\n");
    }

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }

    old_frame = lpThread->bottom_frame;

/*
    The frames' addresses do not have to be in descending order - the optimizing compiler
    can reorder the frames if there are two or more nested handlers in one function.

    if( old_frame && old_frame < pRegistration )
    {
        ASSERT("New exception frame is above previous frame! Stack is "
                "corrupted!\n");
    }
*/

    pRegistration->Next = old_frame;

    lpThread->bottom_frame = pRegistration;

done:
    return;
#endif // !DISABLE_EXCEPTIONS
}


/*++
Function:
  PAL_EndTryHelper

Abstract:
  Clean up the SEH stack when leaving a try block, and jump to the next
  exception frame if we're unwinding

Parameters :
    PPAL_EXCEPTION_REGISTRATION pRegistration : current exception frame
    int ExceptionCode : 0 if no exception occurred, nonzero if an exception
                        occurred and this is a stack unwind

    Always returns 0 that can be used to prevent compiler optimizations
    on setjmp/longjmp
--*/
int
PALAPI
PAL_EndTryHelper(
    IN OUT PPAL_EXCEPTION_REGISTRATION pRegistration,
    IN int ExceptionCode)
{
#if !DISABLE_EXCEPTIONS
    THREAD* lpThread = NULL;

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }

    if( ExceptionCode == 0 )
    {
        /* If ExceptionCode is zero, there was no exception, so we
           only need to pop the exception frame. */

        if( (pRegistration == NULL) ||
            (pRegistration != lpThread->bottom_frame) )
        {
            ASSERT("Exception registration pointers don't match!\n");
            goto done;
        }
        lpThread->bottom_frame = pRegistration->Next;        
    }

    // ExceptionCode is always 0 on all calls to PAL_EndTryHelper
    // Is the rest of this function's code alive?

    else if( pRegistration->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET )
    {
        /* Stop unwinding if this is the target frame */
        
        /* If there was an exception, the current frame has already been popped 
           before the longjmp */

        if( (pRegistration == NULL) ||
            (pRegistration->Next != lpThread->bottom_frame) )
        {                 
            ASSERT("Exception registration pointers don't match!\n");
            goto done;
        }

        TRACE("Exception handled by frame %p. Lowest frame is now %p\n", 
              pRegistration, pRegistration->Next);
    }
    else
    {
        TRACE("Unwinding in progress; looking for next frame\n");
        
        /* If there was an exception, the current frame has already been popped 
           before the longjmp */

        if( (pRegistration == NULL) ||
            (pRegistration->Next != lpThread->bottom_frame) )
        {
            ASSERT("Exception registration pointers don't match!\n");
            goto done;
        }

        SEHUnwind(NULL);

        TRACE("Reached top of exception stack! Exception was not "
              "handled, terminating the process.\n");

        if(!PALIsInitialized())
        {
            /* The only way we can get here is if PAL_Terminate is currently 
               running, but we haven't yet restored the default signal handlers. 
               This isn't a problem if an exception handler accepts the 
               exception, but if we reach here,TerminateProcess will have to use 
               the "non-initialized" code path : save status and exit, without 
               cleaning up shared memory etc. Not good, but no choice, so we 
               indicate the problem but proceed anyway. */
            WARN("Exception was raised during PAL termination!\n");
        }

        TerminateProcess(GetCurrentProcess(),
                         lpThread->current_exception.ExceptionCode);
    }

done:
#endif // !DISABLE_EXCEPTIONS
    return 0;
}

/*++
Function:
  PAL_GetBottommostRegistration

Abstract:
  Return a pointer to the first PAL_EXCEPTION_REGISTRATION structure on the
  current thread's stack.

  This function is highly simplified since it is used heavily.
--*/
PPAL_EXCEPTION_REGISTRATION
PALAPI
PAL_GetBottommostRegistration(
                  VOID)
{
    PPAL_EXCEPTION_REGISTRATION ret = NULL;
    THREAD* lpThread = NULL;

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ERROR( "Unable to get the thread object.\n" );
        goto done;
    }

    ret = lpThread->bottom_frame;

done:
    return ret;
}


/*++
Function:
  PAL_SetBottommostRegistration

Abstract:
  Sets the PPAL_EXCEPTION_REGISTRATION record that is bottommost on the stack  
--*/
VOID
PALAPI
PAL_SetBottommostRegistration(
              PPAL_EXCEPTION_REGISTRATION pRegistration)
{
    THREAD* lpThread = NULL;

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ASSERT("Unable to get the thread object.\n");
        goto done;
    }

    lpThread->bottom_frame = pRegistration;

done: ;
}

/*++
Function:
  PAL_GetBottommostRegistrationPtr

Abstract:
  Return the address where the *pointer* to the current bottommost exception 
  frame is stored for the current thread
--*/
PPAL_EXCEPTION_REGISTRATION *
PALAPI
PAL_GetBottommostRegistrationPtr(
              VOID)
{
    PPAL_EXCEPTION_REGISTRATION * ret = NULL;
    THREAD* lpThread = NULL;

    PERF_ENTRY(PAL_GetBottommostRegistrationPtr);
    ENTRY("PAL_GetBottommostRegistrationPtr () \n");

    /* Get the per-thread buffer from the thread structure. */
    lpThread = PROCGetCurrentThreadObject();
    if ( !lpThread )
    {
        ASSERT("Unable to get the thread object.\n");
        goto done;
    }

    ret = &lpThread->bottom_frame;

done:
    LOGEXIT("PAL_GetBottommostRegistrationPtr returning "
          "PPAL_EXCEPTION_REGISTRATION *%p\n", ret);

    PERF_EXIT(PAL_GetBottommostRegistrationPtr);
    return ret;
}

/* Internal function definitions **********************************************/

/*++
Function :
    SEHInitialize

    Initialize all SEH-related stuff (signals, etc)

    (no parameters)

Return value :
    TRUE  if SEH support initialization succeeded
    FALSE otherwise
--*/
BOOL SEHInitialize (void)
{
    BOOL bRet = FALSE;

    if (PAL_TRY_LOCAL_SIZE < sizeof(sigjmp_buf))
    {
        ASSERT("PAL_TRY_LOCAL_SIZE does not match sizeof(sigjmp_buf)!\n");
    }

    pTopFilter = NULL;
    pCtrlHandler = NULL;
    nCtrlHandlerListLength = 0;

    if (0 != SYNCInitializeCriticalSection(&exception_critsec))
    {
        ERROR("Unable to initialize SEH critical section!\n");
    }
    else
    {
#if HAVE_MACH_EXCEPTIONS
        if (!SEHInitializeMachExceptions())
        {
            ERROR("SEHInitializeMachExceptions failed!\n");
            SEHCleanup();
        } 
#else
        SEHInitializeSignals();
#endif
        bRet = TRUE;
    }

    return bRet;
}

/*++
Function :
    SEHCleanup

    Undo work done by SEHInitialize

    (no parameters, no return value)
    
--*/
void SEHCleanup (void)
{
    TRACE("Cleaning up SEH\n");

#if HAVE_MACH_EXCEPTIONS
    SEHCleanupExceptionPort();
#else
    SEHCleanupSignals();
#endif
}

LONG PAL_RunFilters(PEXCEPTION_POINTERS pException);

/*++
Function :
    SEHRaiseException

    Raise an exception given a specified exception information.

Parameters :
    PEXCEPTION_POINTERS lpExceptionPointers : specification of exception
    to raise.
    int signal_code : signal that caused the exception, if applicable; 
                      0 otherwise

(no return value)
--*/
void SEHRaiseException( PEXCEPTION_POINTERS lpExceptionPointers, 
                        int signal_code )
{
    LPTOP_LEVEL_EXCEPTION_FILTER top_filter;
    LONG default_action;
    THREAD *lpThread;
    LONG filterResult;

    if(!lpExceptionPointers)
    {
        ASSERT("Invalid exception record!\n");
        return;
    }

    TRACE("Raising exception %#x (record is %p)\n",
          lpExceptionPointers->ExceptionRecord->ExceptionCode,
          lpExceptionPointers);

    lpThread = PROCGetCurrentThreadObject();
    if(NULL == lpThread)
    {
        ASSERT("Couldn't get SEH thread data!\n");
    }
    else
    {
        /* save copy of exception record for access after longjmp */
        memcpy(&lpThread->current_exception,
               lpExceptionPointers->ExceptionRecord, 
               sizeof lpThread->current_exception);
    }

    lpThread->signal_code = signal_code;
    
    /* Call exception filters until one returns EXCEPTION_EXECUTE_HANDLER */

    TRACE("Looking for exception handler...\n");

    filterResult = PAL_RunFilters(lpExceptionPointers);

    /* once we get here we can assume the exception stack is (reasonably) 
       valid, so we can allow signals to be handled again */
    SEHSetSafeState(TRUE);

    switch (filterResult) {
    case EXCEPTION_EXECUTE_HANDLER:
        SEHUnwind(lpExceptionPointers->ContextRecord);
        // control should not get here

    case EXCEPTION_CONTINUE_EXECUTION:
        return;

    default:
        break;
    }

    /* No handler found : start default handling */

    TRACE("No handler found for exception. Using default behavior.\n");

    /* Call application-defined top-level filter. */
    top_filter = pTopFilter;

    if(top_filter)
    {
        TRACE("Calling application-defined top-level filter\n");

/* reset ENTRY nesting level back to zero while inside the callback... */
#if !_NO_DEBUG_MESSAGES_
    {
        int old_level;
        old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

        default_action = top_filter(lpExceptionPointers);

/* ...and set nesting level back to what it was */
#if !_NO_DEBUG_MESSAGES_
        DBG_change_entrylevel(old_level);
    }
#endif /* !_NO_DEBUG_MESSAGES_ */

        switch( default_action )
        {
        case EXCEPTION_CONTINUE_SEARCH:
            break;
        case EXCEPTION_EXECUTE_HANDLER:
            break;
        default:
            ASSERT("Application-defined top-level exception filter returned "
                  "invalid value %d\n", default_action);
            default_action = EXCEPTION_CONTINUE_SEARCH;
            break;
        }
    }
    else
    {
        default_action = EXCEPTION_CONTINUE_SEARCH;
    }

    WARN("Exception %08x not handled; process will be terminated.\n",
          lpExceptionPointers->ExceptionRecord->ExceptionCode);

    SEHUnwind(lpExceptionPointers->ContextRecord);

    /* Exception not handled, no termination handlers to execute : terminate */

    TRACE("Reached top of exception stack! Exception was not handled, "
          "terminating the process.\n");

    if(!PALIsInitialized())
    {
        /* The only way we can get here is if PAL_Terminate is currently 
           running, but we haven't yet restored the default signal handlers. 
           This isn't a problem if an exception handler accepts the exception, 
           but if we reach here, TerminateProcess will have to use the 
           "non-initialized" code path : save status and exit, wihtout cleaning 
           up shared memory etc. Not good, but no choice, so we indicate the 
           problem but proceed anyway. */
        WARN("Exception was raised during PAL termination!\n");
    } 
    else if(0 != signal_code)
    {
        // Terminate unconditionally. The application is not in
        // a safe state.
#if HAVE_MACH_EXCEPTIONS
        SEHCleanupExceptionPort();
#else
        SEHCleanupSignals();
#endif
        PROCCleanupProcess(TRUE);

        /* signal handlers uninstalled : let the signal be raised again */
        return;                                                         
    }                                                                   

    /* exit code is exception code */
    TerminateProcess(GetCurrentProcess(),
                     lpExceptionPointers->ExceptionRecord->ExceptionCode);
    ASSERT("TerminateProcess() returned!\n");
}

/*++
Function :
    SEHHandleControlEvent

    handle Control-C and Control-Break events (call handler routines, 
    notify debugger)

Parameters :
    DWORD event : event that occurred
    LPVOID eip  : instruction pointer when exception occurred                                 

(no return value)

Notes :
    Handlers are called on a last-installed, first called basis, until a
    handler returns TRUE. If no handler returns TRUE (or no hanlder is
    installed), the default behavior is to call TerminateProcess
--*/
void SEHHandleControlEvent(DWORD event, LPVOID eip)
{
    /* handler is actually a copy of the original list */
    CTRL_HANDLER_LIST *handler=NULL, *handlertail=NULL, *handlertmp, *newelem;
    BOOL handled = FALSE;

    /* second, call handler routines until one handles the event */
    SYNCEnterCriticalSection(&exception_critsec, TRUE);
    handlertmp = pCtrlHandler;

    /* nCtrlHandlerListLength is guaranteed to be at most 1 less than,
     * and not greater than, actual length.
     * We might get a stack overflow here, if the list is too large
     * However, that will lead us to terminate with an error, which is
     * the default behavior anyway.
     */
    newelem = alloca(sizeof(CTRL_HANDLER_LIST)*(nCtrlHandlerListLength+1));

    // If alloca failed, we terminate
    if (newelem == NULL) {
        ERROR("alloca failed!");
        goto done;
    }

    /* list copying */
    while(NULL!=handlertmp)
    {
        newelem->handler = handlertmp->handler;
        newelem->next = NULL;
 
        /* add the new element to the list */
        if (handler == NULL)
        {
            handler = newelem;
        }
        else
        {
            handlertail->next = newelem;
        }
        handlertail = newelem;
 
        handlertmp = handlertmp->next;

        newelem++;
    }

    while(NULL!=handler)
    {
        BOOL handler_retval;

/* reset ENTRY nesting level back to zero while inside the callback... */
#if !_NO_DEBUG_MESSAGES_
    {
        int old_level;
        old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

        handler_retval = handler->handler(event);

/* ...and set nesting level back to what it was */
#if !_NO_DEBUG_MESSAGES_
        DBG_change_entrylevel(old_level);
    }
#endif /* !_NO_DEBUG_MESSAGES_ */

        if(handler_retval)
        {
            TRACE("Console Control handler %p has handled event\n",
                  handler->handler);
            handled = TRUE;
            break;
        }
        handler = handler->next;
    }

done:
    SYNCLeaveCriticalSection(&exception_critsec, TRUE);

    if(!handled)
    {
        if(CTRL_C_EVENT == event)
        {
            TRACE("Control-C not handled; terminating.\n");
        }
        else
        {
            TRACE("Control-Break not handled; terminating.\n");
        }
        /* tested in Win32 : this is the exit code when ^C and ^Break are not 
           handled */
        TerminateProcess(GetCurrentProcess(),CONTROL_C_EXIT);
    }
}

/*++
Function :
    SEHSetSafeState

    specify whether the current thread is in a state where exception handling 
    of signals can be done safely

Parameters:
    BOOL state : TRUE if the thread is safe, FALSE otherwise

(no return value)
--*/
void SEHSetSafeState(BOOL state)
{
    THREAD* lpThread;

    lpThread = PROCGetCurrentThreadObject();
    if(NULL == lpThread)
    {
        ASSERT("couldn't retrieve SEH TLS structure!\n");
        return;
    }
    lpThread->safe_state = state;
}

/*++
Function :
    SEHGetSafeState

    determine whether the current thread is in a state where exception handling 
    of signals can be done safely

    (no parameters)

Return value :
    TRUE if the thread is in a safe state, FALSE otherwise
--*/
BOOL SEHGetSafeState(void)
{
    THREAD* lpThread;

    lpThread = PROCGetCurrentThreadObject();
    if(NULL == lpThread)
    {
        ASSERT("couldn't retrieve SEH TLS structure!\n");
        return FALSE;
    }
    return lpThread->safe_state;
}


/*++
Function :
    SEHGetCurrentException

    get the current_exception out of seh_info

    (no parameters)

Return value :
    seh_info->current_exception
--*/
EXCEPTION_RECORD *SEHGetCurrentException(void)
{
    THREAD* lpThread;

    lpThread = PROCGetCurrentThreadObject();
    if(NULL == lpThread)
    {
        ASSERT("couldn't retrieve SEH TLS structure!\n");
        return NULL;
    }
    return &(lpThread->current_exception);
}


/* Static function definitions ************************************************/

VOID
HijackForUnwind(IN LPCONTEXT context, IN LPVOID target);
void Cpp_RaiseException();

void SEHUnwind(LPCONTEXT contextRecord)
{
    PAL_EXCEPTION_REGISTRATION *jmp_frame;
    EXCEPTION_POINTERS ep;
    LONG retval;
    THREAD * lpThread;

    lpThread = PROCGetCurrentThreadObject();

    jmp_frame = PAL_GetBottommostRegistration();
    while(NULL != jmp_frame)
    {
        if (jmp_frame->dwFlags & PAL_EXCEPTION_FLAGS_CPPEHUNWIND) {
            // OK, we need to make gcc unwind upto this point.
            
            jmp_frame->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;
            PAL_SetBottommostRegistration(jmp_frame);

            // Cause a Cpp throw to unwind gcc's handlers upto next managed code block
#if HIJACK_ON_SIGNAL
            if (contextRecord && lpThread && (lpThread->current_exception.ExceptionFlags & EXCEPTION_IS_SIGNAL))
            {
                HijackForUnwind(contextRecord, (LPVOID)Cpp_RaiseException);
            }
            else
#endif
            {
                Cpp_RaiseException();
            }
            // That will not return
        }
        else if (jmp_frame->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDONLY)
        {
            jmp_frame->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;

            PAL_SetBottommostRegistration(jmp_frame);

            ep.ExceptionRecord = (PEXCEPTION_RECORD) jmp_frame->ReservedForPAL;
            ep.ExceptionRecord->ExceptionFlags |= EXCEPTION_UNWINDING;
            ep.ContextRecord = NULL;

/* reset ENTRY nesting level back to zero while inside the callback... */
#if !_NO_DEBUG_MESSAGES_
            {
                int old_level;
                old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

                retval = jmp_frame->Handler(&ep, 
                                            jmp_frame->pvFilterParameter);
                
/* ...and set nesting level back to what it was, (if callback returned) */
#if !_NO_DEBUG_MESSAGES_
                DBG_change_entrylevel(old_level);
            }
#endif /* !_NO_DEBUG_MESSAGES_ */

            if(EXCEPTION_CONTINUE_SEARCH!=retval)
            {
                ASSERT("Win32 EH filter returned %d instead of "
                       "EXCEPTION_CONTINUE_SEARCH (%d)!\n", 
                       retval, EXCEPTION_CONTINUE_SEARCH);
                break; /* because what else can we do? */
            }

            if (jmp_frame->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET)
            {
                ASSERT("PAL_EXCEPTION_FLAGS_UNWINDTARGET is set\n");
            }
        }           
        else if( jmp_frame->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET )
        {
            TRACE("Found unwind target frame (%p)\n", jmp_frame);
            break;
        }

        jmp_frame = jmp_frame->Next;
    }
            
    if (jmp_frame)
    {
        TRACE("Jumping to exception frame %p\n", jmp_frame);

        /* remove the target frame from the chain : if there's an exception 
            within the handler, we don't want it to be sent back to the same
            handler */
        PAL_SetBottommostRegistration(jmp_frame->Next);

/* reset ENTRY nesting level back to zero before longjmping */
#if !_NO_DEBUG_MESSAGES_
        {
            int old_level;

            if (lpThread == NULL) {
                ASSERT("lpThread is NULL\n");
            }

            old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */
              
#if HIJACK_ON_SIGNAL
            if (contextRecord && lpThread && (lpThread->current_exception.ExceptionFlags & EXCEPTION_IS_SIGNAL))
            {
                HijackForUnwind(contextRecord, (LPVOID)Cpp_RaiseException);
            }
            else
#endif
            {
                Cpp_RaiseException();
            }

#if !_NO_DEBUG_MESSAGES_
        }
#endif /* !_NO_DEBUG_MESSAGES_ */
    }

    PAL_SetBottommostRegistration(NULL);

    /* exit code is exception code */
    if(NULL == lpThread)
    {
        WARN("SEH thread data not available, no exception code available! "
             "using fake exit code\n");
        TerminateProcess(GetCurrentProcess(),-1);
    }
    if(0 == lpThread->signal_code)
    {   
        TerminateProcess(GetCurrentProcess(),
                         lpThread->current_exception.ExceptionCode);
    }
    else
    {
        int signal_code;

        TRACE("unhandled signal : shutting down the PAL and re-raising the "
              "signal\n");
        signal_code = lpThread->signal_code;
        // Terminate unconditionally. The application is not in a
        // safe state.
#if HAVE_MACH_EXCEPTIONS
        SEHCleanupExceptionPort();
#else
        SEHCleanupSignals();    
#endif
        PROCCleanupProcess(TRUE);

        /* signal handlers now uninstalled : raise the signal again */
        kill(gPID, signal_code);
    }
}
