/*============================================================================
**
** Source:  test1.c
**
** Purpose: Check that memset correctly fills a destination buffer
**          without overflowing it.
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

int __cdecl main(int argc, char **argv)
{

    char testA[22] = "bbbbbbbbbbbbbbbbbbbbb";
    char *retVal;

    int i;
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    retVal = (char *)memset(testA, 'a', 20);
    if (retVal != testA) 
    {
        Fail("memset should have returned the value of the destination"
             "pointer, but didn't");
    }

    for(i = 0; i<20; i++)
    {
        if (testA[i]!= 'a')
        {
            Fail("memset didn't set the destination bytes.\n");
        }
    }
    if (testA[20] == 'a')
    {
        Fail("memset overfilled the destination buffer.\n");
    }

    PAL_Terminate();
    return PASS;
}










