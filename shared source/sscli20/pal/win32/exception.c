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

#include "win32pal.h"
#include "perftrace.h"


/*

How the emulation of portable exception handling works
------------------------------------------------------

- overwrites the per-process exception filter (the one set using SetUnhandledExceptionFilter)
with own special exception filter. The special exception filter will first pass the exception
through the per-thread filter chain, and then passes it to the original per-process exception filter.

- assumes that nobody ever installs a per-thread exception filter in the process, or sets 
SetUnhandledExceptionFilter directly - the only allowed way is through the PAL

- Note that RaiseException is not overwritten for the portable exception handling. One of the reason 
is to let it generate the debugger events, the events will not happen in exact same order as with
the real Win32 exception handling though.

*/

#define TRACE 1 ? NULL :

PAL_LPTOP_LEVEL_EXCEPTION_FILTER SEH_CurrentTopLevelFilter = NULL;
DWORD SEH_Tls = TLS_OUT_OF_INDEXES;

/* static helpers *************************************************************/

static PPAL_EXCEPTION_REGISTRATION GetCurrentFrame()
{
    PPAL_EXCEPTION_REGISTRATION pRegistration;
    PALASSERT(SEH_Tls != TLS_OUT_OF_INDEXES);
    pRegistration = (PPAL_EXCEPTION_REGISTRATION)TlsGetValue(SEH_Tls);
    PALASSERT(pRegistration == NULL || (PVOID)pRegistration > (PVOID)&pRegistration);
    return pRegistration;
}

static void SetCurrentFrame(PPAL_EXCEPTION_REGISTRATION pRegistration)
{
    PALASSERT(pRegistration == NULL || (PVOID)pRegistration > (PVOID)&pRegistration);
    TlsSetValue(SEH_Tls, (LPVOID)pRegistration);
}

void RtlUnwindCallback()
{
    PALASSERT(!"Should never get here");
}

WINBASEAPI
VOID
WINAPI
RtlUnwind(PVOID TargetFrame, 
        PVOID TargetIp, 
        PVOID ExceptionRecord, 
        PVOID ReturnValue);

BOOL __declspec(naked)
CallRtlUnwind(PVOID pEstablisherFrame, PVOID callback, PVOID pExceptionRecord, PVOID retVal)
{
#ifndef __GNUC__
    // In the checked version, we compile /GZ to check for imbalanced stacks and
    // uninitialized locals.  But RtlUnwind doesn't restore registers across a
    // call (it's more of a longjmp).  So make the call manually to defeat the
    // compiler's checking.

    // RtlUnwind(pEstablisherFrame, RtlUnwindCallback, pExceptionRecord, retVal);
    __asm
    {
        push    ebp
        mov     ebp, esp
        
        push    ebx             // RtlUnwind trashes EBX, ESI, EDI.
        push    esi             // To avoid confusing VC, save them.
        push    edi
        push    dword ptr [retVal]
        push    dword ptr [pExceptionRecord]
        push    offset RtlUnwindCallback
        push    dword ptr [pEstablisherFrame]

        mov     eax, [RtlUnwind]
        call    eax

        pop     edi
        pop     esi
        pop     ebx
    // on x86 at least, RtlUnwind always returns
        push 1
        pop  eax

        leave
        ret 10h
    }
#else //__GNUC__
    __asm__ __volatile__ (
        "push %%ebx\n\t"
        "push %%esi\n\t"
        "push %%edi\n\t"

        "push %0\n\t"
        "push %1\n\t"
        "push %2\n\t"
        "push %3\n\t"

        "call %4\n\t"

        "pop  %%edi\n\t"
        "pop  %%esi\n\t"
        "pop  %%ebx\n\t"
        :
        : "r" (retVal), "r" (pExceptionRecord), "r" (RtlUnwindCallback), "r" (pEstablisherFrame),
          "r" (RtlUnwind)
        : "eax"
          );

    return TRUE;
#endif // __GNUC__
}

