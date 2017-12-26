/*=====================================================================
**
** Source:  getfiletype.c
**
** Purpose: Test the PAL implementation of GetFileType to ensure it
**          recognizes opened pipes.
**
** Depends: CreatePipe
**          CloseHandle
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
**===================================================================*/

#include <palsuite.h>


int __cdecl main(int argc, char **argv)
{
    HANDLE  hReadPipe   = NULL;
    HANDLE  hWritePipe  = NULL;
    BOOL    bRetVal     = FALSE;
    DWORD   dwFileType;
    SECURITY_ATTRIBUTES lpPipeAttributes;

    /*Initialize the PAL*/
    if ((PAL_Initialize(argc, argv)) != 0)
    {
        return (FAIL);
    }

    /*
    ** create a pipe and make sure GetFileType returns the correct value
    */

    /*Setup SECURITY_ATTRIBUTES structure for CreatePipe*/
    lpPipeAttributes.nLength              = sizeof(lpPipeAttributes); 
    lpPipeAttributes.lpSecurityDescriptor = NULL; 
    lpPipeAttributes.bInheritHandle       = TRUE; 

    /*Create a Pipe*/
    bRetVal = CreatePipe(&hReadPipe,      /* read handle*/
                &hWritePipe,              /* write handle */
                &lpPipeAttributes,        /* security attributes*/
                0);                       /* pipe size*/
    if (bRetVal == FALSE)
    {
        Fail("ERROR: %u :Unable to create pipe.\n", GetLastError());
    }

    // Get the file type
    dwFileType = GetFileType(hReadPipe);
    if (dwFileType != FILE_TYPE_PIPE)
    {
        if (!CloseHandle(hWritePipe))
        {
            Trace("ERROR: %u : Unable to close write pipe handle "
                "hWritePipe=0x%lx\n", GetLastError(), hWritePipe);
        }
        if (!CloseHandle(hReadPipe))
        {
            Trace("ERROR: %u : Unable to close read pipe handle "
                "hReadPipe=0x%lx\n", GetLastError(), hReadPipe);
        }
        Fail("ERROR: GetFileType returned %u for a pipe instead of the "
            "expected FILE_TYPE_PIPE (%u).\n",
            dwFileType,
            FILE_TYPE_PIPE);
    }

    /*Close write pipe handle*/
    if (!CloseHandle(hWritePipe))
    {
        if (!CloseHandle(hReadPipe))
        {
            Trace("ERROR: %u : Unable to close read pipe handle "
                "hReadPipe=0x%lx\n", GetLastError(), hReadPipe);
        }
        Fail("ERROR: %u : Unable to close write pipe handle "
             "hWritePipe=0x%lx\n", GetLastError(), hWritePipe);
    }

    /*Close Read pipe handle*/
    if (!CloseHandle(hReadPipe))
    {
        Fail("ERROR: %u : Unable to close read pipe handle "
             "hReadPipe=0x%lx\n", GetLastError(), hReadPipe);
    }

    PAL_Terminate();
    return (PASS);
}
