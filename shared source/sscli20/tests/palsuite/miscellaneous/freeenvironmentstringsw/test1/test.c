/*============================================================
**
** Source: test.c
**
** Purpose: Test for FreeEnvironmentStringsW() function
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

    LPWSTR CapturedEnvironment = NULL;
    BOOL TheResult = 0;
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
 
    CapturedEnvironment = GetEnvironmentStrings();

    /* If it's pointing to NULL, it failed. This checks the dependency */
    if(CapturedEnvironment == NULL) {
        Fail("The function GetEnvironmentStrings() failed, and the "
             "FreeEnvironmentStrings() tests is dependant on it.\n");    
    }

    /* This should return 1, if it succeeds, otherwise, test fails */
    TheResult = FreeEnvironmentStrings(CapturedEnvironment);
    if(TheResult != 1) {
        Fail("The function returned %d which indicates failure to Free the "
             "Environment Strings.\n",TheResult);
    }
  
    
    PAL_Terminate();
    return PASS;
}



