/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    exception/signal.c

Abstract:

    Signal handler implementation (map signals to exceptions)

--*/


#include "pal/palinternal.h"
#if !HAVE_MACH_EXCEPTIONS
#include "pal/dbgmsg.h"
#include "pal/seh.h"
#include "pal/init.h"
#include "pal/thread.h"
#include "pal/context.h"
#include "pal/debug.h"

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/ucontext.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <unistd.h>

SET_DEFAULT_DEBUG_CHANNEL(EXCEPT);

/* local type definitions *****************************************************/

#if !HAVE_SIGINFO_T
/* This allows us to compile on platforms that don't have siginfo_t.
 * Exceptions will work poorly on those platforms. */
#warning Exceptions will work poorly on this platform
typedef void *siginfo_t;
#endif  /* !HAVE_SIGINFO_T */
typedef void (*SIGFUNC)(int, siginfo_t *, void *);

/* Static variables ***********************************************************/
static LONG fatal_signal_received;


/* internal function declarations *********************************************/

static void sigint_handler(int code, siginfo_t *siginfo, void *context);
static void sigquit_handler(int code, siginfo_t *siginfo, void *context);
static void sigill_handler(int code, siginfo_t *siginfo, void *context);
static void sigfpe_handler(int code, siginfo_t *siginfo, void *context);
static void sigsegv_handler(int code, siginfo_t *siginfo, void *context);
static void sigtrap_handler(int code, siginfo_t *siginfo, void *context);
static void sigbus_handler(int code, siginfo_t *siginfo, void *context);
static void fatal_signal_handler(int code, siginfo_t *siginfo, void *context);
static void common_signal_handler(PEXCEPTION_POINTERS pointers, int code, 
                                  native_context_t *ucontext);

void handle_signal(int signal_id, SIGFUNC sigfunc);

#if HAVE__THREAD_SYS_SIGRETURN
int _thread_sys_sigreturn(native_context_t *);
#endif

/* public function definitions ************************************************/

/*++
Function :
    SEHInitializeSignals

    Set-up signal handlers to catch signals and translate them to exceptions

    (no parameters, no return value)
--*/
void SEHInitializeSignals(void)
{
    TRACE("Initializing signal handlers\n");

    fatal_signal_received = 0;
    
    /* we call handle signal for every possible signal, even
       if we don't provide a signal handler. 

       handle_signal will set SA_RESTART flag for specified signal.
       Therefore, all signals will have SA_RESTART flag set, preventing
       slow Unix system calls from being interrupted. On systems without
       siginfo_t, SIGKILL and SIGSTOP can't be restarted, so we don't
       handle those signals. Both the Darwin and FreeBSD man pages say
       that SIGKILL and SIGSTOP can't be handled, but FreeBSD allows us
       to register a handler for them anyway. We don't do that.

       see sigaction man page for more details
    */
    handle_signal(SIGHUP,    fatal_signal_handler);
    handle_signal(SIGINT,    sigint_handler);
    handle_signal(SIGQUIT,   sigquit_handler);
    handle_signal(SIGILL,    sigill_handler);
    handle_signal(SIGTRAP,   sigtrap_handler);
    handle_signal(SIGABRT,   fatal_signal_handler); 
#ifdef SIGEMT
    handle_signal(SIGEMT,    fatal_signal_handler);
#endif // SIGEMT
    handle_signal(SIGFPE,    sigfpe_handler);
    handle_signal(SIGBUS,    sigbus_handler);
    handle_signal(SIGSEGV,   sigsegv_handler);
    handle_signal(SIGSYS,    fatal_signal_handler); 
    handle_signal(SIGALRM,   fatal_signal_handler); 
    handle_signal(SIGTERM,   fatal_signal_handler); 
    handle_signal(SIGURG,    NULL);
    handle_signal(SIGTSTP,   NULL);
    handle_signal(SIGCONT,   NULL);
    handle_signal(SIGCHLD,   NULL);
    handle_signal(SIGTTIN,   NULL);
    handle_signal(SIGTTOU,   NULL);
    handle_signal(SIGIO,     NULL);
    handle_signal(SIGXCPU,   fatal_signal_handler);
    handle_signal(SIGXFSZ,   fatal_signal_handler);
    handle_signal(SIGVTALRM, fatal_signal_handler);
    handle_signal(SIGPROF,   fatal_signal_handler);
    handle_signal(SIGWINCH,  NULL);
#ifdef SIGINFO
    handle_signal(SIGINFO,   NULL);
#endif  // SIGINFO
    handle_signal(SIGUSR1,   fatal_signal_handler);
    handle_signal(SIGUSR2,   fatal_signal_handler);
    
    
    
    /* The default action for SIGPIPE is process termination.
       Since SIGPIPE can be signaled when trying to write on a socket for which
       the connection has been dropped, we need to tell the system we want
       to ignore this signal. 
       
       Instead of terminating the process, the system call which would had
       issued a SIGPIPE will, instead, report an error and set errno to EPIPE.
    */
    signal(SIGPIPE, SIG_IGN);
}

