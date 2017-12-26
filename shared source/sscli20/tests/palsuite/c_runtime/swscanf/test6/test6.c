/*============================================================================
**
** Source:  test6.c
**
** Purpose:Tests swscanf with octal numbers
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

    DoNumTest(convert("1234d"), convert("%o"), 668);
    DoNumTest(convert("1234d"), convert("%2o"), 10);
    DoNumTest(convert("-1"), convert("%o"), -1);
    DoNumTest(convert("0x1234"), convert("%o"), 0);
    DoNumTest(convert("012"), convert("%o"), 10);
    DoShortNumTest(convert("-1"), convert("%ho"), 65535);
    DoShortNumTest(convert("200000"), convert("%ho"), 0);
    DoNumTest(convert("-1"), convert("%lo"), -1);
    DoNumTest(convert("200000"), convert("%lo"), 65536);
    DoNumTest(convert("-1"), convert("%Lo"), -1);
    DoNumTest(convert("200000"), convert("%Lo"), 65536);
    DoI64NumTest(convert("40000000000"), convert("%I64o"), I64(4294967296));

    PAL_Terminate();
    return PASS;
}
