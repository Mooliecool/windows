/*=====================================================================
**
** Source:  test2.c
**
** Purpose:  Create an environment variable with _putenv and then use getenv 
** to check it.  This test resets an environment variable.
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

const char *_putenvString0 = "AnUnusualVariable=AnUnusualValue";
const char *_putenvString1 = "AnUnusualVariable=";
const char *variable = "AnUnusualVariable";
const char *value = "AnUnusualValue";

int __cdecl main(int argc, char **argv)
{
   
    char *variableValue;

    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    if(_putenv(_putenvString0) == -1)
    {
        Fail("ERROR: _putenv failed to set an environment "
             "variable with a valid format.\n  Call was"
             "_putenv(%s)\n", _putenvString0);
    }

    variableValue = getenv(variable);
    
    if (variableValue == NULL)
    { 
        Fail("ERROR: getenv(%s) call returned NULL\nThe call "
             "should have returned '%s'\n", variable, value);
    }  
    else 
    {
        if ( strcmp(variableValue, value) != 0 ) 
        {
            Fail("ERROR: _putenv(%s)\nshould have set the variable "
                 "'%s'\n to '%s'.\nA subsequent call to getenv(%s)\n"
                 "returned '%s' instead.\n", _putenvString0,
                 variable, value, variable, variableValue);
        }
        else 
        {
            if(_putenv(_putenvString1) == -1)
            {
                Fail("ERROR: _putenv failed to set an environment "
                     "variable with a valid format.\n  Call was"
                     "_putenv(%s)\n", _putenvString1);
            }

            variableValue = getenv(variable);

            if (variableValue != NULL)
            { 
                Fail("ERROR: getenv(%s) call did not return NULL.\nThe call "
                     "returned '%s'.\n", variable, value);
            }
        }
    }
    
    PAL_Terminate();
    return PASS;
}
