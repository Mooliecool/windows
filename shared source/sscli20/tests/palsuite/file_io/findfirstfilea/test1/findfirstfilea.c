/*=====================================================================
**
** Source:  FindFirstFileA.c
**
** Purpose: Tests the PAL implementation of the FindFirstFileA function.
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


const char* szNoFileName =          "333asdf.x77t";
const char* szFindName =            "test01.txt";
const char* szFindNameWldCard_01 =  "test0?.txt";
const char* szFindNameWldCard_02 =  "*.txt";
const char* szDirName =             "test_dir";
const char* szDirNameSlash =        "test_dir\\";
const char* szDirNameWldCard_01 =   "?est_dir";
const char* szDirNameWldCard_02 =   "test_*";




int __cdecl main(int argc, char *argv[])
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = NULL;
    FILE *pFile = NULL;
    BOOL bRc = FALSE;
    WCHAR* szwTemp = NULL;


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    //
    // find a file with a NULL pointer
    //
    hFind = FindFirstFileA(NULL, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        Fail ("FindFirstFileA: ERROR -> Found invalid NULL file");
    }


    //
    // find a file that doesn't exist
    //
    hFind = FindFirstFileA(szNoFileName, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        Fail ("FindFirstFileA: ERROR -> Found invalid NULL file");
    }


    //
    // find a file that exists
    //
    pFile = fopen(szFindName, "w");
    if (pFile == NULL)
    {
        Fail("FindFirstFileA: ERROR -> Unable to create a test file\n");
    }
    else
    {
        fclose(pFile);
    }
    hFind = FindFirstFileA(szFindName, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        Fail ("FindFirstFileA: ERROR -> Unable to find \"%s\"\n", szFindName);
    }
    else
    {
        // validate we found the correct file
        if (strcmp(szFindName, findFileData.cFileName) != 0)
        {
            Fail ("FindFirstFileA: ERROR -> Found the wrong file\n");
        }
    }


    //
    // find a directory that exists
    //
    szwTemp = convert((LPSTR)szDirName);
    bRc = CreateDirectoryW(szwTemp, NULL);
    free(szwTemp);
    if (bRc == FALSE)
    {
        Fail("FindFirstFileA: ERROR -> Failed to create the directory "
            "\"%s\"\n", 
            szDirName);
    }

    hFind = FindFirstFileA(szDirName, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        Fail ("FindFirstFileA: ERROR. Unable to find \"%s\"\n", szDirName);
    }
    else
    {
        // validate we found the correct directory
        if (strcmp(szDirName, findFileData.cFileName) != 0)
        {
            Fail ("FindFirstFileA: ERROR -> Found the wrong directory\n");
        }
    }


    //
    // find a directory using a trailing '\' on the directory name: should fail
    //
    hFind = FindFirstFileA(szDirNameSlash, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) 
    {
        Fail ("FindFirstFileA: ERROR -> Able to find \"%s\": trailing "
            "slash should have failed.\n", 
            szDirNameSlash);
    }

    // find a file using wild cards
    hFind = FindFirstFileA(szFindNameWldCard_01, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        Fail ("FindFirstFileA: ERROR -> Unable to find \"%s\"\n", 
            szFindNameWldCard_01);
    }

    hFind = FindFirstFileA(szFindNameWldCard_02, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        Fail ("FindFirstFileA: ERROR -> Unable to find \"%s\"\n", szFindNameWldCard_02);
    }


    //
    // find a directory using wild cards
    //
    hFind = FindFirstFileA(szDirNameWldCard_01, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        Fail ("FindFirstFileA: ERROR -> Unable to find \"%s\"\n", szDirNameWldCard_01);
    }

    hFind = FindFirstFileA(szDirNameWldCard_02, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        Fail ("FindFirstFileA: ERROR -> Unable to find \"%s\"\n", szDirNameWldCard_02);
    }

    szwTemp = convert((LPSTR)szDirName);
    RemoveDirectoryW(szwTemp);
    free(szwTemp);
    DeleteFileA(szFindName);
    PAL_Terminate();  

    return PASS;
}
