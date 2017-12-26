/*============================================================================
**
** Source:  test1.c
**
** Purpose: General test of swscanf
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


int __cdecl main(int argc, char *argv[])
{
    int num;
    int ret;
    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    DoVoidTest(convert("foo bar"), convert("foo"));
    DoVoidTest(convert("foo bar"), convert("baz"));
    DoVoidTest(convert("foo bar"), convert("foo %*s"));

    DoStrTest(convert("foo % bar"), convert("foo %% %S"), "bar");
    DoStrTest(convert("foo bar baz"), convert("foo %bar %S"), "baz");

    DoVoidTest(convert("foo bar baz"), convert("foo % bar %S"));
    DoVoidTest(convert("foo bar baz"), convert("foo% bar %S"));


    ret = swscanf(convert("foo bar baz"), convert("foo bar %n"), &num);
    if (ret != 0 || num != 8)
    {
        Fail("ERROR: Got incorrect values in scanning \"%s\" using \"%s\".\n"
            "Expected to get a value of %d with return value of %d, "
            "got %d with return %d\n", "foo bar baz", "foo bar %n", 8, 0, 
            num, ret);
    }

    PAL_Terminate();
    return PASS;
}
