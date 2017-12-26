/*=====================================================================
**
** Source:  SetCurrentDirectoryA.c (test 3)
**
** Purpose: Try calling SetCurrentDirectoryA with an invalid path,
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
    const char szDirName[MAX_PATH]  = "testing";
    const char szFileName[MAX_PATH] = "setcurrentdirectorya.c";

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* set the current directory to an unexistant folder */
    if (0 != SetCurrentDirectoryA(szDirName))
    {
        Fail("ERROR: SetCurrentDirectoryA should have failed "
             "when trying to set the current directory to "
             "an invalid folder\n");
    }

    /* set the current directory to an unexistant folder */
    if (0 != SetCurrentDirectoryA(szFileName))
    {
        Fail("ERROR: SetCurrentDirectoryA should have failed "
             "when trying to set the current directory to "
             "a valid file name\n");
    }
    
    /* set the current directory to NULL */
    if (0 != SetCurrentDirectoryA(NULL))
    {
        Fail("ERROR: SetCurrentDirectoryA should have failed "
             "when trying to set the current directory to "
             "NULL\n");
    }

    PAL_Terminate();

    return PASS;
}


