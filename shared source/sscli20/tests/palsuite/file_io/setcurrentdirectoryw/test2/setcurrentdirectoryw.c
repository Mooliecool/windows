/*=====================================================================
**
** Source:  SetCurrentDirectoryW.c (test 2)
**
** Purpose: Tests the PAL implementation of the SetCurrentDirectoryW function
**          by setting the current directory with ../
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
    const char szDirName[MAX_PATH] = "testing";
    char szBuiltDir[MAX_PATH];
    WCHAR* szwBuiltDir = NULL;
    WCHAR szwHomeDirBefore[MAX_PATH];
    WCHAR szwHomeDirAfter[MAX_PATH];
    WCHAR* szwPtr = NULL;


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* create a temp directory off the current directory */
    szwPtr = convert((LPSTR)szDirName);
 
    if (CreateDirectoryW(szwPtr, NULL) != TRUE)
    {
        free(szwPtr);
        Fail("Unexpected error: CreateDirectoryW failed "
             "with error code: %ld.\n", 
             GetLastError());
    }

    /* find out what the current "home" directory is */
    memset(szwHomeDirBefore, 0, MAX_PATH * sizeof(WCHAR));

    if( 0 == GetCurrentDirectoryW(MAX_PATH, szwHomeDirBefore) )
    {
        Trace("Unexpected error: Unable to get current directory "
              "with GetCurrentDirectoryW that returned %ld\n",
              GetLastError());

        if (!RemoveDirectoryW(szwPtr))
        {
            Trace("Unexpected error: RemoveDirectoryW failed "
            "with error code: %ld.\n", 
            GetLastError());
        }
        free(szwPtr);

        Fail("");
    }

     /* append the temp name to the "home" directory */
    memset(szBuiltDir, 0, MAX_PATH);
#if WIN32
    sprintf(szBuiltDir,"%s\\..\\", szDirName);
#else
    sprintf(szBuiltDir,"%s/../", szDirName);
#endif

    szwBuiltDir = convert(szBuiltDir);

    /* set the current directory to the temp directory */
    if (SetCurrentDirectoryW(szwBuiltDir) != TRUE)
    {
        Trace("ERROR: Unable to set current "
              "directory to %S. Failed with error code: %ld.\n", 
              szwBuiltDir,
              GetLastError());

        if (!RemoveDirectoryW(szwPtr))
        {
            Trace("Unexpected error: RemoveDirectoryW failed "
            "with error code: %ld.\n", 
            GetLastError());
        }
        free(szwPtr);
        free(szwBuiltDir);
        Fail("");
    }

    free(szwBuiltDir);

    /* find out what the current "home" directory is */
    memset(szwHomeDirAfter, 0, MAX_PATH * sizeof(WCHAR));

    if( 0 == GetCurrentDirectoryW(MAX_PATH, szwHomeDirAfter) )
    {
        Trace("Unexpected error: Unable to get current directory "
              "with GetCurrentDirectoryW that returned %ld\n",
              GetLastError());

        if (!RemoveDirectoryW(szwPtr))
        {
            Trace("ERROR: RemoveDirectoryW failed "
            "with error code: %ld.\n", 
            GetLastError());
        }
        free(szwPtr);

        Fail("");
    }

    /*compare the new current dir to the compiled current dir */
    if (wcsncmp(szwHomeDirBefore, szwHomeDirAfter, wcslen(szwHomeDirBefore)) != 0)
    {
        Trace("ERROR:The set directory \"%S\" does not "
              "compare to the built directory \"%S\".\n",
              szwHomeDirAfter,
              szwHomeDirBefore);

        if (!RemoveDirectoryW(szwPtr))
        {
            Trace("Unexpected error: RemoveDirectoryW failed "
                  "with error code: %ld.\n", 
                  GetLastError());
        }
        free(szwPtr);
        Fail("");
    }

    /* clean up */
    if (!RemoveDirectoryW(szwPtr))
    {
        free(szwPtr);
        Fail("Unexpected error: RemoveDirectoryW failed "
             "with error code: %ld.\n", 
             GetLastError());
    }

    free(szwPtr);
    PAL_Terminate();

    return PASS;
}


