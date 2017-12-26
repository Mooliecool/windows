/*=============================================================
**
** Source: test3.c
**
** Purpose: Open a file, lock a region in the middle.  Create a new process
** and attempt to read and write directly before and after that region, which
** should succeed.  Also, check to see that reading/writing in the locked
** region fails.
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**============================================================*/

#include <palsuite.h>
#include "../lockfile.h"

#define HELPER "helper"
#define FILENAME "testfile.txt"

int __cdecl main(int argc, char *argv[])
{
    
    HANDLE TheFile = NULL;
    DWORD FileStart = 0;
    DWORD FileEnd = 0;
    char* WriteBuffer = "12345678901234567890123456"; 
    
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    
    /* Call the helper function to Create a file, write 'WriteBuffer' to
       the file, and lock the file.
    */
    
    FileEnd = strlen(WriteBuffer);
    TheFile = CreateAndLockFile(TheFile,FILENAME, WriteBuffer, 
                                FileStart+3, FileEnd-6);
    
    
    /* Launch another process, which will attempt to read and write from
       the locked file.
       
       If the helper program returns 1, then the test fails. More 
       specific errors are given by the Helper file itself.
    */
    if(RunHelper(HELPER))
    {
        Fail("ERROR: The Helper program determined that the file was not "
             "locked properly by LockFile.");
    }

    if(UnlockFile(TheFile, FileStart+3, 0, FileEnd-6, 0) == 0)
    {
        Fail("ERROR: UnlockFile failed.  GetLastError returns %d.",
             GetLastError());
    }
    
    if(CloseHandle(TheFile) == 0)
    {
        Fail("ERROR: CloseHandle failed to close the file. "
             "GetLastError() returned %d.",GetLastError());
    }
    
    PAL_Terminate();
    return PASS;
}
