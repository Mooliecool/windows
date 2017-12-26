/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests that GetUserDefaultLCID returns a valid locale that is 
**          consistent with LOCALE_USER_DEFAULT.
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
    
    lcid = GetUserDefaultLCID();
    if (lcid == 0)
    {
        Fail("GetUserDefaultLCID failed!\n");
    }

    if (!IsValidLocale(lcid, LCID_INSTALLED))
    {
        Fail("GetUserDefaultLCID gave an invalid locale!\n");
    }

    /* Make sure results consistent with using LOCALE_USER_DEFAULT */
    if (!SetThreadLocale(LOCALE_USER_DEFAULT))
    {
        Fail("Unexpected error testing GetUserDefaultLCID!\n");
    }
    if (GetThreadLocale() != lcid)
    {
        Fail("Results from GetUserDefaultLCID inconsistent with "
            "LOCALE_USER_DEFAULT!\n");
    }

    PAL_Terminate();

    return PASS;
}

