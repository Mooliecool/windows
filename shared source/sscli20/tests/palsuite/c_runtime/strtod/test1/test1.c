/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the strtod function.
**          Convert a number of strings to doubles.  Ensure they
**          convert correctly.
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
    double CorrectResult;  /* The returned double value */
    char ResultString[20]; /* The remainder string */
    char string[20];       /* The test string */
};


int __cdecl main(int argc, char **argv)
{

    char * endptr;
    double result;  
    int i;
  
    struct testCase testCases[] = 
        {
            {1234,"","1234"},
            {-1234,"","-1234"},
            {1234.44,"","1234.44"},
            {1234e-5,"","1234e-5"},
            {1234e+5,"","1234e+5"},
            {12345E5,"","12345e5"},
            {1234.657e-8,"","1234.657e-8"},
            {1234567e-8,"foo","1234567e-8foo"},
            {999,"foo","999 foo"},
            {7,"foo"," 7foo"},
            {0,"a7","a7"},
            {-777777,"z zz","-777777z zz"}
        };
  
    /*
     *  Initialize the PAL
     */
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
  
    /* Loop through the structure to test each case */
    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        result = strtod(testCases[i].string,&endptr);
      
        /* need to check the result and the endptr result */
        if ((testCases[i].CorrectResult != result) &&
           (strcmp(testCases[i].ResultString,endptr)!=0))
        {
            Fail("ERROR:  strtod returned %f instead of %f and "
                 "\"%s\" instead of \"%s\" for the test of \"%s\"\n",
                   result, 
                 testCases[i].CorrectResult,
                 endptr,
                 testCases[i].ResultString,
                 testCases[i].string);
        }
      
    }      
  
    PAL_Terminate();
    return PASS;
} 













