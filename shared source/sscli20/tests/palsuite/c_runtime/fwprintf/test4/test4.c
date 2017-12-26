/*============================================================================
**
** Source:      test4.c
**
** Purpose:     Tests the pointer specifier (%p).
**              This test is modeled after the sprintf series.
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
#include "../fwprintf.h"

/* 
 * Depends on memcmp, strlen, fopen, fseek and fgets.
 */

int __cdecl main(int argc, char *argv[])
{
    void *ptr = (void*) 0x123456;
    INT64 lptr = I64(0x1234567887654321);

    if (PAL_Initialize(argc, argv) != 0)
    {
        return(FAIL);
    }

    DoPointerTest(convert("%p"), NULL, "NULL", "00000000", "0x0");
    DoPointerTest(convert("%p"), ptr, "pointer to 0x123456", "00123456", 
        "0x123456");
    DoPointerTest(convert("%9p"), ptr, "pointer to 0x123456", " 00123456", 
        " 0x123456");
    DoPointerTest(convert("%09p"), ptr, "pointer to 0x123456", " 00123456", 
        "0x0123456");
    DoPointerTest(convert("%-9p"), ptr, "pointer to 0x123456", "00123456 ", 
        "0x123456 ");
    DoPointerTest(convert("%+p"), ptr, "pointer to 0x123456", "00123456", 
        "0x123456");
    DoPointerTest(convert("%#p"), ptr, "pointer to 0x123456", "0X00123456", 
        "0x123456");
    DoPointerTest(convert("%lp"), ptr, "pointer to 0x123456", "00123456", 
        "0x123456");
    DoPointerTest(convert("%hp"), ptr, "pointer to 0x123456", "00003456", 
        "0x3456");
    DoPointerTest(convert("%Lp"), ptr, "pointer to 0x123456", "00123456", 
        "0x123456");
    DoI64Test(convert("%I64p"), lptr, "pointer to 0x1234567887654321", 
        "1234567887654321", "0x1234567887654321");

    PAL_Terminate();
    return PASS;
}
