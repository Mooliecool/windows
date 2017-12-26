/*============================================================================
**
** Source:  test7.c
**
** Purpose: Test #7 for the vprintf function. Tests the wide char
**          specifier (%C).
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
#include "../vprintf.h"



int __cdecl main(int argc, char *argv[])
{
    WCHAR wb = (WCHAR) 'b';
    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    DoWCharTest("foo %C", wb, "foo b");
    DoWCharTest("foo %hC", wb, "foo b");
    DoCharTest("foo %lC", 'c', "foo c");
    DoWCharTest("foo %LC", wb, "foo b");
    DoWCharTest("foo %I64C", wb, "foo b");
    DoWCharTest("foo %5C", wb, "foo     b");
    DoWCharTest("foo %.0C", wb, "foo b");
    DoWCharTest("foo %-5C", wb, "foo b    ");
    DoWCharTest("foo %05C", wb, "foo 0000b");
    DoWCharTest("foo % C", wb, "foo b");
    DoWCharTest("foo %#C", wb, "foo b");

    PAL_Terminate();
    return PASS;
}

