/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the atoi function.
**          Check to ensure that the different ints (normal,
**          negative, decimal,exponent), all work as expected with
**          this function.
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
    int IntValue;
    char avalue[20];
};

int __cdecl main(int argc, char **argv)
{

    int result=0;
    int i=0;

    struct testCase testCases[] =
        {
            {1234,  "1234"},
            {-1234, "-1234"},
            {1234,  "1234.44"},
            {1234,  "1234e-5"},
            {1234,  "1234e+5"},
            {1234,  "1234E5"},
            {1234,  "1234.657e-8"},
            {1234567,  "   1234567e-8 foo"},
            {0,     "aaa 32 test"}
        };

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* Loop through each case.  Convert the string to an int
       and then compare to ensure that it is the correct value.
    */

    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        /*Convert the string to an int.*/
        result = atoi(testCases[i].avalue);

        if (testCases[i].IntValue != result)
        {
            Fail("ERROR: atoi misinterpreted \"%s\" as %i instead of %i.\n"
                   , testCases[i].avalue, result, testCases[i].IntValue);
        }

    }


    PAL_Terminate();
    return PASS;
}













