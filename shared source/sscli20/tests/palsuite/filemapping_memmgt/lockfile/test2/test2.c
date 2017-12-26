/*=============================================================
**
** Source: test2.c
**
** Purpose: Open a file, and lock it from start to EOF.  Check to ensure
** the current process can still read and write from/to the file.
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

int __cdecl main(int argc, char *argv[])
{
    
    HANDLE TheFile = NULL;
    DWORD FileStart = 0;
    DWORD FileEnd = 0;
    DWORD BytesWritten = 0;
    DWORD BytesRead = 0;
    char WriteBuffer[] = "This is some test data.";
    char DataBuffer[128];

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* Call the helper function to Create a file, write 'WriteBuffer' to
       the file, and lock the file.
    */

    FileEnd = strlen(WriteBuffer);
    TheFile = CreateAndLockFile(TheFile, FILENAME, WriteBuffer, 
                                FileStart, FileEnd);
       
    /* Move the file pointer to the start of the file */
    if(SetFilePointer(TheFile, 0, NULL, FILE_BEGIN) != 0)
    {
        Fail("ERROR: SetFilePointer failed to move the file pointer back "
             "to the start of the file.");
    }
    
    /* Attempt to Read 5 bytes from this file.  Since the lock does not
       affect the calling process, this should succeed.
    */
    
    if(ReadFile(TheFile, DataBuffer, 5, &BytesRead, NULL) == 0)
    {
        Fail("ERROR: ReadFile has failed.  Attempted to read in 5 bytes from "
             "the file '%s' after it had LockFile called upon it, but within "
             "the same process.",FILENAME);
    }

    if(strncmp(DataBuffer, WriteBuffer, 5) != 0)
    {
        Fail("ERROR: The data read in from ReadFile is not what should have "
             "been written in the file. '%s' ",DataBuffer);
    }

    /* Attempt to Write 5 bytes to this file.  Since the lock does not affect
       the calling process, this should succeed.
    */

    memset(WriteBuffer, 'X', strlen(WriteBuffer));

    if(WriteFile(TheFile, WriteBuffer, 5,&BytesWritten, NULL) == 0)
    {
        Fail("ERROR: WriteFile has failed.  Attempted to write 5 bytes to "
             "the file '%s' after it had LockFile called upon it, but within "
             "the same process.",FILENAME);
    }

    if(UnlockFile(TheFile, FileStart, 0, FileEnd, 0) == 0)
    {
        Fail("ERROR: UnlockFile failed.  GetLastError returns %d.",
             GetLastError());
    }
    
    if(CloseHandle(TheFile) == 0)
    {
        Fail("ERROR: CloseHandle failed to close the file.");
    }
  
    PAL_Terminate();
    return PASS;
}
