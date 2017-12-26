/*=====================================================================
**
** Source:  abs.c (test 1)
**
** Purpose: Tests the PAL implementation of the abs function.
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
**===================================================================*/

#include <palsuite.h>

struct TESTS 
{
    int nTest;
    int nResult;
};

int __cdecl main(int argc, char *argv[])
{
    int i = 0;
    int nRc = 0;
    struct TESTS testCase[] = 
    {
        {0, 0},
        {1, 1},
        {-1, 1}
    };


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    for (i = 0; i < (sizeof(testCase)/sizeof(struct TESTS)); i++)
    {
        nRc = abs(testCase[i].nTest);
        if (nRc != testCase[i].nResult)
        {
            Fail("abs: ERROR -> abs(%d) returned %d "
                "when it was expected to return %d \n",
                testCase[i].nTest,
                nRc,
                testCase[i].nResult);
        }
    }
    
    PAL_Terminate();
    return PASS;
}
