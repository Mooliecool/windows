/*============================================================================
**
** Source:  test12.c
**
** Purpose:  Tests sscanf with wide strings
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

    DoWStrTest("foo bar", "foo %S", convert("bar"));
    DoWStrTest("foo bar", "foo %2S", convert("ba"));
    DoStrTest("foo bar", "foo %hS", "bar");
    DoWStrTest("foo bar", "foo %lS", convert("bar"));
    DoWStrTest("foo bar", "foo %LS", convert("bar"));
    DoWStrTest("foo bar", "foo %I64S", convert("bar"));

    PAL_Terminate();
    return PASS;
}
