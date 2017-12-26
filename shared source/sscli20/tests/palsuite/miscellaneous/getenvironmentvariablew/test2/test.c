/*============================================================
**
** Source : test.c
**
** Purpose: Test for GetEnvironmentVariable() function
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

#define SMALL_BUFFER_SIZE 1

int __cdecl main(int argc, char *argv[]) 
{
    
    WCHAR pSmallBuffer[SMALL_BUFFER_SIZE];
    
    /* A place to stash the returned values */
    int  ReturnValueForSmallBuffer;
    
    /*
     * Initialize the PAL and return FAILURE if this fails
     */
    
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    
    /* PATH won't fit in this buffer, it should return how many characters 
       it needs 
    */
    
    ReturnValueForSmallBuffer = GetEnvironmentVariable(convert("PATH"),       
                                                       pSmallBuffer,
                                                       SMALL_BUFFER_SIZE); 
    
    if(ReturnValueForSmallBuffer <= 0) 
    {
        Fail("The return value was %d when it should have been greater "
             "than 0. This should return the  number of characters needed "
             "to contained the contents of PATH in a buffer.\n",
             ReturnValueForSmallBuffer);      
    }
    
    PAL_Terminate();
    return PASS;
}

