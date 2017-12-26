/*============================================================================
**
** Source:  test1.c
**
** Purpose: Repeatedly allocates and frees a chunk of memory, to verify
**          that free is really returning memory to the heap
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

    long i;
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    /* check that free really returns memory to the heap. */
    for(i=1; i<1000000; i++)
    {
        testA = (char *)malloc(1000*sizeof(char));
        if (testA==NULL)
        {
            Fail("Either free is failing to return memory to the heap, or"
                 " the system is running out of memory for some other "
                 "reason.\n");
        }
        free(testA);
    }

    free(NULL); /*should do nothing*/
    PAL_Terminate();
    return PASS;
}

















