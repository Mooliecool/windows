/*============================================================================
**
** Source:  test4.c
**
** Purpose: Tests _snwprintf with pointers
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
#include "../_snwprintf.h"

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with wcslen */


int __cdecl main(int argc, char *argv[])
{
    void *ptr = (void*) 0x123456;
    INT64 lptr = I64(0x1234567887654321);
    
    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }


    DoPointerTest(convert("%p"), NULL, convert("00000000"));
    DoPointerTest(convert("%p"), ptr, convert("00123456"));
    DoPointerTest(convert("%9p"), ptr, convert(" 00123456"));
    DoPointerTest(convert("%09p"), ptr, convert(" 00123456"));
    DoPointerTest(convert("%-9p"), ptr, convert("00123456 "));
    DoPointerTest(convert("%+p"), ptr, convert("00123456"));
    DoPointerTest(convert("% p"), ptr, convert("00123456"));
    DoPointerTest(convert("%#p"), ptr, convert("0X00123456"));
    DoPointerTest(convert("%lp"), ptr, convert("00123456"));
    DoPointerTest(convert("%hp"), ptr, convert("00003456"));
    DoPointerTest(convert("%Lp"), ptr, convert("00123456"));
    DoI64Test(convert("%I64p"), lptr, "1234567887654321", 
        convert("1234567887654321"));

    PAL_Terminate();
    return PASS;
}

