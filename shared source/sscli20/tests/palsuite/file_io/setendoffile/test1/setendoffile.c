/*=====================================================================
**
** Source:  SetEndOfFile.c (test 1)
**
** Purpose: Tests the PAL implementation of the SetEndOfFile function.
**          This test will attempt to operate on a NULL file handle and
**          also test truncating a file not opened with GENERIC_WRITE
**
** Assumes successful:
**          SetEndOfFile
**          CreateFile
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

const char* szTextFile = "text.txt";


int __cdecl main(int argc, char *argv[])
{
    HANDLE hFile = NULL;
    BOOL bRc = FALSE;


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    bRc = SetEndOfFile(NULL);
    if (bRc == TRUE)
    {
        Fail("SetEndOfFile: ERROR -> Operation succeeded on a NULL file "
            "handle\n");
    }

    /* create a test file */
    hFile = CreateFile(szTextFile,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        Fail("SetEndOfFile: ERROR -> Unable to create file \"%s\".\n",
            szTextFile);
    }

    bRc = SetEndOfFile(hFile);
    if (bRc == TRUE)
    {
        Trace("SetEndOfFile: ERROR -> Operation succeeded on read-only"
              " file.\n");
        bRc = CloseHandle(hFile);
        if (bRc != TRUE)
        {
            Trace("SetEndOfFile: ERROR -> Unable to close file \"%s\".\n",
                szTextFile);
        }
        PAL_Terminate();
        return FAIL;
    }

    bRc = CloseHandle(hFile);
    if (bRc != TRUE)
    {
        Fail("SetEndOfFile: ERROR -> Unable to close file \"%s\".\n",
            szTextFile);
    }


    PAL_Terminate();
    return PASS;
}
