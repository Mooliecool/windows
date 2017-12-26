/*=====================================================================
**
** Source:  GetFileType.c (test 1)
**
** Purpose: Tests the PAL implementation of the GetFileType function.
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
    DWORD dwRc = 0;


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }


    /* test FILE_TYPE_UNKNOWN */
    dwRc = GetFileType(hFile);
    if (dwRc != FILE_TYPE_UNKNOWN)
    {
        Fail("GetFileType: ERROR -> Was expecting a return type of "
            "FILE_TYPE_UNKNOWN but the function returned %ld.\n",
            dwRc);
    }


    /* create a test file */
    hFile = CreateFile(szTextFile, 
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        Fail("GetFileType: ERROR -> Unable to create file \"%s\".\n", 
            szTextFile);
    }

    dwRc = GetFileType(hFile);
    if (CloseHandle(hFile) != TRUE)
    {
        Fail("GetFileType: ERROR -> Unable to close file \"%s\".\n", 
            szTextFile);
    }
    if (!DeleteFileA(szTextFile))
    {
        Fail("GetFileType: ERROR -> Unable to delete file \"%s\".\n", 
            szTextFile);
    }

    if (dwRc != FILE_TYPE_DISK)
    {
        Fail("GetFileType: ERROR -> Was expecting a return type of "
            "FILE_TYPE_DISK but the function returned %ld.\n",
            dwRc);
    }

    PAL_Terminate();
    return PASS;
}
