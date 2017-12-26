
/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    exception/machexception.h

Abstract:
    Private mach exception handling utilities for SEH

--*/

#ifndef _MACHEXCEPTION_H_
#define _MACHEXCEPTION_H_

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/thread_status.h>

// Process and thread Initialization/Cleanup routines
BOOL SEHInitializeMachExceptions(void);
void SEHCleanupExceptionPort (void);
void MachExceptionInitializeDebug(void);

// List of exception types we will be watching for
#define PAL_EXC_MASK    (EXC_MASK_BAD_ACCESS |          \
                         EXC_MASK_BAD_INSTRUCTION |     \
                         EXC_MASK_ARITHMETIC |          \
                         EXC_MASK_EMULATION )

// List of exception types we will be watching for when
// there's a managed debugger attached
#define PAL_EXC_DEBUGGING_MASK  (EXC_MASK_BREAKPOINT | \
                                 EXC_MASK_SOFTWARE)

#define SET_THREAD_MESSAGE_ID 1
#define HIJACK_ON_SIGNAL 1

typedef struct _MSG_SET_THREAD 
{
  mach_msg_header_t m_MsgHdr;
  CONTEXT m_ThreadContext;
  mach_port_t m_ThreadPort;
} MSG_SET_THREAD;

#endif /* _MACHEXCEPTION_H_ */

