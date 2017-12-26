/*=====================================================================
**
** Source:   test1.c(towupper)
**
**
** Purpose:  Tests the PAL implementation of the towupper function.
**           Check that the towupper function makes lower case
**           character a capital. Also check that it has no effect
**           on upper case letters and special characters.
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
    WCHAR upper;
    WCHAR start;
};

int __cdecl main(int argc, char **argv)
{
  
    int result;  
    int i;

    struct testCase testCases[] = 
        {
            {'A', 'a'},  /* Basic cases */
            {'Z', 'z'},
            {'B', 'B'},  /* Upper case */
            {'%', '%'},  /* Characters without case */
            {157,  157}
        };
  
    if ((PAL_Initialize(argc, argv)) != 0)
    {
        return FAIL;
    }


    /* Loop through each case.  Convert each character to upper case 
       and then compare to ensure that it is the correct value.
    */
  
    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
        /*Convert to upper case*/
        result = towupper(testCases[i].start);
     
        if (testCases[i].upper != result)
        {
            Fail("ERROR: towupper capitalized \"%c\" to %c instead of %c.\n",
                    testCases[i].start, result, testCases[i].upper);
        }
    }      
  
    PAL_Terminate();
    return PASS;
} 
