/*============================================================================
**
** Source:      test2.c
**
** Purpose:     Tests the string specifier (%s).
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

    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoWStrTest(convert("foo %s"), convert("bar"), "foo bar");
    DoStrTest(convert("foo %hs"), "bar", "foo bar");
    DoWStrTest(convert("foo %ls"), convert("bar"), "foo bar");
    DoWStrTest(convert("foo %ws"), convert("bar"), "foo bar");
    DoWStrTest(convert("foo %Ls"), convert("bar"), "foo bar");
    DoWStrTest(convert("foo %I64s"), convert("bar"), "foo bar");
    DoWStrTest(convert("foo %5s"), convert("bar"), "foo   bar");
    DoWStrTest(convert("foo %.2s"), convert("bar"), "foo ba");
    DoWStrTest(convert("foo %5.2s"), convert("bar"), "foo    ba");
    DoWStrTest(convert("foo %-5s"), convert("bar"), "foo bar  ");
    DoWStrTest(convert("foo %05s"), convert("bar"), "foo 00bar");

    PAL_Terminate();
    return PASS;    
}

