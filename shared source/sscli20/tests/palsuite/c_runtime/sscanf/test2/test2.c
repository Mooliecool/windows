/*============================================================================
**
** Source:  test2.c
**
** Purpose: Test to see if sscanf handles whitespace correctly
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

    DoStrTest("foo bar", "foo %s", "bar");
    DoStrTest("foo\tbar", "foo %s", "bar");
    DoStrTest("foo\nbar", "foo %s", "bar");
    DoStrTest("foo\rbar", "foo %s", "bar");
    DoStrTest("foo\vbar", "foo %s", "bar");
    DoStrTest("foo\fbar", "foo %s", "bar");
    DoStrTest("foo \t\n\r\v\fbar", "foo %s", "bar");    

    PAL_Terminate();
    return PASS;
}
