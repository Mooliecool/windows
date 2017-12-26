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

/* Depends on SetEnvironmentVariableW (because we're implmenting the wide 
   version) and strcmp() */

#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) {

    /* Define some buffers needed for the function */
    WCHAR * pResultBuffer = NULL;
    WCHAR SomeEnvironmentVariable[] = {'P','A','L','T','E','S','T','\0'};
    WCHAR TheEnvironmentValue[] = {'T','E','S','T','\0'};
    int size;
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
  
    SetEnvironmentVariable(SomeEnvironmentVariable,
                           TheEnvironmentValue);
  
  
    /* Normal case, PATH should fit into this buffer */
    size = GetEnvironmentVariable(convert("PALTEST"),   // Variable Name
                                  pResultBuffer,        // Buffer for Value
                                  0);                   // Buffer size
  
    pResultBuffer = malloc(size*sizeof(WCHAR));
  
    GetEnvironmentVariable(convert("PALTEST"),
                           pResultBuffer,
                           size);
  
    if(wcsncmp(pResultBuffer,convert("TEST"),wcslen(pResultBuffer) * 2) != 0) 
    {
        Fail("ERROR: The value in the buffer should have been 'TEST' but was "
             "really '%s'.",convertC(pResultBuffer));
    }
    
    free(pResultBuffer);  
    
    PAL_Terminate();
    return PASS;
}

