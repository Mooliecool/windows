/*============================================================
**
** Source: test.c
**
** Purpose: Test for InterlockedCompareExchangePointer() function
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
   one thread can access it.  But, I really don't have a great test to 
   make sure it's thread safe.  Any ideas?
*/

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) 
{

    long StartValue = 5;
    long NewValue   = 10;
    PVOID ReturnValue = NULL;
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    ReturnValue = InterlockedCompareExchangePointer((PVOID)&StartValue,
                                                    (PVOID)NewValue,
                                                    (PVOID)StartValue);

    /* StartValue and NewValue should be equal now */
    if(StartValue != NewValue) 
    {
        Fail("ERROR: These values should be equal after the exchange.  "
             "They should both be %d, however the value that should have "
             "been exchanged is %d.\n",NewValue,StartValue);    
    }
  
    /* Returnvalue should have been set to what 'StartValue' was 
       (5 in this case) 
    */
  
    if((int)ReturnValue != 5) 
    {
        Fail("ERROR: The return value should be the value of the "
             "variable before the exchange took place, which was 5.  "
             "But, the return value was %d.\n",ReturnValue);
    }

    /* This is a mismatch, so no exchange should happen */
    InterlockedCompareExchangePointer((PVOID)&StartValue,
                                       ReturnValue,
                                       ReturnValue);
    if(StartValue != NewValue) 
    {
        Fail("ERROR:  The compare should have failed and no exchange should "
             "have been made, but it seems the exchange still happened.\n");
    }
  
    
    PAL_Terminate();
    return PASS; 
} 






