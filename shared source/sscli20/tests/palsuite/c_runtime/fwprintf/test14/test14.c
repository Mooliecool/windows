/*============================================================================
**
** Source:      test14.c
**
** Purpose:     Tests the lowercase exponential
**              notation double specifier (%e).
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
    double val = 256.0;
    double neg = -256.0;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoDoubleTest(convert("foo %e"), val,  "foo 2.560000e+002", 
        "foo 2.560000e+02");
    DoDoubleTest(convert("foo %le"), val,  "foo 2.560000e+002", 
        "foo 2.560000e+02");
    DoDoubleTest(convert("foo %he"), val,  "foo 2.560000e+002", 
        "foo 2.560000e+02");
    DoDoubleTest(convert("foo %Le"), val,  "foo 2.560000e+002", 
        "foo 2.560000e+02");
    DoDoubleTest(convert("foo %I64e"), val,  "foo 2.560000e+002", 
        "foo 2.560000e+02");
    DoDoubleTest(convert("foo %14e"), val,  "foo  2.560000e+002", 
        "foo   2.560000e+02");
    DoDoubleTest(convert("foo %-14e"), val,  "foo 2.560000e+002 ", 
        "foo 2.560000e+02  ");
    DoDoubleTest(convert("foo %.1e"), val,  "foo 2.6e+002", 
        "foo 2.6e+02");
    DoDoubleTest(convert("foo %.8e"), val,  "foo 2.56000000e+002", 
        "foo 2.56000000e+02");
    DoDoubleTest(convert("foo %014e"), val,  "foo 02.560000e+002", 
        "foo 002.560000e+02");
    DoDoubleTest(convert("foo %#e"), val,  "foo 2.560000e+002", 
        "foo 2.560000e+02");
    DoDoubleTest(convert("foo %+e"), val,  "foo +2.560000e+002", 
        "foo +2.560000e+02");
    DoDoubleTest(convert("foo % e"), val,  "foo  2.560000e+002", 
        "foo  2.560000e+02");
    DoDoubleTest(convert("foo %+e"), neg,  "foo -2.560000e+002", 
        "foo -2.560000e+02");
    DoDoubleTest(convert("foo % e"), neg,  "foo -2.560000e+002", 
        "foo -2.560000e+02");

    PAL_Terminate();
    return PASS;
}
