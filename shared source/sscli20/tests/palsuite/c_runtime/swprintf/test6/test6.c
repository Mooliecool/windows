/*============================================================================
**
** Source:  test6.c
**
** Purpose: Tests swprintf with character
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


    DoWCharTest(convert("foo %c"), wb, convert("foo b"));
    DoCharTest(convert("foo %hc"), 'c', convert("foo c"));
    DoWCharTest(convert("foo %lc"), wb, convert("foo b"));
    DoWCharTest(convert("foo %Lc"), wb, convert("foo b"));
    DoWCharTest(convert("foo %I64c"), wb, convert("foo b"));
    DoWCharTest(convert("foo %5c"), wb, convert("foo     b"));
    DoWCharTest(convert("foo %.0c"), wb, convert("foo b"));
    DoWCharTest(convert("foo %-5c"), wb, convert("foo b    "));
    DoWCharTest(convert("foo %05c"), wb, convert("foo 0000b"));
    DoWCharTest(convert("foo % c"), wb, convert("foo b"));
    DoWCharTest(convert("foo %#c"), wb, convert("foo b"));

    PAL_Terminate();
    return PASS;
}
