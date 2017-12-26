/*============================================================================
**
** Source:  test15.c
**
** Purpose:Tests swprintf with exponential format doubles (uppercase)
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
    double val = 256.0;
    double neg = -256.0;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoDoubleTest(convert("foo %E"), val,  convert("foo 2.560000E+002"),
                 convert("foo 2.560000E+02"));
    DoDoubleTest(convert("foo %lE"), val,  convert("foo 2.560000E+002"),
                 convert("foo 2.560000E+02"));
    DoDoubleTest(convert("foo %hE"), val,  convert("foo 2.560000E+002"),
                 convert("foo 2.560000E+02"));
    DoDoubleTest(convert("foo %LE"), val,  convert("foo 2.560000E+002"),
                 convert("foo 2.560000E+02"));
    DoDoubleTest(convert("foo %I64E"), val,  convert("foo 2.560000E+002"),
                 convert("foo 2.560000E+02"));
    DoDoubleTest(convert("foo %14E"), val,  convert("foo  2.560000E+002"),
                 convert("foo   2.560000E+02"));
    DoDoubleTest(convert("foo %-14E"), val,  convert("foo 2.560000E+002 "),
                 convert("foo 2.560000E+02  "));
    DoDoubleTest(convert("foo %.1E"), val,  convert("foo 2.6E+002"),
                 convert("foo 2.6E+02"));
    DoDoubleTest(convert("foo %.8E"), val,  convert("foo 2.56000000E+002"),
                 convert("foo 2.56000000E+02"));
    DoDoubleTest(convert("foo %014E"), val,  convert("foo 02.560000E+002"),
                 convert("foo 002.560000E+02"));
    DoDoubleTest(convert("foo %#E"), val,  convert("foo 2.560000E+002"),
                 convert("foo 2.560000E+02"));
    DoDoubleTest(convert("foo %+E"), val,  convert("foo +2.560000E+002"),
                 convert("foo +2.560000E+02"));
    DoDoubleTest(convert("foo % E"), val,  convert("foo  2.560000E+002"),
                 convert("foo  2.560000E+02"));
    DoDoubleTest(convert("foo %+E"), neg,  convert("foo -2.560000E+002"),
                 convert("foo -2.560000E+02"));
    DoDoubleTest(convert("foo % E"), neg,  convert("foo -2.560000E+002"),
                 convert("foo -2.560000E+02"));

    PAL_Terminate();
    return PASS;
}
