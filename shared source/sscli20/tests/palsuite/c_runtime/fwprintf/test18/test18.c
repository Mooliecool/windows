/*============================================================================
**
** Source:      test18.c
**
** Purpose:     Tests the uppercase shorthand notation double specifier (%G).
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

    DoDoubleTest(convert("foo %G"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %lG"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %hG"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %LG"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %I64G"), val,  "foo 2560", "foo 2560");
    DoDoubleTest(convert("foo %5G"), val,  "foo  2560", "foo  2560");
    DoDoubleTest(convert("foo %-5G"), val,  "foo 2560 ", "foo 2560 ");
    DoDoubleTest(convert("foo %.1G"), val,  "foo 3E+003", "foo 3E+03");
    DoDoubleTest(convert("foo %.2G"), val,  "foo 2.6E+003", "foo 2.6E+03");
    DoDoubleTest(convert("foo %.12G"), val,  "foo 2560.001", "foo 2560.001");
    DoDoubleTest(convert("foo %06G"), val,  "foo 002560", "foo 002560");
    DoDoubleTest(convert("foo %#G"), val,  "foo 2560.00", "foo 2560.00");
    DoDoubleTest(convert("foo %+G"), val,  "foo +2560", "foo +2560");
    DoDoubleTest(convert("foo % G"), val,  "foo  2560", "foo  2560");
    DoDoubleTest(convert("foo %+G"), neg,  "foo -2560", "foo -2560");
    DoDoubleTest(convert("foo % G"), neg,  "foo -2560", "foo -2560");

    PAL_Terminate();
    return PASS;
}
