/*============================================================================
**
** Source:  test17.c
**
** Purpose:Tests swprintf with compact format doubles (lowercase)
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
    double val = 2560.001;
    double neg = -2560.001;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoDoubleTest(convert("foo %g"), val,  convert("foo 2560"),
                 convert("foo 2560"));
    DoDoubleTest(convert("foo %lg"), val,  convert("foo 2560"),
                 convert("foo 2560"));
    DoDoubleTest(convert("foo %hg"), val,  convert("foo 2560"),
                 convert("foo 2560"));
    DoDoubleTest(convert("foo %Lg"), val,  convert("foo 2560"),
                 convert("foo 2560"));
    DoDoubleTest(convert("foo %I64g"), val,  convert("foo 2560"),
                 convert("foo 2560"));
    DoDoubleTest(convert("foo %5g"), val,  convert("foo  2560"),
                 convert("foo  2560"));
    DoDoubleTest(convert("foo %-5g"), val,  convert("foo 2560 "),
                 convert("foo 2560 "));
    DoDoubleTest(convert("foo %.1g"), val,  convert("foo 3e+003"),
                 convert("foo 3e+03"));
    DoDoubleTest(convert("foo %.2g"), val,  convert("foo 2.6e+003"),
                 convert("foo 2.6e+03"));
    DoDoubleTest(convert("foo %.12g"), val,  convert("foo 2560.001"),
                 convert("foo 2560.001"));
    DoDoubleTest(convert("foo %06g"), val,  convert("foo 002560"),
                 convert("foo 002560"));
    DoDoubleTest(convert("foo %#g"), val,  convert("foo 2560.00"),
                 convert("foo 2560.00"));
    DoDoubleTest(convert("foo %+g"), val,  convert("foo +2560"),
                 convert("foo +2560"));
    DoDoubleTest(convert("foo % g"), val,  convert("foo  2560"),
                 convert("foo  2560"));
    DoDoubleTest(convert("foo %+g"), neg,  convert("foo -2560"),
                 convert("foo -2560"));
    DoDoubleTest(convert("foo % g"), neg,  convert("foo -2560"),
                 convert("foo -2560"));

    PAL_Terminate();
    return PASS;
}
