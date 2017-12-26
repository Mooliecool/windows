/*============================================================================
**
** Source:  test1.c
**
** Purpose: Test #1 for the isspace function
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
    long result;
    char avalue;
};



int __cdecl main(int argc, char *argv[])
{
    int i=0;
    long result = 0;

    /*
     * A structures of the testcases to be tested with
     * isspace function
     */
    struct testCase testCases[] =
    {
           {1,'\n'},
           {1,'\t'},
           {1,'\r'},
           {1,'\v'},
           {1,'\f'},
           {1,' '},
           {0,'a'},
           {0,'A'},
           {0,'z'},
           {0,'Z'},
           {0,'r'},
           {0,'R'},
           {0,'0'},
           {0,'*'},
           {0,3}
    };

    /*
     *  Initialize the PAL
     */
    if ( 0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    /* Loop through the testcases */
    for (i=0; i<sizeof(testCases)/sizeof(struct testCase); i++)
    {
        result = isspace(testCases[i].avalue);
        if ( ((testCases[i].result == 1) && (result==0)) ||
             ((testCases[i].result ==0) && (result !=0)) )
        {
            Fail("ERROR: isspace() returned %d for %c instead of %d\n",
                 result,
                 testCases[i].avalue,
                 testCases[i].result );
        }
    }


    PAL_Terminate();

    return PASS;
}

