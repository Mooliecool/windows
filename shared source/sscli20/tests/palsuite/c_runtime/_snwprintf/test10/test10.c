/*============================================================================
**
** Source:  test10.c
**
** Purpose: Tests _snwprintf with octal numbers
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
#include "../_snwprintf.h"

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with wcslen */

int __cdecl main(int argc, char *argv[])
{
    int neg = -42;
    int pos = 42;
    INT64 l = 42;

    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoNumTest(convert("foo %o"), pos, convert("foo 52"));
    DoNumTest(convert("foo %lo"), 0xFFFF, convert("foo 177777"));
    DoNumTest(convert("foo %ho"), 0xFFFF, convert("foo 177777"));
    DoNumTest(convert("foo %Lo"), pos, convert("foo 52"));
    DoI64Test(convert("foo %I64o"), l, "42", convert("foo 52"));
    DoNumTest(convert("foo %3o"), pos, convert("foo  52"));
    DoNumTest(convert("foo %-3o"), pos, convert("foo 52 "));
    DoNumTest(convert("foo %.1o"), pos, convert("foo 52"));
    DoNumTest(convert("foo %.3o"), pos, convert("foo 052"));
    DoNumTest(convert("foo %03o"), pos, convert("foo 052"));
    DoNumTest(convert("foo %#o"), pos, convert("foo 052"));
    DoNumTest(convert("foo %+o"), pos, convert("foo 52"));
    DoNumTest(convert("foo % o"), pos, convert("foo 52"));
    DoNumTest(convert("foo %+o"), neg, convert("foo 37777777726"));
    DoNumTest(convert("foo % o"), neg, convert("foo 37777777726"));


    PAL_Terminate();
    return PASS;
}

