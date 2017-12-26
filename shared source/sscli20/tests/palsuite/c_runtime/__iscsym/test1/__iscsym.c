/*=============================================================
**
** Source: __iscsym.c
**
** Purpose: Positive test the __iscsym API.
**          Call __iscsym to letter, digit and underscore
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
**============================================================*/
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    int err;
    int index;
    char non_letter_set[]=
        {'~','`','!','@','#','$','%','^','&','*','(',')',')',
            '-','+','=','|','\\',';',':','"','\'','<','>',
            ',','.','?','/','\0'};
    char errBuffer[200];

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }
  
    /*To check if the parameter passed in is a character*/
    for(index = 'a'; index <= 'z'; index++)
    {
        err = __iscsym(index);
        if(0 == err)
        {
            Fail("\n__iscsym failed to recognize  a "
                    "lower-case letter:%c!\n", index);
        }
    }
    
    /*To check if the parameter passed in is a character*/
    for(index = 'A'; index <= 'Z'; index++)
    {
        err = __iscsym(index);
        if(0 == err)
        {
            Fail("\n__iscsym failed to recognize an "
                    "upper-case letter: %c!\n", index);
        }
    }

    /*To check if the parameter passed in is a digit*/
    for(index = '0'; index <= '9'; index++)
    {
        err = __iscsym(index);
        if(0 == err)
        {
            Fail("\n__iscsym failed to recognize a digit %c!\n",
                        index);
        }
    }

    /*To check if the parameter passed in is a underscore*/
    err = __iscsym('_');
    if(0 == err)
    {
        Fail("\n__iscsym failed to recognize an underscore!\n");
    }

    memset(errBuffer, 0, 200);

    for(index = 0; non_letter_set[index]; index++)
    {
        err = __iscsym(non_letter_set[index]);
        if(0 != err)
        {
            strncat(errBuffer, &non_letter_set[index], 1);
            strcat(errBuffer, ", ");
        }
    }

    if(strlen(errBuffer) > 0)
    {
        Fail("\n__iscsym failed to identify the characters '%s' "
             "as not letters, digits "
             "or underscores\n", errBuffer);
    }
    PAL_Terminate();
    return PASS;
}
