/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the tolower function.
**          Check that the tolower function makes capital character
**          lower case. Also check that it has no effect on lower
**          case letters and special characters.
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
    int lower;
    int start;
};

int __cdecl main(int argc, char **argv)
{
  
    int result;  
    int i;

    struct testCase testCases[] = 
        {
            {'a',  'A'},  /* Basic cases */
            {'z', 'Z'},
            {'b',  'b'},  /* Lower case */
            {'?',  '?'},  /* Characters without case */
            {230,  230}
        };
  
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    /* Loop through each case.  Convert each character to lower case 
       and then compare to ensure that it is the correct value.
    */
  
    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        /*Convert to lower case*/
        result = tolower(testCases[i].start);
     
        if (testCases[i].lower != result)
        {
            Fail("ERROR: tolower lowered \"%i\" to %i instead of %i.\n",
                   testCases[i].start, result, testCases[i].lower);
        }
    
    }      
  
  
    PAL_Terminate();
    return PASS;
} 













