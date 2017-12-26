/*=====================================================================
**
** Source:  SetCurrentDirectoryW.c (test 3)
**
** Purpose: Try calling SetCurrentDirectoryW with an invalid path,
**          with a valid filename and with NULL
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
    char szDirName[MAX_PATH]  = "testing";
    WCHAR* szwDirName = NULL;
    char szFileName[MAX_PATH] = "setcurrentdirectorya.c";
    WCHAR* szwFileName = NULL;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* set the current directory to an unexistant folder */
    szwDirName = convert(szDirName);
    if (0 != SetCurrentDirectoryW(szwDirName))
    {
        free(szwDirName);
        Fail("ERROR: SetCurrentDirectoryW should have failed "
             "when trying to set the current directory to "
             "an invalid folder\n");
    }
    free(szwDirName);

    /* set the current directory to an unexistant folder */
    szwFileName = convert(szFileName);
    if (0 != SetCurrentDirectoryW(szwFileName))
    {
        free(szwFileName);
        Fail("ERROR: SetCurrentDirectoryW should have failed "
             "when trying to set the current directory to "
             "a valid file name\n");
    }
    free(szwFileName);
    
    /* set the current directory to NULL */
    if (0 != SetCurrentDirectoryW(NULL))
    {
        Fail("ERROR: SetCurrentDirectoryW should have failed "
             "when trying to set the current directory to "
             "NULL\n");
    }

    PAL_Terminate();

    return PASS;
}


