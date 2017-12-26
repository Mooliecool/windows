/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests IsValidLocale with the current locale, -1, and 
**          LOCALE_USER_DEFAULT (which actually isn't valid).
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

    
    /*
     * Passing LOCALE_USER_DEFAULT to IsValidLocale will fail, so instead
     * the current thread localed is changed to it, and that lcid is passed
     * to IsValidLocale (which should always pass)
     */
    if (!SetThreadLocale(LOCALE_USER_DEFAULT))
    {
        Fail("Unable to set locale to LOCALE_USER_DEFAULT!\n");
    }

    lcid = GetThreadLocale();

    if (!IsValidLocale(lcid, LCID_SUPPORTED))
    {
        Fail("IsValidLocale found the default user locale unsupported!\n");
    }
    if (!IsValidLocale(lcid, LCID_INSTALLED))
    {
        Fail("IsValidLocale found the default user locale uninstalled!\n");
    }

    /*
     * Test out bad parameters
     */
    if (IsValidLocale(-1, LCID_SUPPORTED))
    {
        Fail("IsValideLocale passed with an invalid LCID!\n");
    }    
    if (IsValidLocale(-1, LCID_INSTALLED))
    {
        Fail("IsValideLocale passed with an invalid LCID!\n");
    }    

    if (IsValidLocale(LOCALE_USER_DEFAULT, LCID_SUPPORTED))
    {
        Fail("IsValidLocale passed with LOCALE_USER_DEFAULT!\n");
    }
    if (IsValidLocale(LOCALE_USER_DEFAULT, LCID_INSTALLED))
    {
        Fail("IsValidLocale passed with LOCALE_USER_DEFAULT!\n");
    }

    PAL_Terminate();

    return PASS;
}

