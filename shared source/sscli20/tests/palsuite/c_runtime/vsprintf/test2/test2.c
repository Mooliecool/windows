/*=====================================================================
**
** Source:    test2.c
**
** Purpose:   Test #2 for the vsprintf function.
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

    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoStrTest("foo %s", "bar", "foo bar");  
    DoStrTest("foo %hs", "bar", "foo bar");
    DoWStrTest("foo %ls", convert("bar"), "foo bar");
    DoWStrTest("foo %ws", convert("bar"), "foo bar");
    DoStrTest("foo %Ls", "bar", "foo bar");
    DoStrTest("foo %I64s", "bar", "foo bar");
    DoStrTest("foo %5s", "bar", "foo   bar");
    DoStrTest("foo %.2s", "bar", "foo ba");
    DoStrTest("foo %5.2s", "bar", "foo    ba");
    DoStrTest("foo %-5s", "bar", "foo bar  ");
    DoStrTest("foo %05s", "bar", "foo 00bar");   

    PAL_Terminate();
    return PASS;
}
