/*=====================================================================
**
** Source:    test15.c
**
** Purpose:   Test #15 for the _vsnprintf function.
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
**===================================================================*/   

#include <palsuite.h>
#include "../_vsnprintf.h"

/*
 * Notes: memcmp is used, as is strlen.
 */

int __cdecl main(int argc, char *argv[])
{
    double val = 256.0;
    double neg = -256.0;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoDoubleTest("foo %E", val,  "foo 2.560000E+002", "foo 2.560000E+02");
    DoDoubleTest("foo %lE", val,  "foo 2.560000E+002", "foo 2.560000E+02");
    DoDoubleTest("foo %hE", val,  "foo 2.560000E+002", "foo 2.560000E+02");
    DoDoubleTest("foo %LE", val,  "foo 2.560000E+002", "foo 2.560000E+02");
    DoDoubleTest("foo %I64E", val,  "foo 2.560000E+002", "foo 2.560000E+02");
    DoDoubleTest("foo %14E", val,  "foo  2.560000E+002", "foo   2.560000E+02");
    DoDoubleTest("foo %-14E", val,  "foo 2.560000E+002 ", "foo 2.560000E+02  ");
    DoDoubleTest("foo %.1E", val,  "foo 2.6E+002", "foo 2.6E+02");
    DoDoubleTest("foo %.8E", val,  "foo 2.56000000E+002", "foo 2.56000000E+02");
    DoDoubleTest("foo %014E", val,  "foo 02.560000E+002", "foo 002.560000E+02");
    DoDoubleTest("foo %#E", val,  "foo 2.560000E+002", "foo 2.560000E+02");
    DoDoubleTest("foo %+E", val,  "foo +2.560000E+002", "foo +2.560000E+02");
    DoDoubleTest("foo % E", val,  "foo  2.560000E+002", "foo  2.560000E+02");
    DoDoubleTest("foo %+E", neg,  "foo -2.560000E+002", "foo -2.560000E+02");
    DoDoubleTest("foo % E", neg,  "foo -2.560000E+002", "foo -2.560000E+02");

    PAL_Terminate();
    return PASS;
}
