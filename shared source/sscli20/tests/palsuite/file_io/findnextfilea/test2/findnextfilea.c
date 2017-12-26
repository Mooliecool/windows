/*=====================================================================
**
** Source:  findnextfilea.c
**
** Purpose: Tests the PAL implementation of the FindNextFileA function.
**          Tests '*' and '*.*' to ensure that '.' and '..' are
**          returned in the expected order
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
                                                                   

const char* szDot =         ".";
const char* szDotDot =      "..";
const char* szStar =        "*";
const char* szStarDotStar = "*.*";


static void DoTest(const char* szDir, 
                   const char* szResult1, 
                   const char* szResult2)
{
    HANDLE hFind;
    WIN32_FIND_DATA findFileData;

    /*
    ** find the first
    */
    if ((hFind = FindFirstFileA(szDir, &findFileData)) == INVALID_HANDLE_VALUE)
    {
        Fail("FindNextFileA: ERROR -> FindFirstFileA(\"%s\") failed. "
            "GetLastError returned %u.\n", 
            szStar,
            GetLastError());
    }

    /* did we find the expected */
    if (strcmp(szResult1, findFileData.cFileName) != 0)
    {
        if (!FindClose(hFind))
        {
            Trace("FindNextFileA: ERROR -> Failed to close the find handle. "
                "GetLastError returned %u.\n",
                GetLastError());
        }
        Fail("FindNextFileA: ERROR -> FindFirstFile(\"%s\") didn't find"
            " the expected \"%s\" but found \"%s\" instead.\n",
            szDir,
            szResult1,
            findFileData.cFileName);
    }

    /* we found the first expected, let's see if we find the next expected*/
    if (!FindNextFileA(hFind, &findFileData))
    {
        Trace("FindNextFileA: ERROR -> FindNextFileA should have found \"%s\"" 
            " but failed. GetLastError returned %u.\n",
            szResult2,
            GetLastError());
        if (!FindClose(hFind))
        {
            Trace("FindNextFileA: ERROR -> Failed to close the find handle. "
                "GetLastError returned %u.\n",
                GetLastError());
        }
        Fail("");
    }

    /* we found something, but was it '.' */
    if (strcmp(szResult2, findFileData.cFileName) != 0)
    {
        if (!FindClose(hFind))
        {
            Trace("FindNextFileA: ERROR -> Failed to close the find handle. "
                "GetLastError returned %u.\n",
                GetLastError());
        }
        Fail("FindNextFileA: ERROR -> FindNextFileA based on \"%s\" didn't find"
            " the expected \"%s\" but found \"%s\" instead.\n",
            szDir,
            szResult2,
            findFileData.cFileName);
    }
}

int __cdecl main(int argc, char *argv[])
{

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    DoTest(szStar, szDot, szDotDot);
    DoTest(szStarDotStar, szDot, szDotDot);


    PAL_Terminate();  

    return PASS;
}
