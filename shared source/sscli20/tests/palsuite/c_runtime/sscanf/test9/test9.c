/*============================================================================
**
** Source:  test9.c
**
** Purpose: Tests sscanf with characters
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

    DoCharTest("1234d", "%c", "1", 1);
    DoCharTest("1234d", "%c", "1", 1);
    DoCharTest("abc", "%2c", "ab", 2);
    DoCharTest(" ab", "%c", " ", 1);
    DoCharTest("ab", "%hc", "a", 1);
    DoWCharTest("ab", "%lc", convert("a"), 1);
    DoCharTest("ab", "%Lc", "a", 1);
    DoCharTest("ab", "%I64c", "a", 1);

    PAL_Terminate();
    return PASS;
}