/*++
Function :
    SEHCleanupSignals

    Restore default signal handlers

    (no parameters, no return value)
    
note :
reason for this function is that during PAL_Terminate, we reach a point where 
SEH isn't possible anymore (handle manager is off, etc). Past that point, 
we can't avoid crashing on a signal     
--*/
void SEHCleanupSignals (void)
{
    TRACE("Restoring default signal handlers\n");

    handle_signal(SIGHUP,    NULL);
    handle_signal(SIGINT,    NULL);
    handle_signal(SIGQUIT,   NULL);
    handle_signal(SIGILL,    NULL);
    handle_signal(SIGTRAP,   NULL);
    handle_signal(SIGABRT,   NULL); 
#ifdef SIGEMT
    handle_signal(SIGEMT,    NULL);
#endif // SIGEMT
    handle_signal(SIGFPE,    NULL);
    handle_signal(SIGBUS,    NULL);
    handle_signal(SIGPIPE,   NULL);
    handle_signal(SIGSEGV,   NULL);
    handle_signal(SIGSYS,    NULL); 
    handle_signal(SIGALRM,   NULL); 
    handle_signal(SIGTERM,   NULL); 
    handle_signal(SIGXCPU,   NULL);
    handle_signal(SIGXFSZ,   NULL);
    handle_signal(SIGVTALRM, NULL);
    handle_signal(SIGPROF,   NULL);
    handle_signal(SIGUSR1,   NULL);
    handle_signal(SIGUSR2,   NULL);
}


/* internal function definitions **********************************************/

/*++
Function :
    sigint_handler

    handle SIGINT signal (CONTROL_C_EXIT exception)

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigint_handler(int code, siginfo_t *siginfo, void *context)
{
    native_context_t *ucontext;
    sigset_t signal_set;

    TRACE("Got signal SIGINT (Control-C)!\n");
    
    /* Unmask signal so we can receive it again */
    sigemptyset(&signal_set);
    sigaddset(&signal_set,code);
    if(-1 == sigprocmask(SIG_UNBLOCK,&signal_set,NULL))
    {
        ASSERT("sigprocmask failed; error is %d (%s)\n",errno, strerror(errno));
    }

    ucontext = (native_context_t *) context;
    SEHHandleControlEvent(CTRL_C_EVENT, CONTEXTGetPC(ucontext));                                     
    
    /* if we get here, control-c was handled by callback. */
    TRACE("Control-C handled; resuming execution\n");
}

/*++
Function :
    sigquit_handler

    handle SIGQUIT signal, translate to a control-break event

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigquit_handler(int code, siginfo_t *siginfo, void *context)
{
    native_context_t *ucontext;
    sigset_t signal_set;

    TRACE("Got signal SIGQUIT (Control-\\); treating as control-break\n");
    
    /* Unmask signal so we can receive it again */
    sigemptyset(&signal_set);
    sigaddset(&signal_set,code);
    if(-1 == sigprocmask(SIG_UNBLOCK,&signal_set,NULL))
    {
        ASSERT("sigprocmask failed; error is %d (%s)\n",errno, strerror(errno));
    }

    ucontext = (native_context_t *) context;
    SEHHandleControlEvent(CTRL_BREAK_EVENT, CONTEXTGetPC(ucontext));                                     
    
    /* if we get here, control-break was handled by callback. */
    TRACE("Control-C handled; resuming execution\n");
}

