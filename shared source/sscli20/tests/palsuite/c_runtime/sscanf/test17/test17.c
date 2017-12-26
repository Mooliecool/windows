/*============================================================================
**
** Source:  test17.c
**
** Purpose: Tests sscanf with floats (compact notation, uppercase)
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
#include "../sscanf.h"

int __cdecl main(int argc, char *argv[])
{
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoFloatTest("123.0", "%G", 123.0f);
    DoFloatTest("123.0", "%2G", 12.0f);
    DoFloatTest("10E1", "%G", 100.0f);
    DoFloatTest("-12.01e-2", "%G", -0.1201f);
    DoFloatTest("+12.01e-2", "%G", 0.1201f);
    DoFloatTest("-12.01e+2", "%G", -1201.0f);
    DoFloatTest("+12.01e+2", "%G", 1201.0f);
    DoFloatTest("1234567890.0123456789G", "%G", 1234567936);
    
    PAL_Terminate();
    return PASS;
}
