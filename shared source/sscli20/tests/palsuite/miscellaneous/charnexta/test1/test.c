/*============================================================
**
** Source: test.c
**
** Purpose: Test for CharNextA, ensures it returns the proper char for an 
**          entire string
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

#include <palsuite.h>

int __cdecl main(int argc,char *argv[]) 
{
    
    char * AnExampleString = "this is the string";
    char * StringPointer = AnExampleString;
    int count = 0;
    
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
      return FAIL;
    }

    /* Use CharNext to move through an entire string.  Ensure the pointer that 
       is returned points to the correct character, by comparing it with 
       'StringPointer' which isn't touched by CharNext. 
    */
  
    while(*AnExampleString != '\0') 
    {
    
        /* Fail if any characters are different.  This is comparing the 
         *  addresses of both characters, not the characters themselves. 
         */
    
        if(AnExampleString != &StringPointer[count]) 
        {
            Fail("ERROR: %#x and %#x are different.  These should be the same "
                 " address.\n",AnExampleString,&StringPointer[count]);
            
            
        }
    
        AnExampleString = CharNextA(AnExampleString);
        ++count;
    }	
    
    PAL_Terminate();
    return PASS;
}



