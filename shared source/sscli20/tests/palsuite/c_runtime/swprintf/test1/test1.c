/*============================================================================
**
** Source:  test1.c
**
** Purpose: General test to see if swprintf works correctly
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
#include "../swprintf.h"

/*
 * Uses memcmp & wcslen
 */

int __cdecl main(int argc, char *argv[])
{
    WCHAR *checkstr;
    WCHAR buf[256];

    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }

    checkstr = convert("hello world");
    swprintf(buf, convert("hello world"));

    if (memcmp(checkstr, buf, wcslen(checkstr)*2+2) != 0)
    {
        Fail("ERROR: Expected \"%s\", got \"%s\".\n", "hello world",
             convertC(buf));
    }

    PAL_Terminate();
    return PASS;
}
