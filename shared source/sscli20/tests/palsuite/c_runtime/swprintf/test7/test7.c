/*============================================================================
**
** Source:  test7.c
**
** Purpose: Tests swprintf with wide characters
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
#include "../swprintf.h"

/*
 * Uses memcmp & wcslen
 */


int __cdecl main(int argc, char *argv[])
{
    WCHAR wb = (WCHAR) 'b';
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoCharTest(convert("foo %C"), 'c', convert("foo c"));
    DoWCharTest(convert("foo %hc"), wb, convert("foo b"));
    DoCharTest(convert("foo %lC"), 'c', convert("foo c"));
    DoCharTest(convert("foo %LC"), 'c', convert("foo c"));
    DoCharTest(convert("foo %I64C"), 'c', convert("foo c"));
    DoCharTest(convert("foo %5C"), 'c', convert("foo     c"));
    DoCharTest(convert("foo %.0C"), 'c', convert("foo c"));
    DoCharTest(convert("foo %-5C"), 'c', convert("foo c    "));
    DoCharTest(convert("foo %05C"), 'c', convert("foo 0000c"));
    DoCharTest(convert("foo % C"), 'c', convert("foo c"));
    DoCharTest(convert("foo %#C"), 'c', convert("foo c"));

    PAL_Terminate();
    return PASS;
}
