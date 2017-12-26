/*============================================================================
**
** Source:      test8.c
**
** Purpose:     Tests the decimal specifier (%d).
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
    int neg = -42;
    int pos = 42;
    INT64 l = 42;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoNumTest(convert("foo %d"), pos, "foo 42");
    DoNumTest(convert("foo %ld"), 0xFFFF, "foo 65535");
    DoNumTest(convert("foo %hd"), 0xFFFF, "foo -1");
    DoNumTest(convert("foo %Ld"), pos, "foo 42");
    DoI64Test(convert("foo %I64d"), l, "42", "foo 42", "foo 42");
    DoNumTest(convert("foo %3d"), pos, "foo  42");
    DoNumTest(convert("foo %-3d"), pos, "foo 42 ");
    DoNumTest(convert("foo %.1d"), pos, "foo 42");
    DoNumTest(convert("foo %.3d"), pos, "foo 042");
    DoNumTest(convert("foo %03d"), pos, "foo 042");
    DoNumTest(convert("foo %#d"), pos, "foo 42");
    DoNumTest(convert("foo %+d"), pos, "foo +42");
    DoNumTest(convert("foo % d"), pos, "foo  42");
    DoNumTest(convert("foo %+d"), neg, "foo -42");
    DoNumTest(convert("foo % d"), neg, "foo -42");

    PAL_Terminate();
    return PASS;
}
