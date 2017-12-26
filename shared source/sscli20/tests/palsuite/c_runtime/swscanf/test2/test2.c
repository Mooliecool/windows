/*============================================================================
**
** Source:  test2.c
**
** Purpose: Test to see if swscanf handles whitespace correctly
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

/* 
 * Tests out how it handles whitespace. Seems to accept anything that qualifies 
 * as isspace (space, tab, vertical tab, line feed, carriage return and form 
 * feed), even if it says it only wants spaces tabs and newlines. 
 */

int __cdecl main(int argc, char *argv[])
{

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoStrTest(convert("foo bar"), convert("foo %S"), "bar");
    DoStrTest(convert("foo\tbar"), convert("foo %S"), "bar");
    DoStrTest(convert("foo\nbar"), convert("foo %S"), "bar");
    DoStrTest(convert("foo\rbar"), convert("foo %S"), "bar");
    DoStrTest(convert("foo\vbar"), convert("foo %S"), "bar");
    DoStrTest(convert("foo\fbar"), convert("foo %S"), "bar");
    DoStrTest(convert("foo \t\n\r\v\fbar"), convert("foo %S"), "bar");

    PAL_Terminate();
    return PASS;
}
