/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/debug.h

Abstract:

    Debug API utility functions 

--*/

#ifndef _PAL_DEBUG_H_
#define _PAL_DEBUG_H_

/*++
Function :
    DBG_DebugBreak

    Processor-dependent implementation of DebugBreak

(no parameters, no return value)
--*/
VOID DBG_DebugBreak();

/*++
Function :
    DBG_FlushInstructionCache

    Processor-dependent implementation of FlushInstructionCache

Parameters :
    LPCVOID lpBaseAddress: start of region to flush
    SIZE_T dwSize : length of region to flush

Return value :
    TRUE on success, FALSE on failure

--*/
BOOL
DBG_FlushInstructionCache(
                      IN LPCVOID lpBaseAddress,
                      IN SIZE_T dwSize);


/*++
Function:
  DBGAttachProcess

Abstract  
  
  Attach the indicated process to the current process. 
  
  if the indicated process is already attached by the current process, then 
  increment the number of attachment pending. if ot, attach it to the current 
  process (with PT_ATTACH).

Parameter
  processId : process ID to attach
Return
  Return true if it succeeds, or false if it's fails
--*/
BOOL 
DBGAttachProcess(DWORD processId);

/*++
Function:
  DBGDetachProcess

Abstract
  Detach the indicated process from the current process.
  
  if the indicated process is already attached by the current process, then 
  decrement the number of attachment pending and detach it from the current 
  process (with PT_DETACH) if there's no more attachment left. 
  
Parameter
  processId : process handle

Return
  Return true if it succeeds, or true if it's fails
--*/
BOOL
DBGDetachProcess(DWORD processId);

#endif //PAL_DEBUG_H_
