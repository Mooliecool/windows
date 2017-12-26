/*=============================================================
**
** Source:  test1.c
**
** Purpose: Tests _i64tow with normal values and different radices, negative 
**          values, as well as the highest and lowest values.
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
**============================================================*/

#include <palsuite.h>

typedef struct
{
    INT64 value;
    int radix;
    char *result;
} testCase;


int __cdecl main(int argc, char *argv[])
{
    WCHAR buffer[256];
    WCHAR *testStr;
    WCHAR *ret;
    int i;
    testCase testCases[] = 
    {
        {42, 10, "42"},
        {42, 2, "101010"},
        {29, 32, "t"},
        {-1, 10, "-1"},
        {-1, 8, "1777777777777777777777"},
        {-1, 32, "fvvvvvvvvvvvv"},
        {I64(0x7FFFFFFFFFFFFFFF), 10, "9223372036854775807"},
        {I64(0x8000000000000000), 10, "-9223372036854775808"},
        {0,2,"0"},
        {0,16,"0"},
        {3,16,"3"},
        {15,16,"f"},
        {16,16,"10"},
        
    };


    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    for (i=0; i<sizeof(testCases) / sizeof(testCase); i++)
    {
        ret = _i64tow(testCases[i].value, buffer, testCases[i].radix);
        if (ret != buffer)
        {
            Fail("_i64tow did not return a pointer to the string.\n"
                "Expected %p, got %p\n", buffer, ret);
        }

        testStr = convert(testCases[i].result);
        if (wcscmp(testStr, buffer) != 0)
        {
            Fail("_i64tow did not give the correct string.\n"
                "Expected %S, got %S\n", testStr, buffer);
        }
        free(testStr);
    }
    
   
    PAL_Terminate();
    return PASS;
}