/*++
Function :
    sigill_handler

    handle SIGILL signal (EXCEPTION_ILLEGAL_INSTRUCTION, others?)

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigill_handler(int code, siginfo_t *siginfo, void *context)
{
    EXCEPTION_RECORD record;
    EXCEPTION_POINTERS pointers;
    native_context_t *ucontext;

    ucontext = (native_context_t *)context;

    record.ExceptionCode = CONTEXTGetExceptionCodeForSignal(siginfo, ucontext);
    record.ExceptionFlags = EXCEPTION_IS_SIGNAL; 
    record.ExceptionRecord = NULL;
    record.ExceptionAddress = CONTEXTGetPC(ucontext);
    record.NumberParameters = 0;

    pointers.ExceptionRecord = &record;

    common_signal_handler(&pointers, code, ucontext);

    TRACE("SIGILL Signal was handled; continuing execution.\n");
}

/*++
Function :
    sigfpe_handler

    handle SIGFPE signal (division by zero, floating point exception)

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigfpe_handler(int code, siginfo_t *siginfo, void *context)
{
    EXCEPTION_RECORD record;
    EXCEPTION_POINTERS pointers;
    native_context_t *ucontext;

    ucontext = (native_context_t *)context;

    record.ExceptionCode = CONTEXTGetExceptionCodeForSignal(siginfo, ucontext);
    record.ExceptionFlags = EXCEPTION_IS_SIGNAL;
    record.ExceptionRecord = NULL;
    record.ExceptionAddress = CONTEXTGetPC(ucontext);
    record.NumberParameters = 0;

    pointers.ExceptionRecord = &record;

    common_signal_handler(&pointers, code, ucontext);

    TRACE("SIGFPE Signal was handled; continuing execution.\n");
}

/*++
Function :
    sigsegv_handler

    handle SIGSEGV signal (EXCEPTION_ACCESS_VIOLATION, others)

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigsegv_handler(int code, siginfo_t *siginfo, void *context)
{
    EXCEPTION_RECORD record;
    EXCEPTION_POINTERS pointers;
    native_context_t *ucontext;

    ucontext = (native_context_t *)context;

    record.ExceptionCode = CONTEXTGetExceptionCodeForSignal(siginfo, ucontext);
    record.ExceptionFlags = EXCEPTION_IS_SIGNAL;
    record.ExceptionRecord = NULL;
    record.ExceptionAddress = CONTEXTGetPC(ucontext);
    record.NumberParameters = 2;

    // First parameter says whether a read (0) or write (non-0) caused the
    // fault. We must disassemble the instruction at record.ExceptionAddress
    // to correctly fill in this value.                              
    record.ExceptionInformation[0] = 0;

    // Second parameter is the address that caused the fault.
    record.ExceptionInformation[1] = (size_t)siginfo->si_addr;

    pointers.ExceptionRecord = &record;

    common_signal_handler(&pointers, code, ucontext);

    TRACE("SIGSEGV Signal was handled; continuing execution.\n");
}

/*++
Function :
    sigtrap_handler

    handle SIGTRAP signal (EXCEPTION_SINGLE_STEP, EXCEPTION_BREAKPOINT)

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigtrap_handler(int code, siginfo_t *siginfo, void *context)
{
    EXCEPTION_RECORD record;
    EXCEPTION_POINTERS pointers;
    native_context_t *ucontext;

    ucontext = (native_context_t *)context;

    record.ExceptionCode = CONTEXTGetExceptionCodeForSignal(siginfo, ucontext);
    record.ExceptionFlags = EXCEPTION_IS_SIGNAL;
    record.ExceptionRecord = NULL;
    record.ExceptionAddress = CONTEXTGetPC(ucontext);
    record.NumberParameters = 0;

    pointers.ExceptionRecord = &record;

    common_signal_handler(&pointers, code, ucontext);

    TRACE("SIGTRAP Signal was handled; continuing execution.\n");
}

/*++
Function :
    sigbus_handler

    handle SIGBUS signal (EXCEPTION_ACCESS_VIOLATION?)

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
static void sigbus_handler(int code, siginfo_t *siginfo, void *context)
{
    EXCEPTION_RECORD record;
    EXCEPTION_POINTERS pointers;
    native_context_t *ucontext;

    ucontext = (native_context_t *)context;

    record.ExceptionCode = CONTEXTGetExceptionCodeForSignal(siginfo, ucontext);
    record.ExceptionFlags = EXCEPTION_IS_SIGNAL;
    record.ExceptionRecord = NULL;
    record.ExceptionAddress = CONTEXTGetPC(ucontext);
    record.NumberParameters = 2;

    // First parameter says whether a read (0) or write (non-0) caused the
    // fault. We must disassemble the instruction at record.ExceptionAddress
    // to correctly fill in this value.                              
    record.ExceptionInformation[0] = 0;

    // Second parameter is the address that caused the fault.
    record.ExceptionInformation[1] = (size_t)siginfo->si_addr;

    pointers.ExceptionRecord = &record;

    common_signal_handler(&pointers, code, ucontext);

    TRACE("SIGBUS Signal was handled; continuing execution.\n");
}

/*++
Function :
    fatal_signal_handle

    handles signals for which the default action is to terminate the
    process -- used to allow us to perform cleanup before the process
    goes away

Parameters :
    POSIX signal handler parameter list ("man sigaction" for details)

    (no return value)
--*/
void fatal_signal_handler(int code, siginfo_t *siginfo, void *context)
{
    sigset_t signal_set;

    if (0 == InterlockedCompareExchange(&fatal_signal_received, 1, 0))
    {
        TRACE("Fatal signal %i received; staring cleanup\n", code);
        
        // This is the first fatal signal the process has gotten:
        // * restore our signal handlers to the defaults (as we're about
        //   to destroy the state that exception handling relies on)
        // * unblock the current signal
        // * cleanup the process
        // * re-raise the signal to cause the process to exit
        
        SEHCleanupSignals();

        sigemptyset(&signal_set);
        sigaddset(&signal_set, code);
        if(-1 == sigprocmask(SIG_UNBLOCK, &signal_set, NULL))
        {
            ASSERT("sigprocmask failed; error is %d (%s)\n", errno, strerror(errno));
        }

        PROCCleanupProcess(TRUE);

        TRACE("Re-raising fatal signal to kill process\n");

        raise(code);
    }
    else
    {
        // If another signal handler has already started the cleanup do nothing
        TRACE("Fatal signal %i received; ignoring as cleanup already in progress\n", code);
    }
}

