/*=====================================================================
**
** Source:    test6.c
**
** Purpose:   Test #6 for the _vsnwprintf function.
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
#include "../_vsnwprintf.h"

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with wcslen */

int __cdecl main(int argc, char *argv[])
{
    WCHAR wc = (WCHAR) 'c';
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoWCharTest(convert("foo %c"), wc, convert("foo c"));
    DoCharTest(convert("foo %hc"), 'b', convert("foo b"));
    DoWCharTest(convert("foo %lc"), wc, convert("foo c"));
    DoWCharTest(convert("foo %Lc"), wc, convert("foo c"));
    DoWCharTest(convert("foo %I64c"), wc, convert("foo c"));
    DoWCharTest(convert("foo %5c"), wc, convert("foo     c"));
    DoWCharTest(convert("foo %.0c"), wc, convert("foo c"));
    DoWCharTest(convert("foo %-5c"), wc, convert("foo c    "));
    DoWCharTest(convert("foo %05c"), wc, convert("foo 0000c"));
    DoWCharTest(convert("foo % c"), wc, convert("foo c"));
    DoWCharTest(convert("foo %#c"), wc, convert("foo c"));

    PAL_Terminate();
    return PASS;
}
