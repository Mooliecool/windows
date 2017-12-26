/*=============================================================
**
** Source: isprint.c
**
** Purpose: Positive test the isprint API.
**          Call isprint to test if a character is printable 
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

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*check if the character is printable*/
    for(index = 0x20; index<=0x7E;index++)
    {
        err = isprint(index);
        if(0 == err)
        {
            Fail("\nFailed to call isprint API to check "
                "printable character from 0x20 to 0x7E!\n");
        }
    }


    PAL_Terminate();
    return PASS;
}
