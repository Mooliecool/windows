/*============================================================================
**
** Source: test2.c
**
** Purpose: Tests that MultiByteToWideChar respects the length of the wide 
**          character string.
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

/*
 * For now, it is assumed that MultiByteToWideChar will only be used in the PAL
 * with CP_ACP, and that dwFlags will be 0.
 */

int __cdecl main(int argc, char *argv[])
{    
    char mbStr[128];
    WCHAR wideStr[128];
    int ret;
    int i;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    for (i=0; i<128; i++)
    {
        mbStr[i] = 'a';
        wideStr[i] = 0;
    }

    mbStr[127] = 0;


    ret = MultiByteToWideChar(CP_ACP, 0, mbStr, 10, wideStr, 0);
    if (ret != 10)
    {
        Fail("MultiByteToWideChar did not return correct string length!\n"
            "Got %d, expected %d\n", ret, 10);
    }

    wideStr[10] = (WCHAR) 'b';

    ret = MultiByteToWideChar(CP_ACP, 0, mbStr, 10, wideStr, 128);
    if (ret != 10)
    {
        Fail("MultiByteToWideChar did not return correct string length!\n"
            "Got %d, expected %d\n", ret, 10);
    }

    if (wideStr[10] != 'b')
    {
        Fail("WideCharToMultiByte overflowed the destination buffer!\n");
    }

    PAL_Terminate();

    return PASS;
}

