/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    VerifyThread.c

Abstract:

    Verify that the thread isn't in the current process

--*/

#include <win32pal.h>

/*++

Routine Description:
    Ensure that the given thread handle isn't in the current process.
    Return TRUE if not in current process or is the current thread.
      FALSE otherwise
--*/
int VerifyThreadNotInCurrentProcess(HANDLE hThread)
{
    return TRUE;
}


/*++

Routine Description:
    Ensure that the given thread handle is in the current process.
    Return TRUE if in current process, FALSE otherwise.
--*/
int VerifyThreadInCurrentProcess(HANDLE hThread)
{
    return TRUE;
}
