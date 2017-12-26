/*============================================================================
**
** Source:  test1.c
**
** Purpose: Tests that _wcsicmp correctly compares two strings with 
**          case insensitivity.
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

/*
 * Note: The _wcsicmp is dependent on the LC_CTYPE category of the locale,
 *      and this is ignored by these tests.
 */
int __cdecl main(int argc, char *argv[])
{
    WCHAR str1[] = {'f','o','o',0};
    WCHAR str2[] = {'f','O','o',0};
    WCHAR str3[] = {'f','o','o','_','b','a','r',0};
    WCHAR str4[] = {'f','o','o','b','a','r',0};

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    if (_wcsicmp(str1, str2) != 0)
    {
        Fail ("ERROR: _wcsicmp returning incorrect value:\n"
                "_wcsicmp(\"%S\", \"%S\") != 0\n", str1, str2);
    }

    if (_wcsicmp(str2, str3) >= 0)
    {
        Fail ("ERROR: _wcsicmp returning incorrect value:\n"
                "_wcsicmp(\"%S\", \"%S\") >= 0\n", str2, str3);
    }

    if (_wcsicmp(str3, str4) >= 0)
    {
        Fail ("ERROR: _wcsicmp returning incorrect value:\n"
                "_wcsicmp(\"%S\", \"%S\") >= 0\n", str3, str4);
    }

    if (_wcsicmp(str4, str1) <= 0)
    {
        Fail ("ERROR: _wcsicmp returning incorrect value:\n"
                "_wcsicmp(\"%S\", \"%S\") <= 0\n", str4, str1);
    }

    if (_wcsicmp(str3, str2) <= 0)
    {
        Fail ("ERROR: _wcsicmp returning incorrect value:\n"
                "_wcsicmp(\"%S\", \"%S\") <= 0\n", str2, str3);
    }

    PAL_Terminate();
    return PASS;
}
