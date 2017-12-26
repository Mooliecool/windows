/*============================================================================
**
** Source: test2.c
**
** Purpose: Tests that GetCPInfo gives the correct information for codepage 0x4E4 
**          (the default).
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
    int codepage;
    unsigned int i;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /*
     * codepage 1252 (0x4E4): Windows 3.1 Latin 1 (U.S., Western Europe)
     */
    codepage = 1252;
    
    if (!GetCPInfo(codepage, &cpinfo))
    {
        Fail("GetCPInfo() failed on default ansi code page!\n");
    }
    if (cpinfo.MaxCharSize != 1)
    {
        Fail("GetCPInfo() returned incorrect MaxCharSize information!\n");
    }
    if (cpinfo.DefaultChar[0] != '?' || cpinfo.DefaultChar[1] != 0)
    {
        Fail("GetCPInfo() returned incorrect DefaultChar information");
    }

    for (i = 0; i<MAX_LEADBYTES; i++)
    {
        if (cpinfo.LeadByte[i] != 0)
        {
            Fail("GetCPInfo() returned incorrect LeadByte information");
        }
    }

    PAL_Terminate();

    return PASS;
}

