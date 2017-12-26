/*=============================================================================
**
** Source: test1.c
**
** Purpose: Test to ensure that floor return the correct values
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**				 Fail
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

/**
 * Helper test structure
 */
struct test
{
    double value; // floor param 1
    double result; // expected result
};

/**
 * main
 * 
 * executable entry point
 */
INT __cdecl main(INT argc, CHAR **argv)
{
    int i;

    struct test tests[] = 
    {
      // param 1         result
        { 3,                3 },
        { -10,            -10 },
        { 0,                0 },
        { 1.7e308,          1.7e308 },
        { -1.7e308,        -1.7e308 },
        { 4.94e-324,        0 },
        { -4.94e-324,      -1 },
        { 1234.1234,     1234 },
        { -1234.1234,   -1235 },
        {-0,                0 }
    };


    // PAL initialization
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return FAIL;
    }

    for( i = 0; i < sizeof(tests) / sizeof(struct test); i++)
    {
        double result;

        result = floor( tests[i].value );

        if( result != tests[i].result )
        {
            Fail( "floor(%f) returned %f"
                  " when it should have returned %f",
                  tests[i].value,
                  result, 
                  tests[i].result );
        }
    }

    PAL_Terminate();
    return PASS;
}













