/*=============================================================
**
** Source: helper.c
**
** Purpose: A child process which will attempt to read and write to files
** which were locked in the parent.  It will also lock another region of the
** same file.
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

#define FILENAME "testfile.txt"
#define WAITFILENAME "waitfile"
#define BUF_SIZE 128

int __cdecl main(int argc, char *argv[])
{
    HANDLE TheFile, WaitFile;
    int result = 0;
    char DataBuffer[BUF_SIZE];
    DWORD BytesRead;
    
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    
    /* Open the same file that the parent has opened and locked */
    TheFile = CreateFile(FILENAME,     
                         GENERIC_READ|GENERIC_WRITE, 
                         FILE_SHARE_READ|FILE_SHARE_WRITE,
                         NULL,     
                         OPEN_EXISTING,                 
                         FILE_ATTRIBUTE_NORMAL, 
                         NULL);
    
    if (TheFile == INVALID_HANDLE_VALUE) 
    { 
        Trace("ERROR: Could not open file '%s' with CreateFile.",FILENAME); 
        result = 1;
    }
    
    /* Open up the WaitFile that we're using for IPC */
    WaitFile = CreateFile(WAITFILENAME,     
                          GENERIC_READ|GENERIC_WRITE, 
                          FILE_SHARE_READ|FILE_SHARE_WRITE,
                          NULL,                          
                          OPEN_ALWAYS,                 
                          FILE_ATTRIBUTE_NORMAL, 
                          NULL);
    
    if (WaitFile == INVALID_HANDLE_VALUE) 
    { 
        Trace("ERROR: Could not open file '%s' with CreateFile. "
             "GetLastError() returned %d.",WAITFILENAME,GetLastError()); 
        result = 1;
    }
    
    /* Lock the same file that the parent process locked, but the child
       locks bytes 11 through 20
    */

    if(LockFile(TheFile, 11, 0, 10, 0) == 0)
    {
        Trace("ERROR: LockFile failed in the child proccess.  "
              "GetLastError returns %d.",
              GetLastError());
        result = 1;
    }
    
    /* Check to ensure the parent lock is respected */
    if(ReadFile(TheFile, DataBuffer, 10, &BytesRead, NULL) != 0)
    {
        Trace("ERROR: ReadFile returned success when it should "
             "have failed.  Attempted to read the first 10 bytes "
             "of a file which was locked by the parent process.");
        result = 1;
    }

    /* Check to ensure the lock put on by this proccess doesn't restrict
       access
    */

    if(SetFilePointer(TheFile, 11, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        Trace("ERROR: SetFilePointer was unable to move the file pointer to "
              "the 11th byte in the file, within the child proccess.  "
              "GetLastError() returned %d.",GetLastError());
        result = 1;
    }

    if(ReadFile(TheFile, DataBuffer, 10, &BytesRead, NULL) == 0)
    {
        Trace("ERROR: ReadFile failed when attempting to read a section of "
             "the file which was locked by the current process.  It should "
             "have been able to read this.  GetLastError() returned %d.",
             GetLastError());
        result = 1;
    }

    // Sleep for a bit to give the parent a chance to block before we do.
    Sleep(1000);

    /* Switch back to the parent, so it can check the child's locks */
    SignalAndBusyWait(WaitFile);

    if(UnlockFile(TheFile, 11, 0, 10, 0) == 0)
    {
        Fail("ERROR: Failed to Unlock bytes 11-20 in the file.  "
             "GetLastError returned %d.",GetLastError());
    }
    
    PAL_Terminate();
    return result;
}