static PVOID GetNearestWin32Frame(PVOID frame)
{
    PVOID pWin32Frame;

#ifdef _MSC_VER
    __asm { mov eax, fs:[0] };
    __asm { mov pWin32Frame, eax };
#else // _MSC_VER
    __asm__ __volatile__ (
        "mov %%fs:0, %0\n\t"
        : "=r" (pWin32Frame)
        );
#endif // _MSC_VER

    while (pWin32Frame < (PVOID)frame) {
        pWin32Frame = *(PVOID*)pWin32Frame;
    }

    return pWin32Frame;
}

void Cpp_RaiseException(PAL_PEXCEPTION_RECORD old_record);

PALIMPORT
void
PALAPI
DoUnwind(DWORD ExceptionCode, PAL_PEXCEPTION_RECORD er)
{
    PPAL_EXCEPTION_REGISTRATION frame;
    PAL_EXCEPTION_POINTERS ep;
    LONG action;

    frame = GetCurrentFrame();

    while(frame)
    {
#ifdef __GNUC__
        if (frame->dwFlags & PAL_EXCEPTION_FLAGS_CPPEHUNWIND) {
            // OK, we need to make gcc unwind upto this point.
            
            frame->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;
            SetCurrentFrame(frame);

            ep.ExceptionRecord = (PAL_PEXCEPTION_RECORD)frame->ReservedForPAL;
            ep.ExceptionRecord->ExceptionFlags |= EXCEPTION_UNWINDING;

            CallRtlUnwind(GetNearestWin32Frame((PVOID)frame), RtlUnwindCallback, ep.ExceptionRecord, 0);

            // Cause a Cpp throw to unwind gcc's handlers upto next managed code block
            Cpp_RaiseException(er);

            // That will not return
        }
        else
#endif //__GNUC__
        if( frame->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDONLY)
        {
            frame->dwFlags &= ~PAL_EXCEPTION_FLAGS_UNWINDTARGET;

            SetCurrentFrame(frame);

            ep.ExceptionRecord = (PAL_PEXCEPTION_RECORD)frame->ReservedForPAL;
            ep.ExceptionRecord->ExceptionFlags |= EXCEPTION_UNWINDING;

            CallRtlUnwind(GetNearestWin32Frame((PVOID)frame), RtlUnwindCallback, ep.ExceptionRecord, 0);

            ep.ContextRecord = NULL;
            action = frame->Handler(&ep, frame->pvFilterParameter);
            PALASSERT(action == EXCEPTION_CONTINUE_SEARCH);
            PALASSERT(!(frame->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET));
        }
        else
        if( frame->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET )
        {
            TRACE("Found unwind target frame (%p)\n", frame);
            break;
        }

        frame = frame->Next;
    }

    if (frame)
    {
        TRACE("Jumping to exception frame %p\n", frame);

        /* remove the target frame from the chain : if there's an exception 
            within the handler, we don't want it to be sent back to the same
            handler */
        SetCurrentFrame(frame->Next);
#ifdef __GNUC__
        CallRtlUnwind(GetNearestWin32Frame((PVOID)frame), RtlUnwindCallback, ep.ExceptionRecord, 0);
#endif

        PALASSERT(er != NULL);

        Cpp_RaiseException(er);
    }

    SetCurrentFrame(NULL);

    ExitProcess(ExceptionCode);
}

/* PAL function definitions ***************************************************/

// On Win32, when a managed exception is thrown, stuff gets added to the fs:[0]
// chain after our Unwind and before ResumeAtJitEHHelper is called. This function
// is called from ResumeAtJitEH just before the Helper is called to do the last
// moment unwind of the fs:[0] chain before we reset the esp.

#ifdef __GNUC__
extern void *__w32_sharedptr;
#endif

