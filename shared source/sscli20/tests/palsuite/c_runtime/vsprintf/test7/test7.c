/*=====================================================================
**
** Source:    test7.c
**
** Purpose:   Test #7 for the vsprintf function.
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
    WCHAR wb = (WCHAR) 'b';
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoWCharTest("foo %c", wb, "foo b");
    DoWCharTest("foo %hc", wb, "foo b");
    DoCharTest("foo %lc", 'c', "foo c");
    DoWCharTest("foo %Lc", wb, "foo b");
    DoWCharTest("foo %I64c", wb, "foo b");
    DoWCharTest("foo %5c", wb, "foo     b");
    DoWCharTest("foo %.0c", wb, "foo b");
    DoWCharTest("foo %-5c", wb, "foo b    ");
    DoWCharTest("foo %05c", wb, "foo 0000b");
    DoWCharTest("foo % c", wb, "foo b");
    DoWCharTest("foo %#c", wb, "foo b");

    PAL_Terminate();
    return PASS;
}
