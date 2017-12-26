/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests that GetThreadLocale returns a valid locale.
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
    LCID lcid;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    lcid = GetThreadLocale();
    
    if (!IsValidLocale(lcid, LCID_INSTALLED))
    {
        Fail("GetThreadLocale returned a locale that is not installed!\n");
    }

    PAL_Terminate();

    return PASS;
}

