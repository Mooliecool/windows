/*=====================================================================
**
** Source:    test9.c
**
** Purpose:   Test #9 for the _vsnprintf function.
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
**===================================================================*/

#include <palsuite.h>
#include "../_vsnprintf.h"

/*
 * Notes: memcmp is used, as is strlen.
 */


int __cdecl main(int argc, char *argv[])
{
    int neg = -42;
    int pos = 42;
    INT64 l = 42;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoNumTest("foo %i", pos, "foo 42");
    DoNumTest("foo %li", 0xFFFF, "foo 65535");
    DoNumTest("foo %hi", 0xFFFF, "foo -1");
    DoNumTest("foo %Li", pos, "foo 42");
    DoI64Test("foo %I64i", l, "42", "foo 42");
    DoNumTest("foo %3i", pos, "foo  42");
    DoNumTest("foo %-3i", pos, "foo 42 ");
    DoNumTest("foo %.1i", pos, "foo 42");
    DoNumTest("foo %.3i", pos, "foo 042");
    DoNumTest("foo %03i", pos, "foo 042");
    DoNumTest("foo %#i", pos, "foo 42");
    DoNumTest("foo %+i", pos, "foo +42");
    DoNumTest("foo % i", pos, "foo  42");
    DoNumTest("foo %+i", neg, "foo -42");
    DoNumTest("foo % i", neg, "foo -42");

    PAL_Terminate();
    return PASS;
}
