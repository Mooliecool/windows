/*=====================================================================
**
** Source:  GetTempFileNameA.c (test 1)
**
** Purpose: Tests the PAL implementation of the GetTempFileNameA function.
**
** Depends on:
**          GetFileAttributesA
**          DeleteFileA
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
    const UINT uUnique = 0;
    const char* szDot = {"."};
    const char* szValidPrefix = {"cfr"};
    const char* szLongValidPrefix = {"cfrwxyz"};
    char szReturnedName[256];
    char szTempString[256];

    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* valid path with null prefix */
    uiError = GetTempFileNameA(szDot, NULL, uUnique, szReturnedName);
    if (uiError == 0)
    {
        Fail("GetTempFileNameA: ERROR -> Call failed with a valid path "
            "with the error code: %ld\n", GetLastError());
    }
    else
    {
        /* verify temp file was created */
        if (GetFileAttributesA(szReturnedName) == -1)
        {
            Fail("GetTempFileNameA: ERROR -> GetFileAttributes failed on the "
                "returned temp file \"%s\" with error code: %ld.\n", 
                szReturnedName,
                GetLastError());
        }
        if (DeleteFileA(szReturnedName) != TRUE)
        {
            Fail("GetTempFileNameA: ERROR -> DeleteFileW failed to delete"
                "the created temp file with error code: %ld.\n", GetLastError());
        }
    }


    /* valid path with valid prefix */
    uiError = GetTempFileNameA(szDot, szValidPrefix, uUnique, szReturnedName);
    if (uiError == 0)
    {
        Fail("GetTempFileNameA: ERROR -> Call failed with a valid path and "
            "prefix with the error code: %ld\n", GetLastError());
    }
    else
    {
        /* verify temp file was created */
        if (GetFileAttributesA(szReturnedName) == -1)
        {
            Fail("GetTempFileNameA: ERROR -> GetFileAttributes failed on the "
                "returned temp file \"%s\" with error code: %ld.\n", 
                szReturnedName,
                GetLastError());
        }
        if (DeleteFileA(szReturnedName) != TRUE)
        {
            Fail("GetTempFileNameA: ERROR -> DeleteFileW failed to delete"
                "the created temp \"%s\" file with error code: %ld.\n", 
                szReturnedName,
                GetLastError());
        }
    }

    /* valid path with long prefix */
    uiError = GetTempFileNameA(szDot, szLongValidPrefix, uUnique, szReturnedName);
    if (uiError == 0)
    {
        Fail("GetTempFileNameA: ERROR -> Call failed with a valid path and "
            "prefix with the error code: %ld\n", GetLastError());
    }
    else
    {
        /* verify temp file was created */
        if (GetFileAttributesA(szReturnedName) == -1)
        {
            Fail("GetTempFileNameA: ERROR -> GetFileAttributes failed on the "
                "returned temp file \"%s\" with error code: %ld.\n", 
                szReturnedName,
                GetLastError());
        }

        /* now verify that it only used the first 3 characters of the prefix */
        sprintf(szTempString, "%s\\%s", szDot, szLongValidPrefix);
        if (strncmp(szTempString, szReturnedName, 6) == 0)
        {
            Fail("GetTempFileNameA: ERROR -> It appears that an improper prefix "
                "was used.\n");
        }

        if (DeleteFileA(szReturnedName) != TRUE)
        {
            Fail("GetTempFileNameA: ERROR -> DeleteFileW failed to delete"
                "the created temp file \"%s\" with error code: %ld.\n", 
                szReturnedName,
                GetLastError());
        }
    }

    PAL_Terminate();
    return PASS;
}
