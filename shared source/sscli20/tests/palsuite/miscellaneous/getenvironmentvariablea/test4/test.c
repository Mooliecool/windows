/*============================================================
**
** Source : test.c
**
** Purpose: Test for GetEnvironmentVariable() function
** Set an Environment Variable, then use GetEnvironmentVariable to 
** retrieve it -- ensure that it retrieves properly.
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

/* Depends on SetEnvironmentVariableW (because we're implmenting 
   the wide version) and strcmp() 
*/

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) {

    /* Define some buffers needed for the function */
    char * pResultBuffer = NULL;
    WCHAR SomeEnvironmentVariable[] = {'P','A','L','T','E','S','T','\0'};
    WCHAR TheEnvironmentValue[] = {'T','E','S','T','\0'};
    int size = 0;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
  
    SetEnvironmentVariableW(SomeEnvironmentVariable,
                            TheEnvironmentValue);

 
    /* Normal case, PATH should fit into this buffer */
    size = GetEnvironmentVariable("PALTEST",         // Variable Name
                                  pResultBuffer,     // Buffer for Value
                                  0);                // Buffer size
  
    pResultBuffer = malloc(size);
    if ( pResultBuffer == NULL )
     {
	Fail("ERROR: Failed to allocate memory for pResultBuffer pointer. "
	       "Can't properly exec test case without this.\n");
     }
  
  
    GetEnvironmentVariable("PALTEST",
                           pResultBuffer,
                           size);
  
    if(strcmp(pResultBuffer,"TEST") != 0) 
    {
        free(pResultBuffer);    
        Fail("ERROR: The value in the buffer should have been 'TEST' but "
             "was really '%s'.\n",pResultBuffer);
            
    }
    
    free(pResultBuffer);
    
    PAL_Terminate();
    return PASS;
}



