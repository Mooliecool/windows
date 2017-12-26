/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Call labs on a series of values -- negative, positive, zero,
** and the largest negative value of a long.  Ensure that they are all
** changed properly to their absoulte value. 
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

struct testCase
{
    long LongValue;
    long AbsoluteLongValue;
};

int __cdecl main(int argc, char **argv)
{

    long result=0;
    int i=0;

    struct testCase testCases[] =
        {
            {1234,  1234},
            {-1234, 1234},
            {0,     0},
            {-2147483647, 2147483647},  /* Max value to abs */
            {2147483647, 2147483647}
        };

    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* Loop through each case. Call labs on each long and ensure that
       the resulting value is correct.
    */

    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        /* Absolute value on a long */ 
        result = labs(testCases[i].LongValue);

        if (testCases[i].AbsoluteLongValue != result)
        {
            Fail("ERROR: labs took the absoulte value of '%d' to be '%d' "
                 "instead of %d.\n", 
                 testCases[i].LongValue, 
                 result, 
                 testCases[i].AbsoluteLongValue);
        }
    }

    PAL_Terminate();
    return PASS;
}
