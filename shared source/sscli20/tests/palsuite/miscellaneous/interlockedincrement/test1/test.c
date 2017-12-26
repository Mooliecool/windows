/*============================================================
**
** Source: test.c
**
** Purpose: InterlockedIncrement() function
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
   sure it's thread safe. Any ideas?  Nothing I've tried has worked.
*/


#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) 
{

    int TheValue = 0;
    int TheReturn;
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    InterlockedIncrement(&TheValue);
    TheReturn = InterlockedIncrement(&TheValue);
  
    /* Incremented twice, it should be 2 now */
    if(TheValue != 2) 
    {
        Fail("ERROR: The value was incremented twice and shoud now be 2, "
             "but it is really %d",TheValue); 
    }
  
    /* Check to make sure it returns itself */
    if(TheReturn != TheValue) 
    {
        Fail("ERROR: The function should return the new value, which shoud "
             "have been %d, but it returned %d.",TheValue,TheReturn);          
    }
    
    PAL_Terminate();
    return PASS; 
} 





