/*============================================================================
**
** Source:  test2.c
**
** Purpose: Test that malloc(0) returns non-zero value
** Bug: VSW 177209
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

    /* check that malloc(0) returns non-zero value */
    testA = (char *)malloc(0);
    if (testA == NULL)
    {
        Fail("Call to malloc(0) failed.\n");
    }

    free(testA);

    PAL_Terminate();

    return PASS;
}



