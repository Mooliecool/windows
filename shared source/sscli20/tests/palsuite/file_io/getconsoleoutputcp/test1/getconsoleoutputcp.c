/*=====================================================================
**
** Source:  GetConsoleOutputCP.c (test 1)
**
** Purpose: Tests the PAL implementation of the GetConsoleOutputCP function.
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
    UINT uiCP = 0;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    uiCP = GetConsoleOutputCP();
    if ((uiCP != CP_ACP) && (uiCP != GetACP()) && (uiCP != 437)) /*437 for MSDOS*/
    {
        Fail("GetConsoleOutputCP: ERROR -> The invalid code page %d was returned.\n", 
            uiCP);
    }

    PAL_Terminate();
    return PASS;
}
