/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests wcstod with a number of sample strings.
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
    double CorrectResult;
    char string[20];
    int stopChar;
};

struct testCase testCases[] = 
{
    {1234,"1234", 4},
    {-1234,"-1234", 5},
    {1234.44,"1234.44", 7},
    {1234e-5,"1234e-5", 7},
    {1234e+5,"1234e+5", 7},
    {1234E5,"1234E5", 6},
    {1234.657e-8,  "1234.657e-8", 11},
    {0,  "1e-800", 6},
    {0,  "-1e-800", 7},
    {1234567e-8,  "   1234567e-8 foo", 13},
    {0,     " foo 32 bar", 0},
};

int __cdecl main(int argc, char **argv)
{
    WCHAR *wideStr;
    WCHAR *endptr;
    double result;  
    int i;
  
    if (PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        wideStr = convert(testCases[i].string);
        result = wcstod(wideStr, &endptr);
      
        if (testCases[i].CorrectResult != result)
        {
            free(wideStr);
            Fail("ERROR: wcstod misinterpreted \"%s\" as %g instead of "
                   "%g.\n", 
                   testCases[i].string, 
                   result, 
                   testCases[i].CorrectResult);
        }
      
        if (endptr != wideStr + testCases[i].stopChar)
        {
            free(wideStr);
            Fail("ERROR: wcstod stopped scanning \"%s\" at %p, "
                "instead of %p!\n", testCases[i].string, endptr,
                wideStr + testCases[i].stopChar);
        }

        free(wideStr);
    }      
  
  
    PAL_Terminate();
    return PASS;
}
