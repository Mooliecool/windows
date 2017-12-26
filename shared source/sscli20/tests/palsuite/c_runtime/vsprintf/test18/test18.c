/*=====================================================================
**
** Source:    test18.c
**
** Purpose:   Test #18 for the vsprintf function.
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
    double val = 2560.001;
    double neg = -2560.001;

   if (PAL_Initialize(argc, argv) != 0)
   {
        return(FAIL);
   }

    DoDoubleTest("foo %G", val,  "foo 2560", "foo 2560");
    DoDoubleTest("foo %lG", val,  "foo 2560", "foo 2560");
    DoDoubleTest("foo %hG", val,  "foo 2560", "foo 2560");
    DoDoubleTest("foo %LG", val,  "foo 2560", "foo 2560");
    DoDoubleTest("foo %I64G", val,  "foo 2560", "foo 2560");
    DoDoubleTest("foo %5G", val,  "foo  2560", "foo  2560");
    DoDoubleTest("foo %-5G", val,  "foo 2560 ", "foo 2560 ");
    DoDoubleTest("foo %.1G", val,  "foo 3E+003", "foo 3E+03");
    DoDoubleTest("foo %.2G", val,  "foo 2.6E+003", "foo 2.6E+03");
    DoDoubleTest("foo %.12G", val,  "foo 2560.001", "foo 2560.001");
    DoDoubleTest("foo %06G", val,  "foo 002560", "foo 002560");
    DoDoubleTest("foo %#G", val,  "foo 2560.00", "foo 2560.00");
    DoDoubleTest("foo %+G", val,  "foo +2560", "foo +2560");
    DoDoubleTest("foo % G", val,  "foo  2560", "foo  2560");
    DoDoubleTest("foo %+G", neg,  "foo -2560", "foo -2560");
    DoDoubleTest("foo % G", neg,  "foo -2560", "foo -2560");

    PAL_Terminate();
    return PASS;
}
