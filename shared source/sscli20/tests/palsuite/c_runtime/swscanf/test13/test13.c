/*============================================================================
**
** Source:  test13.c
**
** Purpose: Tests swscanf with floats (decimal notation)
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
#include "../swscanf.h"

int __cdecl main(int argc, char *argv[])
{
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoFloatTest(convert("123.0"), convert("%f"), 123.0f);
    DoFloatTest(convert("123.0"), convert("%2f"), 12.0f);
    DoFloatTest(convert("10E1"), convert("%f"), 100.0f);
    DoFloatTest(convert("-12.01e-2"), convert("%f"), -0.1201f);
    DoFloatTest(convert("+12.01e-2"), convert("%f"), 0.1201f);
    DoFloatTest(convert("-12.01e+2"), convert("%f"), -1201.0f);
    DoFloatTest(convert("+12.01e+2"), convert("%f"), 1201.0f);
    DoFloatTest(convert("1234567890.0123456789f"), convert("%f"), 1234567936);

    PAL_Terminate();
    return PASS;
}
