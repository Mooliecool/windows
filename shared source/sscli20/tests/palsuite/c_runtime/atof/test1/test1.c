/*============================================================================
**
** Source:  test1.c
**
** Purpose: Passes to atof() a series of strings containing floats,
**          checking that each one is correctly extracted.
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

struct testCase
{
    float fvalue;
    char avalue[20];
};

int __cdecl main(int argc, char **argv)
{
    int i=0;
    double f=0;
    struct testCase testCases[] =
    {
        {1234, "1234"},
        {-1234, "-1234"},
        {1234e-5, "1234e-5"},
        {1234e+5, "1234e+5"},
        {1234e5, "1234E5"},
        {1234.567e-8, "1234.657e-8"},
        {1234.567e-8, "   1234567e-8 foo"},
        {0,"a12"}
    };

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        /*Convert the string to a float.*/
        f = atof(testCases[i].avalue);
        if (abs(f-testCases[i].fvalue) > 0)
        {
            Fail ("atof misinterpreted \"%s\" as %f instead of %f.\n"
                    , testCases[i].avalue, f, testCases[i].fvalue);
        }
    }
    PAL_Terminate();
    return PASS;
}













