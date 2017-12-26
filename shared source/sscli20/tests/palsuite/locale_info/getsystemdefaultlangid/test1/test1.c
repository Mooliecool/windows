/*============================================================================
**
** Source: test1.c
**
** Purpose: Checks that GetSystemDefaultLangID can be used to make a valid
**          locale, and that it is consistent with LOCALE_USER_DEFAULT.
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
    LANGID LangID;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    LangID = GetSystemDefaultLangID();
    if (LangID == 0)
    {
        Fail("GetSystemDefaultLangID failed!\n");
    }

    /* Try using the langid (with default sort) as a locale */
    if (!SetThreadLocale(MAKELCID(LangID, SORT_DEFAULT)))
    {
        Fail("Unable to use GetSystemDefaultLangID as a locale!\n");
    }
    lcid = GetThreadLocale();
    if (!IsValidLocale(lcid, LCID_INSTALLED))
    {
        Fail("Unable to use GetSystemDefaultLangID as a locale!\n");
    }

    /* Make sure results consistent with using LOCALE_USER_DEFAULT */
    if (!SetThreadLocale(LOCALE_USER_DEFAULT))
    {
        Fail("Unexpected error testing GetSystemDefaultLangID!\n");
    }
    if (GetThreadLocale() != lcid)
    {
        Fail("Results from GetSystemDefaultLangID inconsistent with "
            "LOCALE_USER_DEFAULT!\n");
    }

    PAL_Terminate();

    return PASS;
}
