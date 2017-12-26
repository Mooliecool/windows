/*============================================================================
**
** Source:  test3.c
**
** Purpose: Tests swscanf with bracketed set strings
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

    DoWStrTest(convert("bar1"), convert("%[a-z]"), convert("bar"));
    DoWStrTest(convert("bar1"), convert("%[z-a]"), convert("bar"));
    DoWStrTest(convert("bar1"), convert("%[ab]"), convert("ba"));
    DoWStrTest(convert("bar1"), convert("%[ar1b]"), convert("bar1"));
    DoWStrTest(convert("bar1"), convert("%[^4]"), convert("bar1"));
    DoWStrTest(convert("bar1"), convert("%[^4a]"), convert("b"));

    PAL_Terminate();
    return PASS;
}
