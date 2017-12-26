/*=============================================================================
**
** Source: test1.c (modf)
**
** Purpose: Test to ensure that modf return the correct values
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**				 Fail
**               fabs
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
**===========================================================================*/

#include <palsuite.h>

#define DELTA 0.0000001 //Error acceptance level to the 7th decimal

/**
 * Helper test structure
 */
struct test
{
    double value;   // fmodf param 1
    double result1; // expected result (fractional portion)
    double result2; // expected result (integer portion)
};

/**
 * main
 * 
 * executable entry point
 */
INT __cdecl main(INT argc, CHAR **argv)
{
    INT i;

    struct test tests[] = 
    {
      // param 1   fractional   integer
        { 3,        0,          3  },
        { -10,      0,         -10 },
        { 1.1234,   0.1234,     1 },
        { -1.1234,  -0.1234,    -1 },
        { 1.7e308,  0,          1.7e308 },
        { -1.7e308, 0,         -1.7e308 },
        { 1.7e-30,  1.7e-30,    0 },
        { 0,        0,          0 }
    };


    // PAL initialization
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return FAIL;
    }

    for( i = 0; i < sizeof(tests) / sizeof(struct test); i++ )
    {
        double fractionalResult;
        double integerResult;
        double testDelta;

        fractionalResult = modf( tests[i].value, &integerResult );

        // The test is valid when the difference between the
        // result and the expectation is less than DELTA
        
        testDelta = fabs( fractionalResult - tests[i].result1 );

        if( (testDelta >= DELTA) ||
            (integerResult != tests[i].result2) )

        {
            Fail( "ERROR: "
                  "modf(%f) returned "
                  "fraction=%20.20f and integer=%20.20f "
                  "when it should have returned "
                  "fraction=%20.20f and integer=%20.20f ",
                  tests[i].value,
                  fractionalResult,
                  integerResult, 
                  tests[i].result1,
                  tests[i].result2 );
        }
    }

    PAL_Terminate();
    return PASS;
}













