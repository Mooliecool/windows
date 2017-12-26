/*============================================================================
**
** Source:  test1.c
**
** Purpose: Test that malloc returns useable memory
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

    char *testA;
    int i;
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* check that malloc really gives us addressable memory */
    testA = (char *)malloc(20 * sizeof(char));
    if (testA == NULL)
    {
        Fail("Call to malloc failed.\n");
    }
    for (i = 0; i < 20; i++)
    {
        testA[i] = 'a';
    }
    for (i = 0; i < 20; i++)
    {
        if (testA[i] != 'a')
        {
            Fail("The memory doesn't seem to be properly allocated.\n");
        }
    }
    free(testA);

    PAL_Terminate();

    return PASS;
}



