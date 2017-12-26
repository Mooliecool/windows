/*============================================================================
**
** Source:  test8.c
**
** Purpose: Test #8 for the printf function. Tests the decimal
**          specifier (%d).
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
#include "../printf.h"



int __cdecl main(int argc, char *argv[])
{
    int neg = -42;
    int pos = 42;
    INT64 l = 42;
    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    DoNumTest("foo %d", pos, "foo 42");
    DoNumTest("foo %ld", 0xFFFF, "foo 65535");
    DoNumTest("foo %hd", 0xFFFF, "foo -1");
    DoNumTest("foo %Ld", pos, "foo 42");
    DoI64Test("foo %I64d", l, "42", "foo 42");
    DoNumTest("foo %3d", pos, "foo  42");
    DoNumTest("foo %-3d", pos, "foo 42 ");
    DoNumTest("foo %.1d", pos, "foo 42");
    DoNumTest("foo %.3d", pos, "foo 042");
    DoNumTest("foo %03d", pos, "foo 042");
    DoNumTest("foo %#d", pos, "foo 42");
    DoNumTest("foo %+d", pos, "foo +42");
    DoNumTest("foo % d", pos, "foo  42");
    DoNumTest("foo %+d", neg, "foo -42");
    DoNumTest("foo % d", neg, "foo -42");

    PAL_Terminate();
    return PASS;
}

