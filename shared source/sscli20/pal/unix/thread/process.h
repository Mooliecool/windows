/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    thread/process.h

Abstract:

    Process object structure definition.

Revision History:

--*/

#ifndef _PAL_PROCESS_H_
#define _PAL_PROCESS_H_

#include "pal/palinternal.h"
#include "pal/handle.h"
#include "pal/module.h"
#include "pal/virtual.h"
#include "pal/thread.h"
#include "pal/shmemory.h"
#include <pthread.h>


typedef enum
{
    PS_STARTING,
    PS_RUNNING,
    PS_DONE
} PROCESS_STATE;

typedef struct _INITIALPROCINFO
{
    LPWSTR lpwstrCmdLine;
    WCHAR  lpwstrCurDir[MAX_PATH];
} INITIALPROCINFO;

typedef struct SHMPROCESS SHMPROCESS;

typedef struct _PROCESS
{
    HOBJSTRUCT   objHeader;
    HANDLE       hProcess;
    DWORD        processId;
    DWORD        refCount;
    CRITICAL_SECTION critSection;
    INITIALPROCINFO  *lpInitProcInfo;
    SHMPROCESS *shmprocess; /* SHMPROCESS object associated to this process */
    PROCESS_STATE state;
    DWORD         exit_code;
} PROCESS;
extern PROCESS *pCurrentProcess;

/* to be placed in shared memory for use by debugger */
struct SHMPROCESS
{
    PROCESS *info;
    DWORD processId;

    int refcount; /* number of PROCESS objects open for this process */

    /* necessary for Read/WriteProcessMemory, so must be in shared memory */
    DWORD attachedByProcId;
    DWORD attachCount;
    SHMPTR self;  
    SHMPTR next;
} ;

/*++
Function:
  PROCAddThread

Abstract
  Add a thread to the thread list of the current process
--*/
BOOL PROCAddThread(THREAD *lpThread);

extern THREAD *pGThreadList;

/*++
Function:
  PROCRemoveThread

Abstract
  Remove a thread form the thread list of the current process
--*/
void PROCRemoveThread(THREAD *lpthread);

/*++
Function:
  PROCGetNumberOfThreads

Abstract
  Return the number of threads in the thread list.
--*/
INT PROCGetNumberOfThreads(void);

/*++
Function:
  PROCFindSHMProcess

Abstract
  Leave the critical section associated to the current process
--*/
SHMPROCESS *PROCFindSHMProcess(DWORD dwProcessId);

/*++
Function:
  TerminateCurrentProcessNoExit

Parameters:
    BOOL bTerminateUnconditionally - If this is set, the PAL will exit as
    quickly as possible. In particular, it will not unload DLLs.

Abstract:
    Terminate Current Process, but leave the caller alive
--*/
void TerminateCurrentProcessNoExit(BOOL bTerminateUnconditionally);
#endif //PAL_PROCESS_H_


