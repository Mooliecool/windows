/*============================================================================
**
** Source:      test3.c
**
** Purpose:     Tests the wide string specifier (%S).
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
    
    DoStrTest(convert("foo %S"), "bar", "foo bar");
    DoStrTest(convert("foo %hS"), "bar", "foo bar");
    DoWStrTest(convert("foo %lS"), convert("bar"), "foo bar");
    DoWStrTest(convert("foo %wS"), convert("bar"), "foo bar");
    DoStrTest(convert("foo %LS"), "bar", "foo bar");
    DoStrTest(convert("foo %I64S"), "bar", "foo bar");
    DoStrTest(convert("foo %5S"), "bar", "foo   bar");
    DoStrTest(convert("foo %.2S"), "bar", "foo ba");
    DoStrTest(convert("foo %5.2S"),"bar", "foo    ba");
    DoStrTest(convert("foo %-5S"), "bar", "foo bar  ");
    DoStrTest(convert("foo %05S"), "bar", "foo 00bar");

    PAL_Terminate();
    return PASS;
}


