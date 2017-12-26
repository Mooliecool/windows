/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/seh.h

Abstract:
    Header file for public Structured Exception Handling stuff

--*/

#ifndef _PAL_SEH_H_
#define _PAL_SEH_H_

#include "config.h"
#include "pal/palinternal.h"

/*++
Function :
    SEHInitialize

    Initialize all SEH-related stuff (signals, etc)

    (no parameters)

Return value:
    TRUE  if SEH support initialization succeeded,
    FALSE otherwise

--*/
BOOL SEHInitialize(void);

/*++
Function :
    SEHCleanup

    Clean up SEH-related stuff(signals, etc)

    (no parameters, no return value)
--*/
void SEHCleanup(void);

/*++
Function :
    SEHRaiseException

    Raise an exception given a specified exception information.

Parameters :
    PEXCEPTION_POINTERS lpExceptionPointers : specification of exception 
    to raise.
    int signal_code : signal that caused the exception, if applicable; 
                      0 otherwise

    (no return value; function should never return)

Notes :
    The PAL does not support continuing execution after an exception was raised
    (using EXCEPTION_CONTINUE_EXECUTION). For this reason, this function should
    never return.
--*/
void SEHRaiseException( PEXCEPTION_POINTERS lpExceptionPointers, 
                        int signal_code );

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
    installed), the default behavior is to call ExitProcess
--*/
void SEHHandleControlEvent(DWORD event, LPVOID eip);

/*++
Function :
    SEHSetSafeState

    specify whether the current thread is in a state where exception handling 
    of signals can be done safely

Parameters:
    BOOL state : TRUE if the thread is safe, FALSE otherwise

(no return value)
--*/
void SEHSetSafeState(BOOL state);

/*++
Function :
    SEHGetSafeState

    determine whether the current thread is in a state where exception handling 
    of signals can be done safely

    (no parameters)

Return value :
    TRUE if the thread is in a safe state, FALSE otherwise
--*/
BOOL SEHGetSafeState(void);

#endif /* _PAL_SEH_H_ */

