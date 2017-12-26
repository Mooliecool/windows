/*============================================================
** Source : test4.c
**
** Purpose: Test for SetEnvironmentVariableA() function
**          Create environment variables that differ only
**          in case and verify that they return the appropriate
**          value in the WIN32 Environment
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
===========================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) 
{

#if WIN32

    /* Define some buffers needed for the function */
    char * pResultBuffer = NULL;

    char FirstEnvironmentVariable[] = {"PALTEST"};
    char FirstEnvironmentValue[] = {"FIRST"};
    char ModifiedEnvVar[] = {"paltest"};

    DWORD size = 0;
    BOOL bRc = TRUE;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
  
    /* Set the first environment variable */
    bRc = SetEnvironmentVariableA(FirstEnvironmentVariable,
                            FirstEnvironmentValue);

    if(!bRc)
    {
        Fail("ERROR: SetEnvironmentVariable failed to set a "
            "proper environment variable with error %u.\n",
            GetLastError());
    }

    /* Normal case, PATH should fit into this buffer */
    size = GetEnvironmentVariableA(ModifiedEnvVar,        
                                  pResultBuffer,    
                                  0);
    
    /* To account for the null character at the end of the string */
    size = size + 1;
    
    pResultBuffer = malloc(sizeof(char)*size);
    if ( pResultBuffer == NULL )
    {
        Fail("ERROR: Failed to allocate memory for pResultBuffer pointer.\n");
    }

    /* Try to retrieve the value of the first environment variable */
    GetEnvironmentVariableA(ModifiedEnvVar,
                           pResultBuffer,
                           size);

    if ( pResultBuffer == NULL )
    {
        free(pResultBuffer);
        Fail("ERROR: GetEnvironmentVariable failed to return a value "
            "from a proper environment variable with error %u.\n",
            GetLastError());
    }

    /* Compare the strings to see that the correct variable was returned */
    if(strcmp(pResultBuffer,FirstEnvironmentValue) != 0) 
    {
        Trace("ERROR: The value in the buffer should have been '%s' but "
             "was really '%s'.\n",FirstEnvironmentValue, pResultBuffer);
        free(pResultBuffer);
        Fail("");
    }

    free(pResultBuffer);
  
    PAL_Terminate();
    return PASS;


#else

    return PASS;
#endif
}
