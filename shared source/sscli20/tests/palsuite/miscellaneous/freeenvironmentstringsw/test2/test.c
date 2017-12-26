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

    WCHAR CapturedEnvironment[] = {'T','E','S','T','\0'};
    BOOL TheResult = 0;
    LPWSTR lpCapturedEnvironment = NULL;
    
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    lpCapturedEnvironment = (LPWSTR)malloc( sizeof(CapturedEnvironment) / 
                                            sizeof(CapturedEnvironment[0]) );

    if ( lpCapturedEnvironment )
    {
        memcpy( lpCapturedEnvironment, CapturedEnvironment, 
                sizeof(CapturedEnvironment) / sizeof(CapturedEnvironment[0]) );
    }
    else
    {
        PAL_Terminate();
        Fail( "malloc()  failed to allocate memory.\n" );
    }
    /* Even if this is not a valid Environment block, the function will 
       still return success 
    */
  
    TheResult = FreeEnvironmentStrings( lpCapturedEnvironment );
    if(TheResult == 0) 
    {
        Fail("The function should still return a success value even if it is "
             "passed a LPWSTR which is not an environment block properly "
             "aquired from GetEnvironmentStrings\n");
    }
  
    /* Even passing this function NULL, should still return a success value */
    TheResult = FreeEnvironmentStrings(NULL);
    if(TheResult == 0) 
    {
        Fail("The function should still return a success value even if pass "
             "NULL.\n");    
    }
 
    
    PAL_Terminate();
    return PASS;
}



