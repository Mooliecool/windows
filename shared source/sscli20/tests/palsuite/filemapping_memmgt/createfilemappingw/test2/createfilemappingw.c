/*=============================================================
**
** Source:  createfilemappingw.c
**
** Purpose: Positive test the CreateFileMapping API.
**          Call CreateFileMapping to create a unnamed
**          file-mapping object with PAGE_READONLY
**          protection and SEC_IMAGE attribute in UNICODE
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
#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{

    HANDLE FileHandle;
    HANDLE FileMappingHandle;
    int err;
    WCHAR *wpFileName = NULL;
    char executableFileName[256]="";


    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }

#if WIN32
    sprintf(executableFileName,"%s","executable.exe");
#else
    sprintf(executableFileName,"%s","executable");
#endif

    //conver string to a unicode one
    wpFileName = convert(executableFileName);


    //create a file and return the file handle
    FileHandle = CreateFile(wpFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_ARCHIVE,
        NULL);

    //free this memory
    free(wpFileName);
   
    if(INVALID_HANDLE_VALUE == FileHandle)
    {
        Fail("Failed to call CreateFile to create a file\n");
    }

    //create a unnamed file-mapping object with file handle FileHandle
    //and with PAGE_READONLY protection
    FileMappingHandle = CreateFileMapping(
        FileHandle,         //File Handle
        NULL,               //not inherited
        PAGE_READONLY|SEC_IMAGE,      //access protection and section attribute
        0,                  //high-order of object size
        0,                  //low-orger of object size
        NULL);              //unnamed object


    if(NULL == FileMappingHandle) 
    {
        Trace("\nFailed to call CreateFileMapping to create a mapping object!\n");
        err = CloseHandle(FileHandle);
        if(0 == err)
        {
            Fail("\nFailed to call CloseHandle API\n");
        }
        Fail("");
    }
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        Trace("\nFile mapping object already exists!\n");
        err = CloseHandle(FileHandle);
        if(0 == err)
        {
            Trace("\nFailed to call CloseHandle API to close a file handle\n");
            err = CloseHandle(FileMappingHandle);
            if(0 == err)
            {
                Fail("\nFailed to call CloseHandle API to close a mapping object handle\n");
            }
            Fail("");
        }
        err = CloseHandle(FileMappingHandle);
        if(0 == err)
        {
            Fail("\nFailed to call CloseHandle API to close a mapping object handle\n");
        }
        Fail("");
    }
    err = CloseHandle(FileMappingHandle);
    if(0 == err)
    {
        Trace("\nFailed to call CloseHandle API to close a mapping object handle\n");
        err = CloseHandle(FileHandle);
        if(0 == err)
        {
            Fail("\nFailed to call CloseHandle API to close a file handle\n");
        }
        Fail("");
    }
    err = CloseHandle(FileHandle);
    if(0 == err)
    {
        Fail("\nFailed to call CloseHandle API to close a file handle\n");
    }

    PAL_Terminate();
    return PASS;
}
