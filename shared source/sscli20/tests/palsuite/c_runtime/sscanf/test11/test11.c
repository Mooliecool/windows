/*============================================================================
**
** Source:  test11.c
**
** Purpose: Tests sscanf with strings
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

int __cdecl main(int argc, char *argv[])
{

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoStrTest("foo bar", "foo %s", "bar");
    DoStrTest("foo bar", "foo %2s", "ba");
    DoStrTest("foo bar", "foo %hs", "bar");
    DoWStrTest("foo bar", "foo %ls", convert("bar"));
    DoStrTest("foo bar", "foo %Ls", "bar");
    DoStrTest("foo bar", "foo %I64s", "bar");

    PAL_Terminate();
    return PASS;
}
