/*============================================================================
**
** Source:  test4.c
**
** Purpose: Tests sscanf with decimal numbers
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

    DoNumTest("1234d", "%d", 1234);
    DoNumTest("1234d", "%2d", 12);
    DoNumTest("-1", "%d", -1);
    DoNumTest("0x1234", "%d", 0);
    DoNumTest("012", "%d", 12);
    DoShortNumTest("-1", "%hd", 65535);
    DoShortNumTest("65536", "%hd", 0);
    DoNumTest("-1", "%ld", -1);
    DoNumTest("65536", "%ld", 65536);
    DoNumTest("-1", "%Ld", -1);
    DoNumTest("65536", "%Ld", 65536);
    DoI64NumTest("4294967296", "%I64d", I64(4294967296));
    
    PAL_Terminate();
    return PASS;
}
