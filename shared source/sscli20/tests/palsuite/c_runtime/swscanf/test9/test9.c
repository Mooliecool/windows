/*============================================================================
**
** Source:  test9.c
**
** Purpose: Tests swscanf with characters
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

    DoWCharTest(convert("1234"), convert("%c"), convert("1"), 1);
    DoWCharTest(convert("1234"), convert("%c"), convert("1"), 1);
    DoWCharTest(convert("abc"), convert("%2c"), convert("ab"), 2);
    DoWCharTest(convert(" ab"), convert("%c"), convert(" "), 1);
    DoCharTest(convert("ab"), convert("%hc"), "a", 1);
    DoWCharTest(convert("ab"), convert("%lc"), convert("a"), 1);
    DoWCharTest(convert("ab"), convert("%Lc"), convert("a"), 1);
    DoWCharTest(convert("ab"), convert("%I64c"), convert("a"), 1);

    PAL_Terminate();
    return PASS;
}
