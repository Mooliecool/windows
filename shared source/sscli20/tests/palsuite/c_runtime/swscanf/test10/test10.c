/*============================================================================
**
** Source:  test10.c
**
** Purpose:Tests swscanf with wide characters 
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

    DoCharTest(convert("1234"), convert("%C"), "1", 1);
    DoCharTest(convert("abc"), convert("%2C"), "ab", 2);
    DoCharTest(convert(" ab"), convert("%C"), " ", 1);
    DoCharTest(convert("ab"), convert("%hC"), "a", 1);
    DoWCharTest(convert("ab"), convert("%lC"), convert("a"), 1);
    DoCharTest(convert("ab"), convert("%LC"), "a", 1);
    DoCharTest(convert("ab"), convert("%I64C"), "a", 1);

    PAL_Terminate();
    return PASS;
}
