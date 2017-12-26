/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests that GetCPInfo works for CP_ACP and 0x4E4 (default codepage)
**          Also makes sure it correctly handles an invalid code page.
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
**==========================================================================*/


#include <palsuite.h>


int __cdecl main(int argc, char *argv[])
{
    CPINFO cpinfo;

    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    if (!GetCPInfo(CP_ACP, &cpinfo))
    {
        Fail("GetCPInfo() unable to get info for CP_ACP\n");
    }
    if (!GetCPInfo(0x4E4, &cpinfo))
    {
        Fail("GetCPInfo() unable to get info for code page 0x4E4\n");
    }


    if (GetCPInfo(-1, &cpinfo))
    {
        Fail("GetCPInfo() did not error on invalid code page!\n");
    }
    
    if (GetLastError() != ERROR_INVALID_PARAMETER)
    {
        Fail("GetCPInfo() failed to set the last error to"
             " ERROR_INVALID_PARAMETER!\n");
    }
    

    PAL_Terminate();

    return PASS;
}

