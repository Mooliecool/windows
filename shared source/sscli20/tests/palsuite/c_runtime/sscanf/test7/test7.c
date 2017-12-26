/*============================================================================
**
** Source:  test7.c
**
** Purpose:  Tests sscanf with hex numbers (lowercase)
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

    DoNumTest("1234i", "%x", 0x1234);
    DoNumTest("1234i", "%2x", 0x12);
    DoNumTest("-1", "%x", -1);
    DoNumTest("0x1234", "%x", 0x1234);
    DoNumTest("012", "%x", 0x12);
    DoShortNumTest("-1", "%hx", 65535);
    DoShortNumTest("10000", "%hx", 0);
    DoNumTest("-1", "%lx", -1);
    DoNumTest("10000", "%lx", 65536);
    DoNumTest("-1", "%Lx", -1);
    DoNumTest("10000", "%Lx", 65536);
    DoI64NumTest("100000000", "%I64x", I64(4294967296));

    PAL_Terminate();
    return PASS;
}