/*++
Function :
    common_signal_handler

    common code for all signal handlers

Parameters :
    PEXCEPTION_POINTERS pointers : exception information
    native_context_t *ucontext : context structure given to signal handler
    int code : signal received

    (no return value)
Note:
    the "pointers" parameter should contain a valid exception record pointer, 
    but the contextrecord pointer will be overwritten.    
--*/
static void common_signal_handler(PEXCEPTION_POINTERS pointers, int code, 
                                  native_context_t *ucontext)
{
    sigset_t signal_set;
    CONTEXT context;

    // Fill context record with required information. from rotor_pal.h :
    // On non-Win32 platforms, the CONTEXT pointer in the
    // PEXCEPTION_POINTERS will contain at least the CONTEXT_CONTROL registers.
    CONTEXTFromNativeContext(ucontext, &context,
                             CONTEXT_CONTROL | CONTEXT_INTEGER);

    pointers->ContextRecord = &context;

    /* Unmask signal so we can receive it again */
    sigemptyset(&signal_set);
    sigaddset(&signal_set,code);
    if(-1 == sigprocmask(SIG_UNBLOCK,&signal_set,NULL))
    {
        ASSERT("sigprocmask failed; error is %d (%s)\n",errno, strerror(errno));
    } 

    /* see if we can safely try to handle the exception. we can't if the signal
       occurred while looking for an exception handler, because this would most 
       likely result in infinite recursion */
    if(SEHGetSafeState())
    {
        // Indicate the thread is no longer safe to handle signals.
        SEHSetSafeState(FALSE);
        SEHRaiseException(pointers, code);

        // SEHRaiseException may have returned before resetting the safe 
        // state; do it here.
        SEHSetSafeState(TRUE);

        // Use sigreturn, in case the exception handler has changed some
        // registers. sigreturn allows us to set the context and terminate
        // the signal handling.
        CONTEXTToNativeContext(&context, ucontext,
                               CONTEXT_CONTROL | CONTEXT_INTEGER);

#if HAVE_SIGRETURN
        sigreturn(ucontext);
#elif HAVE__THREAD_SYS_SIGRETURN
        _thread_sys_sigreturn(ucontext);
#elif HAVE_SETCONTEXT
        setcontext(ucontext);
#else
#error Missing a sigreturn equivalent on this platform!
#endif
        ASSERT("sigreturn has returned, it should not.\n");
    }
    else
    {
        /* signal was received while in an unsafe mode. we were already 
           handling a signal when we got this one, and trying to handle it 
           again would most likely result in the signal being triggered again 
           (infinite recursion). abort. */
        ERROR("got a signal during unsafe portion of exception handling. "
              "aborting\n")
        ExitProcess(pointers->ExceptionRecord->ExceptionCode);
    }
}

/*++
Function :
    handle_signal

    register handler for specified signal

Parameters :
    int signal_id : signal to handle
    SIGFUNC sigfunc : signal handler

    (no return value)
    
note : if sigfunc is NULL, the default signal handler is restored    
--*/
void handle_signal(int signal_id, SIGFUNC sigfunc)
{
    struct sigaction act;

    act.sa_flags = SA_RESTART;

    if( NULL == sigfunc )
    {
        act.sa_handler=SIG_DFL;
#if HAVE_SIGINFO_T
        act.sa_sigaction=NULL;
#endif  /* HAVE_SIGINFO_T */
    }
    else
    {
#if HAVE_SIGINFO_T
        act.sa_handler=NULL;
        act.sa_sigaction=sigfunc;
        act.sa_flags |= SA_SIGINFO;
#else   /* HAVE_SIGINFO_T */
        act.sa_handler = SIG_DFL;
#endif  /* HAVE_SIGINFO_T */
    }
    sigemptyset(&act.sa_mask);

    if(-1==sigaction(signal_id,&act,NULL))
    {
        ASSERT("sigaction() call failed with error code %d (%s)\n",
              errno, strerror(errno));
    }
}
#endif // !HAVE_MACH_EXCEPTIONS
