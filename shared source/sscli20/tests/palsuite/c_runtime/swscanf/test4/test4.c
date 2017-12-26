/*============================================================================
**
** Source:  test4.c
**
** Purpose:Tests swscanf with decimal numbers
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

    DoNumTest(convert("1234d"), convert("%d"), 1234);
    DoNumTest(convert("1234d"), convert("%2d"), 12);
    DoNumTest(convert("-1"), convert("%d"), -1);
    DoNumTest(convert("0x1234"), convert("%d"), 0);
    DoNumTest(convert("012"), convert("%d"), 12);
    DoShortNumTest(convert("-1"), convert("%hd"), 65535);
    DoShortNumTest(convert("65536"), convert("%hd"), 0);
    DoNumTest(convert("-1"), convert("%ld"), -1);
    DoNumTest(convert("65536"), convert("%ld"), 65536);
    DoNumTest(convert("-1"), convert("%Ld"), -1);
    DoNumTest(convert("65536"), convert("%Ld"), 65536);
    DoI64NumTest(convert("4294967296"), convert("%I64d"), I64(4294967296));

    PAL_Terminate();
    return PASS;
}
