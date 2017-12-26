/*============================================================================
**
** Source:  test3.c
**
** Purpose: Tests sscanf with bracketed set strings
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

    DoStrTest("bar1", "%[a-z]", "bar");
    DoStrTest("bar1", "%[z-a]", "bar");
    DoStrTest("bar1", "%[ab]", "ba");
    DoStrTest("bar1", "%[ar1b]", "bar1");
    DoStrTest("bar1", "%[^4]", "bar1");
    DoStrTest("bar1", "%[^4a]", "b");

    PAL_Terminate();
    return PASS;
}
