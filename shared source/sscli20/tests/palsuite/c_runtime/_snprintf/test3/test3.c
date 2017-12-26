/*============================================================================
**
** Source:  test3.c
**
** Purpose: Tests _snprintf with wide strings
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
#include "../_snprintf.h"

/*
 * Notes: memcmp is used, as is strlen.
 */

int __cdecl main(int argc, char *argv[])
{

    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }

    DoWStrTest("foo %S", convert("bar"), "foo bar");
    DoStrTest("foo %hS", "bar", "foo bar");
    DoWStrTest("foo %lS", convert("bar"), "foo bar");
    DoWStrTest("foo %wS", convert("bar"), "foo bar");
    DoWStrTest("foo %LS", convert("bar"), "foo bar");
    DoWStrTest("foo %I64S", convert("bar"), "foo bar");
    DoWStrTest("foo %5S", convert("bar"), "foo   bar");
    DoWStrTest("foo %.2S", convert("bar"), "foo ba");
    DoWStrTest("foo %5.2S", convert("bar"), "foo    ba");
    DoWStrTest("foo %-5S", convert("bar"), "foo bar  ");
    DoWStrTest("foo %05S", convert("bar"), "foo 00bar");
    
    PAL_Terminate();
    return PASS;
}
