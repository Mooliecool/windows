/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
** Tests that wcsncmp case-sensitively compares wide strings, making sure that
** the count argument is handled correctly.
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
 * Notes: uses wcslen.
 */

int __cdecl main(int argc, char *argv[])
{
    WCHAR str1[] = {'f','o','o',0};
    WCHAR str2[] = {'f','o','o','x',0};
    WCHAR str3[] = {'f','O','o',0};
    char cstr1[] = "foo";
    char cstr2[] = "foox";
    char cstr3[] = "fOo";
    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }



    if (wcsncmp(str1, str2, wcslen(str2)) >= 0)
    {
        Fail("ERROR: wcsncmp(\"%s\", \"%s\", %d) returned >= 0\n", cstr1, 
            cstr2, wcslen(str2));
    }

    if (wcsncmp(str2, str1, wcslen(str2)) <= 0)
    {
        Fail("ERROR: wcsncmp(\"%s\", \"%s\", %d) returned <= 0\n", cstr2, 
            cstr1, wcslen(str2));
    }

    if (wcsncmp(str1, str2, wcslen(str1)) != 0)
    {
        Fail("ERROR: wcsncmp(\"%s\", \"%s\", %d) returned != 0\n", cstr1, 
            cstr2, wcslen(str1));
    }

    if (wcsncmp(str1, str3, wcslen(str1)) <= 0)
    {
        Fail("ERROR: wcsncmp(\"%s\", \"%s\", %d) returned >= 0\n", cstr1, 
            cstr3, wcslen(str1));
    }

    if (wcsncmp(str3, str1, wcslen(str1)) >= 0)
    {
        Fail("ERROR: wcsncmp(\"%s\", \"%s\", %d) returned >= 0\n", cstr3, 
            cstr1, wcslen(str1));
    }

    PAL_Terminate();
    return PASS;
}

