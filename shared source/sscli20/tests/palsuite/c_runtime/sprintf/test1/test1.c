/*============================================================================
**
** Source:  test1.c
**
** Purpose: Test #1 for the sprintf function. A single, basic, test
**          case with no formatting.
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
#include "../sprintf.h"

/* 
 * Depends on memcmp and strlen
 */

int __cdecl main(int argc, char *argv[])
{
    char checkstr[] = "hello world";
    char buf[256];

    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    sprintf(buf, "hello world");

    if (memcmp(checkstr, buf, strlen(checkstr)+1) != 0)
    {
        Fail("ERROR: expected %s, got %s\n", checkstr, buf);
    }

    PAL_Terminate();
    return PASS;
}

