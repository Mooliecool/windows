/*=====================================================================
**
** Source:  GetTempFileNameW.c (test 2)
**
** Purpose: Tests the PAL implementation of the GetTempFileNameW function.
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



int __cdecl main(int argc, char *argv[])
{
    UINT uiError = 0;
    DWORD dwError = 0;
    const UINT uUnique = 0;
    WCHAR* wPrefix = NULL;
    WCHAR* wPath = NULL;
    WCHAR wReturnedName[256];
    DWORD i;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }


    // test the number of temp files that can be created
    wPrefix = convert("cfr");
    wPath = convert(".");
    for (i = 0; i < 0x10005; i++)
    {
        uiError = GetTempFileNameW(wPath, wPrefix, uUnique, wReturnedName);
        if (uiError == 0)
        {
            dwError = GetLastError();
            if (dwError == ERROR_FILE_EXISTS)
            {
                // file already existes so break out of the loop
                i--; // decrement the count because it wasn't successful
                break;
            }
            else
            {
                // it was something other than the file already existing?
                free (wPath);
                free (wPrefix);
                Fail("GetTempFileNameW: ERROR -> Call failed with a valid "
                    "path and prefix with the error code: %ld\n", GetLastError());
            }
        }
        else
        {
            // verify temp file was created
            if (GetFileAttributesW(wReturnedName) == -1)
            {
                free (wPath);
                free (wPrefix);
                Fail("GetTempFileNameW: ERROR -> GetFileAttributes failed "
                    "on the returned temp file with error code: %ld.\n", 
                    GetLastError());
            }
        }
    }

    free (wPath);
    free (wPrefix);

    // did it create more than 0xffff files
    if (i > 0xffff)
    {
        Fail("GetTempFileNameW: ERROR -> Was able to create more than 0xffff"
            " temp files.\n");
    }

    PAL_Terminate();
    return PASS;
}
