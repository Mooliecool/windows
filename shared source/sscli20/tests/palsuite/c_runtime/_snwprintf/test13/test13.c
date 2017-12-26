/*============================================================================
**
** Source:  test13.c
**
** Purpose: Tests _snwprintf with hex numbers (uppercase)
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
    int pos = 0x1234ab;
    INT64 l = I64(0x1234567887654321);

    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoNumTest(convert("foo %X"), pos, convert("foo 1234AB"));
    DoNumTest(convert("foo %lX"), pos, convert("foo 1234AB"));
    DoNumTest(convert("foo %hX"), pos, convert("foo 34AB"));
    DoNumTest(convert("foo %LX"), pos, convert("foo 1234AB"));
    DoI64Test(convert("foo %I64X"), l, "0x1234567887654321",
              convert("foo 1234567887654321"));
    DoNumTest(convert("foo %7X"), pos, convert("foo  1234AB"));
    DoNumTest(convert("foo %-7X"), pos, convert("foo 1234AB "));
    DoNumTest(convert("foo %.1X"), pos, convert("foo 1234AB"));
    DoNumTest(convert("foo %.7X"), pos, convert("foo 01234AB"));
    DoNumTest(convert("foo %07X"), pos, convert("foo 01234AB"));
    DoNumTest(convert("foo %#X"), pos, convert("foo 0X1234AB"));
    DoNumTest(convert("foo %+X"), pos, convert("foo 1234AB"));
    DoNumTest(convert("foo % X"), pos, convert("foo 1234AB"));
    DoNumTest(convert("foo %+X"), neg, convert("foo FFFFFFD6"));
    DoNumTest(convert("foo % X"), neg, convert("foo FFFFFFD6"));

    PAL_Terminate();
    return PASS;
}

