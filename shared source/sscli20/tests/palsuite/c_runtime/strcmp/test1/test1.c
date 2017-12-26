/*============================================================================
**
** Source:  test1.c
**
** Purpose:
** Compare a number of different strings against each other, ensure that the
** three return values are given at the appropriate times.
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

typedef struct
{
    int result;
    char string1[50];
    char string2[50];
} testCase;

testCase testCases[]=
{
     {0,"Hello","Hello"},
     {1,"hello","Hello"},
     {-1,"Hello","hello"},
     {0,"0Test","0Test"},
     {0,"***???","***???"},
     {0,"Testing the string for string comparison","Testing the string for "
        "string comparison"},
     {-1,"Testing the string for string comparison","Testing the string for "
         "string comparsioa"},
     {1,"Testing the string for string comparison","Testing the string for "
        "comparison"},
     {-1,"aaaabbbbb","aabcdefeccg"}
};

int __cdecl main(int argc, char *argv[])
{
    int i = 0;
    int result = 0;
    
    /*
     *  Initialize the PAL
     */
    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* Loop through structure and test each case */
    for (i=0; i < sizeof(testCases)/sizeof(testCase); i++)
    {
        result = strcmp(testCases[i].string1,testCases[i].string2);

        /* Compare returned value */
        if( ((result == 0) && (testCases[i].result !=0)) ||
            ((result <0) && (testCases[i].result !=-1)) ||
            ((result >0) && (testCases[i].result !=1)) )
        {
           Fail("ERROR:  strcmp returned %d instead of %d\n",
                result, testCases[i].result);
        }

    }

    PAL_Terminate();

    return PASS;
}
