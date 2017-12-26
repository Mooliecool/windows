/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    machexception.c

Abstract:

    Implementation of MACH exception API functions.

--*/

#include "pal/palinternal.h"
#if HAVE_MACH_EXCEPTIONS
#include "pal/dbgmsg.h"
#include "pal/thread.h"
#include "pal/critsect.h"
#include "pal/seh.h"
#include "pal/debug.h"
#include "pal/init.h"
#include "pal/utils.h"
#include "machexception.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <architecture/ppc/cframe.h>

SET_DEFAULT_DEBUG_CHANNEL(EXCEPT);

#if !DISABLE_EXCEPTIONS
// System exception delegate function
boolean_t exc_server(mach_msg_header_t *request_msg,  mach_msg_header_t *reply_ms);

// The port we use to handle exceptions and to set the thread context
static mach_port_t s_ExceptionPort;
static BOOL s_DebugInitialized = FALSE;

#endif

/*++
Function :
    SEHCleanupExceptionPort

    Restore default exception port handler

    (no parameters, no return value)
    
Note :
During PAL_Terminate, we reach a point where SEH isn't possible any more
(handle manager is off, etc). Past that point, we can't avoid crashing on
an exception.
--*/
void SEHCleanupExceptionPort (void)
{
#if !DISABLE_EXCEPTIONS
    kern_return_t MachRet;

    TRACE("Restoring default exception ports\n");
    // Set the port that listens to exceptions for this task.
    MachRet = task_set_exception_ports(mach_task_self(),
                                       PAL_EXC_MASK | (s_DebugInitialized ? PAL_EXC_DEBUGGING_MASK : 0),
                                       MACH_PORT_NULL,
                                       EXCEPTION_DEFAULT,
                                       PPC_THREAD_STATE);
    if (MachRet != KERN_SUCCESS)
    {
       UTIL_SetLastErrorFromMach(MachRet);
    }

    s_DebugInitialized = FALSE;
#endif // !DISABLE_EXCEPTIONS
}

#if !DISABLE_EXCEPTIONS

extern BOOL CONTEXT_GetThreadContextFromPort(mach_port_t Port, LPCONTEXT lpContext);
extern BOOL CONTEXT_SetThreadContextOnPort(mach_port_t Port, LPCONTEXT lpContext);

void PAL_DispatchException(PCONTEXT pContext, PEXCEPTION_RECORD pExRecord)
{

    MSG_SET_THREAD MsgSet;
    kern_return_t MachRet;
    EXCEPTION_POINTERS pointers;

    pointers.ExceptionRecord = pExRecord;
    pointers.ContextRecord = pContext;

    // Raise the exception
    SEHRaiseException(&pointers, 0);

    // We need to send a message to the worker thread so that it can set our thread context

    // Setup the heaer
    MsgSet.m_MsgHdr.msgh_size = sizeof(MsgSet);
    MsgSet.m_MsgHdr.msgh_bits = MACH_MSGH_BITS_REMOTE(MACH_MSG_TYPE_MAKE_SEND|MACH_MSG_TYPE_MOVE_RECEIVE); // Simple Message
    MsgSet.m_MsgHdr.msgh_remote_port = s_ExceptionPort; // Send, dest port
    MsgSet.m_MsgHdr.msgh_local_port = MACH_PORT_NULL;   // We're not expecting a msg back
    MsgSet.m_MsgHdr.msgh_id = SET_THREAD_MESSAGE_ID; // Message ID
    MsgSet.m_MsgHdr.msgh_reserved = 0; // Not used

    // Setup the thread and thread context
    MsgSet.m_ThreadPort = mach_thread_self();
    MsgSet.m_ThreadContext = *pContext;

    // Send the  message to the exception port
    MachRet = mach_msg(&MsgSet.m_MsgHdr,
                       MACH_SEND_MSG,
                       MsgSet.m_MsgHdr.msgh_size,
                       0,
                       MACH_PORT_NULL,
                       MACH_MSG_TIMEOUT_NONE,
                       MACH_PORT_NULL);

    if (MachRet != KERN_SUCCESS)
    {
        UTIL_SetLastErrorFromMach(MachRet);
        ExitProcess(GetLastError());
    }

    // Make sure we don't do anything
    while(1)
    {
        sched_yield();
    }
}

