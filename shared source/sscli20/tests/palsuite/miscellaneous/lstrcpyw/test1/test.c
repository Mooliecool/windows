/*============================================================
**
** Source: test.c
**
** Purpose: Test for lstrcpyW() function
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

int __cdecl main(int argc, char *argv[])
{
    WCHAR FirstString[5] = {'T','E','S','T','\0'};
    WCHAR ResultBuffer[5];
    WCHAR* ResultPointer = NULL;
	
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    ResultPointer = lstrcpy(ResultBuffer,FirstString);

    /* Check the return value */
    if(ResultPointer != &ResultBuffer[0]) 
    {
        Fail("ERROR: The function did not return a pointer to the Result "
             "Buffer after being called.\n");   
    }

    /* A straight copy, the values should be equal. */	
    if(memcmp(ResultBuffer,FirstString,wcslen(ResultBuffer)*2+2) != 0) 
    {
        Fail("ERROR: The result of the copy was '%s' when it should have "
             "been '%s'.\n",convertC(ResultBuffer),convertC(FirstString));
    }
  
    /* If either param is NULL, it should return NULL. */
    if(lstrcpy(ResultBuffer,NULL) != NULL)  
    {
        Fail("ERROR: The second parameter was NULL, so the function should "
             "fail and return NULL.\n");    
    }
    if(lstrcpy(NULL,FirstString) != NULL) 
    {
        Fail("ERROR: The first parameter was NULL, so the function should "
             "fail and return NULL.\n");
    }
    
    
    PAL_Terminate();
    return PASS;
}


