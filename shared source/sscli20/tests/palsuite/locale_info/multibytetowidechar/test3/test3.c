/*============================================================================
**
** Source: test3.c
**
** Purpose: Tests that MultiByteToWideChar correctly handles the following 
**          error conditions: insufficient buffer space, invalid code pages,
**          and invalid flags.
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

    /* try with insufficient buffer space */
    ret = MultiByteToWideChar(CP_ACP, 0, mbStr, -1, wideStr, 10);
    if (ret != 0)
    {
        Fail("MultiByteToWideChar did not return an error!\n"
            "Expected return of 0, got %d", ret);
    }

    ret = GetLastError();
    if (ret != ERROR_INSUFFICIENT_BUFFER)
    {
        Fail("MultiByteToWideChar did not set the last error to "
            "ERROR_INSUFFICIENT_BUFFER!\n");
    }

    /* try with a wacky code page */
    ret = MultiByteToWideChar(-1, 0, mbStr, -1, wideStr, 128);
    if (ret != 0)
    {
        Fail("MultiByteToWideChar did not return an error!\n"
            "Expected return of 0, got %d", ret);
    }

    ret = GetLastError();
    if (ret != ERROR_INVALID_PARAMETER)
    {
        Fail("MultiByteToWideChar did not set the last error to "
            "ERROR_INVALID_PARAMETER!\n");
    }

    PAL_Terminate();

    return PASS;
}

