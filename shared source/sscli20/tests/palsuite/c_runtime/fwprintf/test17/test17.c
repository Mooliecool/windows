/*============================================================================
**
** Source:      test17.c
**
** Purpose:     Tests the lowercase shorthand notation double specifier (%g).
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
    double val = 2560.001;
    double neg = -2560.001;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoDoubleTest(convert("foo %g"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %lg"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %hg"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %Lg"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %I64g"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %5g"), val,  "foo  2560", "foo  2560");
    DoDoubleTest(convert("foo %-5g"), val,  "foo 2560 ", "foo 2560 ");
    DoDoubleTest(convert("foo %.1g"), val,  "foo 3e+003", "foo 3e+03");
    DoDoubleTest(convert("foo %.2g"), val,  "foo 2.6e+003", "foo 2.6e+03");
    DoDoubleTest(convert("foo %.12g"), val,  "foo 2560.001", "foo 2560.001");
    DoDoubleTest(convert("foo %06g"), val,  "foo 002560", "foo 002560");
    DoDoubleTest(convert("foo %#g"), val,  "foo 2560.00", "foo 2560.00");
    DoDoubleTest(convert("foo %+g"), val,  "foo +2560", "foo +2560");
    DoDoubleTest(convert("foo % g"), val,  "foo  2560", "foo  2560");
    DoDoubleTest(convert("foo %+g"), neg,  "foo -2560", "foo -2560");
    DoDoubleTest(convert("foo % g"), neg,  "foo -2560", "foo -2560");

    PAL_Terminate();
    return PASS;
}
