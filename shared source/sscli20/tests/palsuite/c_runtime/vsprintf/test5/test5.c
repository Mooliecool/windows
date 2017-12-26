/*=====================================================================
**
** Source:    test5.c
**
** Purpose:   Test #5 for the vsprintf function.
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

    DoTest("foo %n bar", 4, "foo  bar");
    DoTest("foo %#n bar", 4, "foo  bar");
    DoTest("foo % n bar", 4, "foo  bar");
    DoTest("foo %+n bar", 4, "foo  bar");
    DoTest("foo %-n bar", 4, "foo  bar");
    DoTest("foo %0n bar", 4, "foo  bar");
    DoShortTest("foo %hn bar", 4, "foo  bar");
    DoTest("foo %ln bar", 4, "foo  bar");
    DoTest("foo %Ln bar", 4, "foo  bar");
    DoTest("foo %I64n bar", 4, "foo  bar");
    DoTest("foo %20.3n bar", 4, "foo  bar");

    PAL_Terminate();
    return PASS;
}
