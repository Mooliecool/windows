/*=====================================================================
**
** Source:  test1.c
**
** Purpose: Tests the PAL implementation of the iswupper function
**          Check that a number of characters return the correct
**          values for whether they are upper case or not.
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

#define UNICODE
#include <palsuite.h>


struct testCase
{
    int CorrectResult;
    WCHAR character;
};

int __cdecl main(int argc, char **argv)
{
  
    int result;  
    int i;
    
    /* Note: 1 iff char =  A..Z
             0 iff char =~ A..Z
    */
                       
    struct testCase testCases[] = 
        {
            {1,  'A'},  /* Basic cases */
            {1, 'Z'},
            {0,  'b'},  /* Lower case */
            {0,  '?'},  /* Characters without case */
            {0,  230}
        };
  
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

  
    /* Loop through each case. Check to see if each is upper case or
       not.
    */
  
    for(i = 0; i < sizeof(testCases) / sizeof(struct testCase); i++)
    {
      
        result = iswupper(testCases[i].character);
     
        /* The return value is 'non-zero' for success.  This if condition
         * will still work if that non-zero isn't just 1 
         */ 
        if ( ((testCases[i].CorrectResult == 1) && (result == 0)) ||
             ( (testCases[i].CorrectResult == 0) && (result != 0) ))
        {
            Fail("ERROR: iswupper returned %i instead of %i for "
                   "character %c.\n",
                   result,
                   testCases[i].CorrectResult, 
                   testCases[i].character);
        }
    
    }      
  
  
    PAL_Terminate();
    return PASS;
} 













