/*============================================================
**
** Source: test.c
**
** Purpose: InterlockedExchange() function
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

/* This test is FINISHED.  Note:  The biggest feature of this function is that
   it locks the value before it increments it -- in order to make it so only 
   one thread can access it.  But, I really don't have a great test to make 
   sure it's thread safe.  Any ideas?
*/

#include <palsuite.h>

#define START_VALUE 0

int __cdecl main(int argc, char *argv[]) {

    int TheValue = START_VALUE;
    int NewValue = 5;
    int TheReturn;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    TheReturn = InterlockedExchange(&TheValue,NewValue);
  
    /* Compare the exchanged value with the value we exchanged it with.  Should
       be the same.
    */  
    if(TheValue != NewValue) 
    {
        Fail("ERROR: The value which was exchanged should now be %d, but "
             "instead it is %d.",NewValue,TheValue);    
    }
  
    /* Check to make sure it returns the origional number which 'TheValue' was 
       set to. 
    */
  
    if(TheReturn != START_VALUE) 
    {
        Fail("ERROR: The value returned should be the value before the "
             "exchange happened, which was %d, but %d was returned.",
             START_VALUE,TheReturn);
    }

    
    PAL_Terminate();
    return PASS; 
} 





