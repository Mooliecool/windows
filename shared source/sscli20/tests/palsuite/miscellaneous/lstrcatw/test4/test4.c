/*============================================================
**
** Source:    test4.c
**
** Purpose:   Testing the behaviour of lstrcatw when string2 contains 
**            special characters, this test case depends on:
**            memcmp
**            wcslen
**            lstrcpyn
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
**=========================================================*/

#define UNICODE

#include <palsuite.h>

struct testCase
{
    WCHAR SecondString[5];
    WCHAR CorrectString[10];
};

int __cdecl main(int argc, char *argv[])
{

    WCHAR FirstString[10] = {'T','E','S','T','\0'};
    WCHAR TestString[10] = {'T','E','S','T','\0'};
    int i = 0;

    /*
     * this structure includes several strings to be tested with
     * lstrcatW function and the expected results
     */

    struct testCase testCases[]=
    {
        {{'\t','T','A','B','\0'},
        {'T','E','S','T','\t','T','A','B','\0'}},
        {{'2','T','\?','B','\0'},
        {'T','E','S','T','2','T','\?','B','\0'}},
        {{'\v','T','E','\v','\0'},
        {'T','E','S','T','\v','T','E','\v','\0'}},
        {{'T','\a','E','\a','\0'},
        {'T','E','S','T','T','\a','E','\a','\0'}},
        {{'0','\f','Z','\f','\0'},
        {'T','E','S','T','0','\f','Z','\f','\0'}},
        {{'\r','H','I','\r','\0'},
        {'T','E','S','T','\r','H','I','\r','\0'}},
        {{'H','I','\"','\"','\0'},
        {'T','E','S','T','H','I','\"','\"','\0'}},
        {{'H','\b','I','\b','\0'},
        {'T','E','S','T','H','\b','I','\b','\0'}},
        {{'H','\n','I','\n','\0'},
        {'T','E','S','T','H','\n','I','\n','\0'}}
    };

  


    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }


    /* Loop through the struct and validate the resulted string */
    for( i = 0; i < sizeof(testCases)/sizeof(struct testCase); i++)
    {

        lstrcat(FirstString, testCases[i].SecondString);
        
        if(memcmp(FirstString,testCases[i].CorrectString,
            wcslen(FirstString)*sizeof(WCHAR)))
        {
            
            Fail("ERROR: the function failed with a special character.\n");
        }

        /* reinitialize the first string */        
        lstrcpyn(FirstString,TestString,10); 

    }


    

    PAL_Terminate();
    return PASS;
}



