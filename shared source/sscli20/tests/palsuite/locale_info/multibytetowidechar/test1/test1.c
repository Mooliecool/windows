/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests MultiByteToWideChar with all the ASCII characters (0-127).
**          Also tests that WideCharToMultiByte handles different buffer
**          lengths correctly (0, -1, and a valid length)
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
        mbStr[i] = 127 - i;
        wideStr[i] = 0;
    }


    ret = MultiByteToWideChar(CP_ACP, 0, mbStr, -1, wideStr, 0);
    if (ret != 128)
    {
        Fail("MultiByteToWideChar did not return correct string length!\n"
            "Got %d, expected %d\n", ret, 128);
    }

    /* Make sure the ASCII set (0-127) gets translated correctly */
    ret = MultiByteToWideChar(CP_ACP, 0, mbStr, -1, wideStr, 128);
    if (ret != 128)
    {
        Fail("MultiByteToWideChar did not return correct string length!\n"
            "Got %d, expected %d\n", ret, 128);
    }

    for (i=0; i<128; i++)
    {
        if (wideStr[i] != (WCHAR)(127 - i))
        {
            Fail("MultiByteToWideChar failed to translate correctly!\n"
                "Expected character %d to be %c (%x), got %c (%x)\n",
                i, 127 - i, 127 - i,wideStr[i], wideStr[i]);
        }
    }


    /* try a 0 length string */
    mbStr[0] = 0;
    ret = MultiByteToWideChar(CP_ACP, 0, mbStr, -1, wideStr, 0);
    if (ret != 1)
    {
        Fail("MultiByteToWideChar did not return correct string length!\n"
            "Got %d, expected %d\n", ret, 1);
    }

    PAL_Terminate();

    return PASS;
}

