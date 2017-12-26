/*============================================================================
**
** Source:  test11.c
**
** Purpose: Tests swscanf with strings
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
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoWStrTest(convert("foo bar"), convert("foo %s"), convert("bar"));
    DoWStrTest(convert("foo bar"), convert("foo %2s"), convert("ba"));
    DoStrTest(convert("foo bar"), convert("foo %hs"), "bar");
    DoWStrTest(convert("foo bar"), convert("foo %ls"), convert("bar"));
    DoWStrTest(convert("foo bar"), convert("foo %Ls"), convert("bar"));
    DoWStrTest(convert("foo bar"), convert("foo %I64s"), convert("bar"));

    PAL_Terminate();
    return PASS;
}
