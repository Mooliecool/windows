/*============================================================================
**
** Source:  test5.c
**
** Purpose: Test #5 for the printf function. Tests the count specifier (%n).
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
    char *longStr = 
        "really-long-string-that-just-keeps-going-on-and-on-and-on.."
        "..................useless-filler.................................."
        "..................useless-filler.................................."
        "..................useless-filler.................................."
        "%n bar";
    char *longResult = 
        "really-long-string-that-just-keeps-going-on-and-on-and-on.."
        "..................useless-filler.................................."
        "..................useless-filler.................................."
        "..................useless-filler.................................."
        " bar";

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoCountTest("foo %n bar", 4, "foo  bar");
    DoCountTest(longStr, 257, longResult);
    DoCountTest("fo%n bar", 2, "fo bar");
    DoCountTest("%n", 0, "");
    DoCountTest("foo %#n bar", 4, "foo  bar");
    DoCountTest("foo % n bar", 4, "foo  bar");
    DoCountTest("foo %+n bar", 4, "foo  bar");
    DoCountTest("foo %-n bar", 4, "foo  bar");
    DoCountTest("foo %0n bar", 4, "foo  bar");
    DoShortCountTest("foo %hn bar", 4, "foo  bar");
    DoCountTest("foo %ln bar", 4, "foo  bar");
    DoCountTest("foo %Ln bar", 4, "foo  bar");
    DoCountTest("foo %I64n bar", 4, "foo  bar");
    DoCountTest("foo %20.3n bar", 4, "foo  bar");

    PAL_Terminate();
   
    return PASS;
}
