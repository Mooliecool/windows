/*=====================================================================
**
** Source:  test4.c
**
** Purpose: Create an environment variable and try to retrieve
**          it using the same name but with different case.  This
**          is to show that the Win32 representation of _putenv
**          is case insensitive.
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
**===================================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
#if WIN32

    const char* FirstVariable = "PalTestingEnvironmentVariable=The value";
    const char* ModifiedName = "PALTESTINGEnvironmentVariable";
    const char* FirstVarValue = "The value";
    char* result;

   
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    
    /* Use _putenv to set an environment variable.  This ensures that the
       variable we're testing on is always present.
    */

    if(_putenv(FirstVariable) != 0)
    {
        Fail("ERROR: _putenv failed to set an environment variable that "
             "getenv will be using for testing.\n");
    }


    /* Call getenv -- ensure it doesn't return NULL and the string it returns
       is the value we set above. Also make sure that each environment variable,
       differing only by case, doesn't affect the return value.
    */
    
    result = getenv(ModifiedName);
    if(result == NULL)
    {
        Fail("ERROR: The result of getenv on a valid Environment Variable "
             "was NULL, which indicates the environment varaible was not "
             "found.\n");
    }

    if(strcmp(result, FirstVarValue) != 0)
    {
        Fail("ERROR: The value obtained by getenv() was not equal to the "
             "correct value of the environment variable.  The correct "
             "value is '%s' and the function returned '%s'.\n",
             FirstVarValue,
             result);
    }
    

    PAL_Terminate();
    return PASS;

#else
    return PASS;

#endif
}
