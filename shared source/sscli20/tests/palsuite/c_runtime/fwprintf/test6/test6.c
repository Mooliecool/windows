/*============================================================================
**
** Source:      test6.c
**
** Purpose:     Tests the char specifier (%c).
**              This test is modeled after the sprintf series.
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
#include "../fwprintf.h"

/* 
 * Depends on memcmp, strlen, fopen, fseek and fgets.
 */

int __cdecl main(int argc, char *argv[])
{
    WCHAR wb = (WCHAR) 'b';
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoWCharTest(convert("foo %c"), wb, "foo b");
    DoCharTest(convert("foo %hc"), 'c', "foo c");
    DoWCharTest(convert("foo %lc"), wb, "foo b");
    DoWCharTest(convert("foo %Lc"), wb, "foo b");
    DoWCharTest(convert("foo %I64c"), wb, "foo b");
    DoWCharTest(convert("foo %5c"), wb, "foo     b");
    DoWCharTest(convert("foo %.0c"), wb, "foo b");
    DoWCharTest(convert("foo %-5c"), wb, "foo b    ");
    DoWCharTest(convert("foo %05c"), wb, "foo 0000b");
    DoWCharTest(convert("foo % c"), wb, "foo b");
    DoWCharTest(convert("foo %#c"), wb, "foo b");
    
    PAL_Terminate();
    return PASS;
}