/*++
Function :
    exception_from_trap_code

    map a Trap code to a SEH exception code

Parameters :
    exception_type_t exception          : Trap code to map
    exception_data_t code               : Sub code
    mach_msg_type_number_t code_count   : Size of sub code
*/
static DWORD exception_from_trap_code(
   exception_type_t exception,          // [in] The type of the exception
   exception_data_t code,               // [in] A machine dependent array indicating a particular instance of exception
   mach_msg_type_number_t code_count)   // [in] The size of the buffer (in natural-sized units). 
{

    switch(exception)
    {
    // Could not access memory. subcode contains the bad memory address. 
    case EXC_BAD_ACCESS:
        return EXCEPTION_ACCESS_VIOLATION; 
        break;

    // Instruction failed. Illegal or undefined instruction or operand. 
    case EXC_BAD_INSTRUCTION :
        return EXCEPTION_ILLEGAL_INSTRUCTION; 
        break;

    // Arithmetic exception; exact nature of exception is in subcode field. 
    case EXC_ARITHMETIC:
        if (code_count != 2)
        {
            ASSERT("Got an unexpected sub code");
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        }
        switch (*(unsigned *)code)
        {
        case EXC_PPC_OVERFLOW:
            return EXCEPTION_INT_OVERFLOW;
        case EXC_PPC_ZERO_DIVIDE:
            return EXCEPTION_INT_DIVIDE_BY_ZERO;
        case EXC_PPC_FLT_INEXACT:
            return EXCEPTION_FLT_INEXACT_RESULT;
        case EXC_PPC_FLT_ZERO_DIVIDE:
            return EXCEPTION_FLT_DIVIDE_BY_ZERO;
        case EXC_PPC_FLT_UNDERFLOW:
            return EXCEPTION_FLT_UNDERFLOW;
        case EXC_PPC_FLT_OVERFLOW:
            return EXCEPTION_FLT_OVERFLOW;
        case EXC_PPC_FLT_NOT_A_NUMBER:
	    return EXCEPTION_ILLEGAL_INSTRUCTION;
        default:
            return EXCEPTION_ILLEGAL_INSTRUCTION; 
        }
        break;

    // Software generated exception; exact exception is in subcode field. Codes 0 - 0xFFFF reserved to hardware; codes 0x10000 - 0x1FFFF reserved for OS emulation. 
    case EXC_SOFTWARE:
        if (*(unsigned *)code == EXC_PPC_TRAP)
        {
            return EXCEPTION_BREAKPOINT;
        }        
        return EXCEPTION_ILLEGAL_INSTRUCTION; 
        break;

    // Trace, breakpoint, etc. Details in subcode field. 
    case EXC_BREAKPOINT:
        if (*(unsigned *)code == EXC_PPC_TRACE)
        {
            return EXCEPTION_SINGLE_STEP;
        }
        else
        {
            return EXCEPTION_BREAKPOINT;
        }
        break;


    // System call requested. Details in subcode field. 
    case EXC_SYSCALL:
        return EXCEPTION_ILLEGAL_INSTRUCTION; 
        break;

    // System call with a number in the Mach call range requested. Details in subcode field. 
    case EXC_MACH_SYSCALL:
        return EXCEPTION_ILLEGAL_INSTRUCTION; 
        break;

    default:
        ASSERT("Got unknown trap code %d\n", exception);
        break;
    }
    return EXCEPTION_ILLEGAL_INSTRUCTION;
}

/*++
Function :
    catch_exception_raise

    called from exc_server and does the exception processing

Return value :
   KERN_SUCCESS if the error is handled
   MIG_DESTROY_REQUEST if the error was not handled
--*/

