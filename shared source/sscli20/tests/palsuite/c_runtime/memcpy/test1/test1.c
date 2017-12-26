/*============================================================================
**
** Source:  test1.c
**
** Purpose: Calls memcpy and verifies that the buffer was copied correctly.
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
    char testA[20];
    char testB[20];
    void *retVal;
    long i;
   
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    memset(testA, 'a', 20);
    memset(testB, 'b', 20);

    retVal = (char *)memcpy(testB, testA, 0);
    if (retVal != testB)
    {
        Fail("memcpy should return a pointer to the destination buffer, "
             "but doesn't.\n");
    }
    for(i = 0; i<20; i++)
    {
        if (testB[i]!= 'b')
        {
            Fail("The destination buffer overflowed by memcpy.\n");
        }
    }

    retVal = (char *)memcpy(testB+1, testA, 18);
    if (retVal != testB+1)
    {
        Fail("memcpy should return a pointer to the destination buffer, "
             "but doesn't.\n");
    }

    if (testB[0] != 'b' || testB[19] != 'b')
    {
        Fail("The destination buffer was written out of bounds by memcpy!\n");
    }

    for(i = 1; i<19; i++)
    {
        if (testB[i]!= 'a')
        {
            Fail("The destination buffer copied to by memcpy doesn't match "
                 "the source buffer.\n");
        }
    }

    PAL_Terminate();

    return PASS;
}

