PALIMPORT
void 
PALAPI
PAL_DoLastMomentUnwind(IN PVOID target) {
#ifdef _MSC_VER
    CallRtlUnwind(GetNearestWin32Frame(target), RtlUnwindCallback, NULL, 0);
#else
    DWORD dwLasterror;
    unsigned long ulKey;
    void *pvContext;
    int i;

#define FC_KEY_OFFSET 0x28

    PALASSERT(__w32_sharedptr != NULL);

    TRACE("called for target= %p\n", target);

    // Get the TLS key for the frame context
    ulKey = *(unsigned long *)(__w32_sharedptr + FC_KEY_OFFSET);

    // We don't want to change the last error, so save the last error
    dwLasterror = GetLastError();

    // Get the current head of the EH context linked list that gcc keeps
    pvContext = TlsGetValue(ulKey);
    
    // Find the first context below target ESP
    while ((pvContext < target) && (pvContext != NULL)) {
        TRACE("Removing frame at %p\n", pvContext);

        // The prev context is stored at offset 0
        pvContext = *(LPVOID *)pvContext;
    }

    // Set the head of the EH context linked list
    TlsSetValue(ulKey, pvContext);

    // Restore the last error
    SetLastError(dwLasterror);

    // We're done
    return;
#endif // _MSC_VER
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
PALIMPORT
void
PALAPI
PAL_TryHelper(
    IN OUT PPAL_EXCEPTION_REGISTRATION pRegistration)
{
    PPAL_EXCEPTION_REGISTRATION old_frame;
    LOGAPI("PAL_TryHelper(pRegistration=%p)\n", pRegistration);

    PALASSERT(pRegistration != NULL);

    PALASSERT(pRegistration->dwFlags == 0 ||
              pRegistration->dwFlags == PAL_EXCEPTION_FLAGS_UNWINDONLY ||
              pRegistration->dwFlags == PAL_EXCEPTION_FLAGS_LONGJMP ||
              pRegistration->dwFlags == PAL_EXCEPTION_FLAGS_CPPEHUNWIND);

    old_frame = GetCurrentFrame();

    // The frames addresses does not have to be in descenting order - the optimizing compiler 
    // can reorder the frames if there are two or more nested handlers in one function.
    // PALASSERT(old_frame == NULL || pRegistration < old_frame);

    pRegistration->Next = old_frame;

    SetCurrentFrame(pRegistration);

    LOGAPI("PAL_TryHelper returns void\n");
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
PALIMPORT
int
PALAPI
PAL_EndTryHelper(
    IN OUT PPAL_EXCEPTION_REGISTRATION pRegistration,
    IN int ExceptionCode)
{
    PPAL_EXCEPTION_REGISTRATION next_frame;

    LOGAPI("PAL_EndTryHelper(pRegistration=%p, ExceptionCode=0x%08x)\n",
          pRegistration, ExceptionCode);

    PALASSERT(pRegistration != NULL);

    if( ExceptionCode == 0 )
    {
        /* If ExceptionCode is zero, there was no exception, so we
           only need to pop the exception frame. */
        PALASSERT(pRegistration == GetCurrentFrame());
        next_frame = pRegistration->Next;

        TRACE("No exceptions encountered in frame %p. Lowest frame "
              "is now %p\n", pRegistration, next_frame);

        SetCurrentFrame(next_frame);
    }
    else if( pRegistration->dwFlags & PAL_EXCEPTION_FLAGS_UNWINDTARGET )
    {
        /* Stop unwinding if this is the target frame */

        /* If there was an exception, the current frame has already been popped 
           before the longjmp */

        PALASSERT(pRegistration->Next == GetCurrentFrame());

        TRACE("Exception handled by frame %p. Lowest frame is now "
              "%p\n", pRegistration, pRegistration->Next);
    }
    else
    {
        TRACE("Unwinding in progress; looking for next frame\n");

        DoUnwind(ExceptionCode, NULL);
        PALASSERT(FALSE);
    }

    LOGAPI("PAL_EndTryHelper returns\n");
    return 0;
}

/*++
Function:
  PAL_GetBottommostRegistration

Abstract:
  Return a pointer to the first PAL_EXCEPTION_REGISTRATION structure on the
  current thread's stack.
--*/
PALIMPORT
PPAL_EXCEPTION_REGISTRATION
PALAPI
PAL_GetBottommostRegistration()
{
    PPAL_EXCEPTION_REGISTRATION pRegistration;
    LOGAPI("PAL_GetBottommostRegistration()\n");

    pRegistration = GetCurrentFrame();

    LOGAPI("PAL_GetBottommostRegistration returns PPAL_EXCEPTION_REGISTRATION %p\n", 
        pRegistration);
    return pRegistration;
}

/*++
Function:
  PAL_SetBottommostRegistration

Abstract:
  Set a pointer to the first PAL_EXCEPTION_REGISTRATION structure on the
  current thread's stack.
--*/
PALIMPORT
VOID
PALAPI
PAL_SetBottommostRegistration(
    PPAL_EXCEPTION_REGISTRATION pRegistration)
{
    LOGAPI("PAL_SetBottommostRegistration(%p)\n", pRegistration);

    SetCurrentFrame(pRegistration);

    LOGAPI("PAL_SetBottommostRegistration returns\n");
}

/*++
Function:
  PAL_GetBottommostRegistrationPtr

Abstract:
  Return a pointer to storage of the pointer to the first PAL_EXCEPTION_REGISTRATION 
  structure for the current thread.
--*/
PALIMPORT
PPAL_EXCEPTION_REGISTRATION *
PALAPI
PAL_GetBottommostRegistrationPtr()
{
    PPAL_EXCEPTION_REGISTRATION * ppRegistration;
    PVOID pTEB;

    LOGAPI("PAL_GetBottommostRegistrationPtr()\n");

#define PcTeb 0x18
#ifdef _MSC_VER
    __asm {
        mov eax, dword ptr fs:[PcTeb] // TEB->Self
        mov pTEB, eax
    }
#else // _MSC_VER
    __asm__ __volatile__ (
        "mov %%fs:0x18, %0\n\t"
        "mov 0(%0), %0\n\t"
        : "=r" (pTEB)
        );
#endif // _MSC_VER

#define WINNT_TLS_OFFSET    0xe10     // TLS[0] at fs:[WINNT_TLS_OFFSET]
    ppRegistration = (PPAL_EXCEPTION_REGISTRATION*)((BYTE*)pTEB + WINNT_TLS_OFFSET + sizeof(PVOID) * SEH_Tls);

    LOGAPI("PAL_GetBottommostRegistrationPtr returns PPAL_EXCEPTION_REGISTRATION * %p\n", 
        ppRegistration);

    return ppRegistration;
}

LONG PAL_RunFilters(PAL_PEXCEPTION_POINTERS pException);

/* Internal function definitions **********************************************/

/*++
Function :
    PAL_ExceptionFilter

    Raise an exception given a specified exception information.

Parameters :
    PEXCEPTION_POINTERS lpExceptionPointers : specification of exception 
    to raise.

(no return value)
--*/

LONG 
PALAPI
PAL_ExceptionFilter(
    PAL_PEXCEPTION_POINTERS lpExceptionPointers)
{
    PAL_LPTOP_LEVEL_EXCEPTION_FILTER top_filter;
    PPAL_EXCEPTION_REGISTRATION frame;
    PAL_LPCONTEXT context;
    LONG default_action;
    LONG action;

    PALASSERT(lpExceptionPointers != NULL);
    PALASSERT(lpExceptionPointers->ExceptionRecord != NULL);

    TRACE("Raising exception %08x (record is %p)\n",
          lpExceptionPointers->ExceptionRecord->ExceptionCode, 
          lpExceptionPointers);

   context = lpExceptionPointers->ContextRecord;

    /* Call exception filters until one returns EXCEPTION_EXECUTE_HANDLER */

    TRACE("Looking for exception handler...\n");

    action = PAL_RunFilters(lpExceptionPointers);

    TRACE("Back from PAL_RunFilters...\n");

    switch (action) {
        case EXCEPTION_EXECUTE_HANDLER:
            TRACE("Calling DoUnwind\n");
            DoUnwind(lpExceptionPointers->ExceptionRecord->ExceptionCode, lpExceptionPointers->ExceptionRecord);
            TRACE("Back from DoUnwind\n");
            PALASSERT(FALSE);
            return EXCEPTION_CONTINUE_SEARCH;

        case EXCEPTION_CONTINUE_EXECUTION:
            return EXCEPTION_CONTINUE_EXECUTION;

        case EXCEPTION_CONTINUE_SEARCH:
            break;
    }

    /* No handler found : start default handling */

    TRACE("No handler found for exception. Using default behavior.\n");

    /* Call application-defined top-level filter*/
    top_filter = SEH_CurrentTopLevelFilter;

    /* avoid reentrancy from the global system filter */
    SetUnhandledExceptionFilter(NULL);

    if(top_filter)
    {
        TRACE("Calling application-defined top-level filter\n");

        default_action = top_filter(lpExceptionPointers);
        switch( default_action )
        {
        case EXCEPTION_CONTINUE_SEARCH:
            default_action = UnhandledExceptionFilter((PEXCEPTION_POINTERS)lpExceptionPointers);
            break;
        case EXCEPTION_EXECUTE_HANDLER:
            break;
        default:
            // Application-defined top-level exception filter returned invalid value
            PALASSERT(FALSE);
            default_action = EXCEPTION_CONTINUE_SEARCH;
            break;
        }
    }
    else
    {
        default_action = EXCEPTION_CONTINUE_SEARCH;
    }

    TRACE("Reached top of exception stack! Exception was not handled, "
        "terminating the process.\n");

    DoUnwind(lpExceptionPointers->ExceptionRecord->ExceptionCode, lpExceptionPointers->ExceptionRecord);
    PALASSERT(FALSE);
    return EXCEPTION_CONTINUE_SEARCH;
}



typedef
EXCEPTION_DISPOSITION
(*PEXCEPTION_ROUTINE) (
    IN struct _EXCEPTION_RECORD *ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT struct _CONTEXT *ContextRecord,
    IN OUT PVOID DispatcherContext
    );

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD;

#define EXCEPTION_UNWINDING 0x2
#define EXCEPTION_EXIT_UNWIND 0x4
#define EXCEPTION_STACK_INVALID 0x8
#define EXCEPTION_NESTED_CALL 0x10
#define EXCEPTION_TARGET_UNWIND 0x20
#define EXCEPTION_COLLIDED_UNWIND 0x40

#define EXCEPTION_UNWIND (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND | \
                          EXCEPTION_TARGET_UNWIND | EXCEPTION_COLLIDED_UNWIND)

#define IS_UNWINDING(Flag) ((Flag & EXCEPTION_UNWIND) != 0)

EXCEPTION_DISPOSITION
PAL_ExceptionHandler(
    struct _EXCEPTION_RECORD *ExceptionRecord,
    PVOID EstablisherFrame,
    OUT struct _CONTEXT *ContextRecord,
    OUT PVOID DispatcherContext) 
{
    EXCEPTION_POINTERS ep;

    if (!IS_UNWINDING(ExceptionRecord->ExceptionFlags))
    {
        ep.ExceptionRecord = ExceptionRecord;
        ep.ContextRecord = ContextRecord;

        if (PAL_ExceptionFilter((PAL_PEXCEPTION_POINTERS)&ep) 
                == EXCEPTION_CONTINUE_EXECUTION) {
            return ExceptionContinueExecution;
        }
    }

    return ExceptionContinueSearch;
}

#ifdef _MSC_VER
#pragma warning(disable:4733)
#endif

PALIMPORT
DWORD_PTR
PALAPI
PAL_EntryPoint(
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter)
{
    DWORD_PTR Ret;
    EXCEPTION_REGISTRATION_RECORD er;
    
    er.Handler = PAL_ExceptionHandler;

#ifdef _MSC_VER
    __asm {
        __asm lea edx, er.Next
        __asm mov eax, fs:[0]
        __asm mov [edx], eax
        __asm mov fs:[0], edx
    }
#else // _MSC_VER
    __asm__ __volatile__ (
        "mov %0, %%edx\n\t"
        "mov %%fs:0, %%eax\n\t"
        "mov %%eax, 0(%%eax)\n\t"
        "mov %%edx, %%fs:0"
        :
        : "r" (&er.Next)
        : "edx", "eax"
            );
#endif // _MSC_VER


    // __try
    {
        Ret = (*lpStartAddress)(lpParameter);
    }
    // __except(PAL_ExceptionFilter((PAL_PEXCEPTION_POINTERS)GetExceptionInformation()))
    // {
    // }

#ifdef _MSC_VER
    __asm {
        __asm lea edx, er.Next
        __asm mov eax, [edx]
        __asm mov fs:[0], eax
    };
#else // _MSC_VER
    __asm__ __volatile__ (
        "mov 0(%0), %%eax\n\t"
        "mov %%eax, %%fs:0\n\t"
        :: "r" (&er.Next)
        : "eax"
        );
#endif // _MSC_VER

    return  Ret;
}
#ifdef _MSC_VER
#pragma warning(default:4733)
#endif