kern_return_t
catch_exception_raise(
   mach_port_t exception_port,          // [in] Mach Port that is listening to the exception
   mach_port_t thread,                  // [in] thread the exception happened on
   mach_port_t task,                    // [in] task the exception happened on
   exception_type_t exception,          // [in] The type of the exception
   exception_data_t code,               // [in] A machine dependent array indicating a particular instance of exception
   mach_msg_type_number_t code_count)   // [in] The size of the buffer (in natural-sized units). 
{

    kern_return_t MachRet;
    ppc_thread_state_t ThreadState;
    unsigned int count = sizeof(ThreadState)/sizeof(unsigned);

    CONTEXT ThreadContext;

    EXCEPTION_RECORD ExceptionRecord;

    void *FramePointer;

    ThreadContext.ContextFlags = CONTEXT_FULL;



    CONTEXT_GetThreadContextFromPort(thread, &ThreadContext);



    // We need to hijack the thread to point to PAL_DispatchException



    MachRet = thread_get_state(thread,

                               MACHINE_THREAD_STATE,

                               (thread_state_t)&ThreadState,

                               &count);



    if (MachRet != KERN_SUCCESS)

    {

        UTIL_SetLastErrorFromMach(MachRet);

        ExitProcess(GetLastError());

    }



    ExceptionRecord.ExceptionCode = exception_from_trap_code(exception, code, code_count);

    // If we're in single step mode, disable it since we're going to call PAL_DispatchException

    if (ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP)

    {

        ThreadState.srr1 &= ~0x400UL;

    }



    ExceptionRecord.ExceptionFlags = EXCEPTION_IS_SIGNAL; 

    ExceptionRecord.ExceptionRecord = NULL;

    ExceptionRecord.ExceptionAddress = (void *)ThreadContext.Iar;

    ExceptionRecord.NumberParameters = 0;



    FramePointer =(void *) ThreadState.r1;



    // Make sure it's naturally aligned

    FramePointer = FramePointer - ((unsigned)FramePointer % 8);



    // Move it pass the red zone

    FramePointer -= C_RED_ZONE;



    // Put the Context on the stack

    FramePointer -= sizeof(CONTEXT);

    *(CONTEXT *)FramePointer = ThreadContext;

    ThreadState.r3 = (unsigned)FramePointer;



    // Put the exception record on the stack

    FramePointer -= sizeof(EXCEPTION_RECORD);

    *(EXCEPTION_RECORD *)FramePointer = ExceptionRecord;

    ThreadState.r4 = (unsigned)FramePointer;



    // Add room for the args to spill

    FramePointer -= 2*sizeof(void *);



    // Add room for the linking area (24 bytes)

    FramePointer -= 24;

    *(unsigned *)FramePointer = ThreadState.r1;





    // Make the instruction register point to DispatchException

    ThreadState.srr0 =(unsigned) &PAL_DispatchException;

    ThreadState.r1 = (unsigned)FramePointer;


    // Now set the thread state for the faulting thread so that PAL_DispatchException executes next
    MachRet = thread_set_state(thread,
                               MACHINE_THREAD_STATE,
                               (thread_state_t)&ThreadState,
                               count);

    if (MachRet != KERN_SUCCESS)
    {
        UTIL_SetLastErrorFromMach(MachRet);
        ExitProcess(GetLastError());
    }

    // We're done!
    return KERN_SUCCESS ;

}
/*++
Function :
    SEHExceptionThread

    Entry point for the thread that will listen for exception in any other thread.

Parameters :
    void *args - not used

Return value :
   Never returns
--*/
void *SEHExceptionThread(void *args)
{
    mach_msg_return_t MachRet;

    // Handle exceptions forever.
    while (1)
    {
        char buf[512];
        mach_msg_header_t *msg = (mach_msg_header_t *)buf;
        MachRet = mach_msg(msg,
                           MACH_RCV_MSG | MACH_RCV_LARGE,
                           0,
                           sizeof(buf),
                           s_ExceptionPort,
                           MACH_MSG_TIMEOUT_NONE,
                           MACH_PORT_NULL);

        if (MachRet == MACH_RCV_TOO_LARGE)
        {
            UTIL_SetLastErrorFromMach(MachRet);
            ExitProcess(GetLastError());
        }

        if (MachRet != MACH_MSG_SUCCESS)
        {
            UTIL_SetLastErrorFromMach(MachRet);
            ExitProcess(GetLastError());
        }

        if (msg->msgh_id == SET_THREAD_MESSAGE_ID)
        {
            MSG_SET_THREAD *MsgSet = (MSG_SET_THREAD *)msg;

            MachRet = thread_suspend(MsgSet->m_ThreadPort);

            if (MachRet != KERN_SUCCESS)
            {
                UTIL_SetLastErrorFromMach(MachRet);
                ExitProcess(GetLastError());
            }
            if (!CONTEXT_SetThreadContextOnPort(MsgSet->m_ThreadPort, &MsgSet->m_ThreadContext))
            {
                ExitProcess(GetLastError());
            }

            MachRet = thread_resume(MsgSet->m_ThreadPort);
            if (MachRet != KERN_SUCCESS)
            {
                UTIL_SetLastErrorFromMach(MachRet);
                ExitProcess(GetLastError());
            }
        }
        else
        {
            char *MsgReply[512];
            mach_msg_header_t *reply = (mach_msg_header_t *)MsgReply;

            // Pass the message to exc_server which in turn calls catch_exception_raise
            exc_server(msg, reply);

            // Send the reply message back
            MachRet = mach_msg(reply,
                            MACH_SEND_MSG | MACH_MSG_OPTION_NONE,
                            reply->msgh_size,
                            0,
                            MACH_PORT_NULL,
                            MACH_MSG_TIMEOUT_NONE,
                            MACH_PORT_NULL);

            if (MachRet != KERN_SUCCESS)
            {
                UTIL_SetLastErrorFromMach(MachRet);
                ExitProcess(GetLastError());
            }
        }        
    } // Check for more errors

}
#endif // !DISABLE_EXCEPTIONS

