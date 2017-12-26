/*=============================================================
**
** Source: helper.c
**
** Purpose: A child process which will attempt to append to the end of 
** a locked file.
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
    DWORD BytesWritten;
    
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
        Trace("ERROR: Could not open file '%s' with CreateFile.  "
             "GetLastError() returns %d.",FILENAME,GetLastError()); 
        result = -1;
    }
      
    
    /* Move the FilePointer to the EOF */
    if(SetFilePointer(TheFile,0,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
    {
        Trace("ERROR: Could not set the file pointer to the EOF "
             "using SetFilePointer.  It returned INVALID_SET_FILE_POINTER.");
        result = -1;
    }

    memset(DataBuffer, 'X', BUF_SIZE);

    /* Return the result of WriteFile -- we want to check in the parent that
       this was successful.  Note: WriteFile doesn't get run if something
       failed during the setup, in that case -1 is returned.
    */

    if(result != -1)
    {
        result = WriteFile(TheFile, DataBuffer, 3,&BytesWritten, NULL);   
    }
    
    PAL_Terminate();
    return result;
}
