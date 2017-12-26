/*=====================================================================
**
** Source:    test11.c
**
** Purpose:   Test #11 for the vsprintf function.
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
#include "../vsprintf.h"

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

    DoNumTest("foo %u", pos, "foo 42");
    DoNumTest("foo %lu", 0xFFFF, "foo 65535");
    DoNumTest("foo %hu", 0xFFFF, "foo 65535");
    DoNumTest("foo %Lu", pos, "foo 42");
    DoI64Test("foo %I64u", l, "42", "foo 42");
    DoNumTest("foo %3u", pos, "foo  42");
    DoNumTest("foo %-3u", pos, "foo 42 ");
    DoNumTest("foo %.1u", pos, "foo 42");
    DoNumTest("foo %.3u", pos, "foo 042");
    DoNumTest("foo %03u", pos, "foo 042");
    DoNumTest("foo %#u", pos, "foo 42");
    DoNumTest("foo %+u", pos, "foo 42");
    DoNumTest("foo % u", pos, "foo 42");
    DoNumTest("foo %+u", neg, "foo 4294967254");
    DoNumTest("foo % u", neg, "foo 4294967254");

    PAL_Terminate();
    return PASS;
}
