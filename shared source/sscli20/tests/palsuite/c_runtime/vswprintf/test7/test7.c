/*=====================================================================
**
** Source:    test7.c
**
** Purpose:   Test #7 for the vswprintf function.
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
#include "../vswprintf.h"

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with wcslen */

int __cdecl main(int argc, char *argv[])
{
    WCHAR wc = (WCHAR) 'c';
    
    if (PAL_Initialize(argc, argv) != 0)
        return(FAIL);

    DoCharTest(convert("foo %C"), 'b', convert("foo b"));
    DoWCharTest(convert("foo %hC"), wc, convert("foo c"));
    DoCharTest(convert("foo %lC"), 'b', convert("foo b"));
    DoCharTest(convert("foo %LC"), 'b', convert("foo b"));
    DoCharTest(convert("foo %I64C"), 'b', convert("foo b"));
    DoCharTest(convert("foo %5C"), 'b', convert("foo     b"));
    DoCharTest(convert("foo %.0C"), 'b', convert("foo b"));
    DoCharTest(convert("foo %-5C"), 'b', convert("foo b    "));
    DoCharTest(convert("foo %05C"), 'b', convert("foo 0000b"));
    DoCharTest(convert("foo % C"), 'b', convert("foo b"));
    DoCharTest(convert("foo %#C"), 'b', convert("foo b"));

    PAL_Terminate();
    return PASS;
}
