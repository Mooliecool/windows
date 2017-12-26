/*============================================================================
**
** Source:  test12.c
**
** Purpose: Test #12 for the sprintf function. Tests the (lowercase)
**          hexadecimal specifier (%x)
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
#include "../sprintf.h"

/* 
 * Depends on memcmp and strlen
 */

int __cdecl main(int argc, char *argv[])
{
    int neg = -42;
    int pos = 0x1234ab;
    INT64 l = I64(0x1234567887654321);
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoNumTest("foo %x", pos, "foo 1234ab");
    DoNumTest("foo %lx", pos, "foo 1234ab");
    DoNumTest("foo %hx", pos, "foo 34ab");
    DoNumTest("foo %Lx", pos, "foo 1234ab");
    DoI64Test("foo %I64x", l, "0x1234567887654321",
              "foo 1234567887654321");
    DoNumTest("foo %7x", pos, "foo  1234ab");
    DoNumTest("foo %-7x", pos, "foo 1234ab ");
    DoNumTest("foo %.1x", pos, "foo 1234ab");
    DoNumTest("foo %.7x", pos, "foo 01234ab");
    DoNumTest("foo %07x", pos, "foo 01234ab");
    DoNumTest("foo %#x", pos, "foo 0x1234ab");
    DoNumTest("foo %+x", pos, "foo 1234ab");
    DoNumTest("foo % x", pos, "foo 1234ab");
    DoNumTest("foo %+x", neg, "foo ffffffd6");
    DoNumTest("foo % x", neg, "foo ffffffd6");

    PAL_Terminate();
    return PASS;
}

