/*=============================================================
**
** Source: helper.c
**
** Purpose: A child process which will attempt to read and write to files
** which were locked in the parent.
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

#define FILENAME "testfile.txt"
#define BUF_SIZE 128

int __cdecl main(int argc, char *argv[])
{
    HANDLE TheFile;
    int result = 0;
    char DataBuffer[BUF_SIZE];
    DWORD BytesRead, BytesWritten;
    
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
        Fail("ERROR: Could not open file '%s' with CreateFile.  "
             "GetLastError() returns %d.",FILENAME,GetLastError()); 
    }
    
    /* Attempt to Read the first 3 bytes from this file.  
       Since it is unlocked, this should work properly.
    */
    
    if(ReadFile(TheFile, DataBuffer, 3, &BytesRead, NULL) == 0)
    {
        Trace("ERROR: ReadFile should have succeeded in reading the first "
              "three bytes of the file, as these bytes were not locked.  "
              "GetLastError() returned %d.",GetLastError());
        result = 1;
    }

    /* Now, read the next 10 bytes, which should be locked.  Ensure that 
       ReadFile fails.
    */
    
    if(ReadFile(TheFile, DataBuffer,10, &BytesRead, NULL) != 0)
    {
        Trace("ERROR: ReadFile should have failed when attempting to read in "
              "bytes between StartOfFile+3 and EndOfFile-3.");
        result = 1;
    }
    
    /* Attempt to Write 10 bytes to this file.  Since it is locked this should
       fail.
    */
    
    memset(DataBuffer,'X',BUF_SIZE);
    
    if(WriteFile(TheFile, DataBuffer, 10,&BytesWritten, NULL) != 0)
    {
        Trace("ERROR: WriteFile should have failed when attempting to write "
              "bytes between StartOfFile+3 and EOF-3.");
        result = 1;
    } 
    
    
    /* Move the FilePointer to the EOF-3, where the lock ends */
    if(SetFilePointer(TheFile,-3,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
    {
        Fail("ERROR: Could not set the file pointer to the EOF-3 "
             "using SetFilePointer.  It returned INVALID_SET_FILE_POINTER.");
    }
    
    /* Attempt to write to those 3 unlocked bytes on the end of the file */
    if(WriteFile(TheFile, DataBuffer, 3,&BytesWritten, NULL) == 0)
    {
        Trace("ERROR: WriteFile should have succeeded when attempting "
              "to write the last three bytes of the file, as they were not "
              "locked.  GetLastError() returned %d.",GetLastError());
        result = 1;
    } 
    
    PAL_Terminate();
    return result;
}