/*++
Function :
    SEHInitializeMachExceptions 

    Initialize all SEH-related stuff related to mach exceptions

    (no parameters)

Return value :
    TRUE  if SEH support initialization succeeded
    FALSE otherwise
--*/
BOOL SEHInitializeMachExceptions (void)
{
#if !DISABLE_EXCEPTIONS
    kern_return_t MachRet;
    int CreateRet;
    pthread_t exception_thread;

    // Allocate a mach port that will listen in on exceptions
    MachRet = mach_port_allocate(mach_task_self(),
                                 MACH_PORT_RIGHT_RECEIVE,
                                 &s_ExceptionPort);

    if (MachRet != KERN_SUCCESS)
    {
        UTIL_SetLastErrorFromMach(MachRet);
        return FALSE;
    }

    // Insert the send right into the task
    MachRet = mach_port_insert_right(mach_task_self(),
                                     s_ExceptionPort,
                                     s_ExceptionPort,
                                     MACH_MSG_TYPE_MAKE_SEND);

    if (MachRet != KERN_SUCCESS)
    {
        UTIL_SetLastErrorFromMach(MachRet);
        return FALSE;
    }

    MachRet = task_set_exception_ports(mach_task_self(),
                                       PAL_EXC_MASK ,
                                       s_ExceptionPort,
                                       EXCEPTION_DEFAULT,
                                       PPC_THREAD_STATE);

    if (MachRet != KERN_SUCCESS)
    {
        UTIL_SetLastErrorFromMach(MachRet);
        return FALSE;
    }

    // Create the thread that will listen to the exception for all threads
    CreateRet = pthread_create(&exception_thread, NULL, SEHExceptionThread, NULL);

    if ( CreateRet != 0 )
    {
        ERROR("pthread_create failed, error is %d (%s)\n", CreateRet, strerror(CreateRet));
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
#endif // !DISABLE_EXCEPTIONS

    // Tell the system to ignore SIGPIPE signals rather than use the default
    // behavior of terminating the process. Ignoring SIGPIPE will cause
    // calls that would otherwise raise that signal to return EPIPE instead.
    // The PAL expects EPIPE from those functions and won't handle a
    // SIGPIPE signal.
    signal(SIGPIPE, SIG_IGN);

    // We're done
    return TRUE;
}

/*++
Function :
    MachExceptionInitializeDebug 

    Initialize the mach exception handlers necessary for a managed debugger
    to work

Return value :
    None
--*/
void MachExceptionInitializeDebug(void)
{
#if !DISABLE_EXCEPTIONS
    if (s_DebugInitialized == FALSE)
    {
        kern_return_t MachRet;
        MachRet = task_set_exception_ports(mach_task_self(),
                                           PAL_EXC_DEBUGGING_MASK,
                                           s_ExceptionPort,
                                           EXCEPTION_DEFAULT,
                                           PPC_THREAD_STATE);
        if (MachRet != KERN_SUCCESS)
        {
            TerminateProcess(GetCurrentProcess(), -1);
        }

        s_DebugInitialized = TRUE;
    }
#endif // !DISABLE_EXCEPTIONS
}

#endif // HAVE_MACH_EXCEPTIONS
