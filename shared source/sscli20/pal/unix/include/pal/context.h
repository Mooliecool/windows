/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/context.h

Abstract:

    Header file for thread context utility functions.

--*/

#ifndef _PAL_CONTEXT_H_
#define _PAL_CONTEXT_H_

#if !HAVE_MACH_EXCEPTIONS
/* A type to wrap the native context type, which is ucontext_t on some
 * platforms and another type elsewhere. */
#if HAVE_UCONTEXT_T
#include <ucontext.h>

typedef ucontext_t native_context_t;
#else   // HAVE_UCONTEXT_T
#error Native context type is not known on this platform!
#endif  // HAVE_UCONTEXT_T
#endif  // !HAVE_MACH_EXCEPTIONS

/*++
Function :
    CONTEXT_SetThreadContext

    Processor-dependent implementation of SetThreadContext

Parameters :
    HANDLE hThread : thread whose context is to be set
    CONTEXT *lpContext : new context

Return value :
    TRUE on success, FALSE on failure

--*/
BOOL CONTEXT_SetThreadContext(HANDLE hThread, CONST CONTEXT *lpContext);


/*++
Function :
    CONTEXT_GetThreadContext

    Processor-dependent implementation of GetThreadContext

Parameters :
    HANDLE hThread : thread whose context is to retrieved
    LPCONTEXT lpContext  : destination for thread's context

Return value :
    TRUE on success, FALSE on failure

--*/
BOOL CONTEXT_GetThreadContext(HANDLE hThread, LPCONTEXT lpContext);

#if !HAVE_MACH_EXCEPTIONS
/*++
Function :
    CONTEXTToNativeContext
    
    Converts a CONTEXT record to a native context.

Parameters :
    CONST CONTEXT *lpContext : CONTEXT to convert
    native_context_t *native : native context to fill in
    ULONG contextFlags : flags that determine which registers are valid in
                         lpContext and which ones to set in native

Return value :
    None

--*/
void CONTEXTToNativeContext(CONST CONTEXT *lpContext, native_context_t *native,
                            ULONG contextFlags);

/*++
Function :
    CONTEXTFromNativeContext
    
    Converts a native context to a CONTEXT record.

Parameters :
    const native_context_t *native : native context to convert
    LPCONTEXT lpContext : CONTEXT to fill in
    ULONG contextFlags : flags that determine which registers are valid in
                         native and which ones to set in lpContext

Return value :
    None

--*/
void CONTEXTFromNativeContext(const native_context_t *native, LPCONTEXT lpContext,
                              ULONG contextFlags);

/*++
Function :
    CONTEXTGetPC
    
    Returns the program counter from the native context.

Parameters :
    const native_context_t *context : native context

Return value :
    The program counter from the native context.

--*/
LPVOID CONTEXTGetPC(const native_context_t *context);

/*++
Function :
    CONTEXTGetExceptionCodeForSignal
    
    Translates signal and context information to a Win32 exception code.

Parameters :
    const siginfo_t *siginfo : signal information from a signal handler
    const native_context_t *context : context information

Return value :
    The Win32 exception code that corresponds to the signal and context
    information.

--*/
DWORD CONTEXTGetExceptionCodeForSignal(const siginfo_t *siginfo,
                                       const native_context_t *context);

#endif  // !HAVE_MACH_EXCEPTIONS

#endif  // _PAL_CONTEXT_H_
