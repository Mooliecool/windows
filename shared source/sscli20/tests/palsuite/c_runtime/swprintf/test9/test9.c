/*============================================================================
**
** Source:  test9.c
**
** Purpose: Tests swprintf with integer numbers
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
#include "../swprintf.h"

/*
 * Uses memcmp & wcslen
 */

int __cdecl main(int argc, char *argv[])
{
    int neg = -42;
    int pos = 42;
    INT64 l = 42;
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoNumTest(convert("foo %i"), pos, convert("foo 42"));
    DoNumTest(convert("foo %li"), 0xFFFF, convert("foo 65535"));
    DoNumTest(convert("foo %hi"), 0xFFFF, convert("foo -1"));
    DoNumTest(convert("foo %Li"), pos, convert("foo 42"));
    DoI64Test(convert("foo %I64i"), l, "0x0000000000000042",
              convert("foo 42"));
    DoNumTest(convert("foo %3i"), pos, convert("foo  42"));
    DoNumTest(convert("foo %-3i"), pos, convert("foo 42 "));
    DoNumTest(convert("foo %.1i"), pos, convert("foo 42"));
    DoNumTest(convert("foo %.3i"), pos, convert("foo 042"));
    DoNumTest(convert("foo %03i"), pos, convert("foo 042"));
    DoNumTest(convert("foo %#i"), pos, convert("foo 42"));
    DoNumTest(convert("foo %+i"), pos, convert("foo +42"));
    DoNumTest(convert("foo % i"), pos, convert("foo  42"));
    DoNumTest(convert("foo %+i"), neg, convert("foo -42"));
    DoNumTest(convert("foo % i"), neg, convert("foo -42"));

    PAL_Terminate();
    return PASS;
}

