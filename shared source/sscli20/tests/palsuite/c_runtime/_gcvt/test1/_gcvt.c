/*=============================================================
**
** Source: _gcvt.c
**
** Purpose: Positive test the _gcvt API.
**          Call _gcvt to convert a floatable value to a string 
**          with specified sigficant digits stored
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
    double dValue = -3.1415926535;
    char buffer[1024];
    char *pChar7 = "-3.141593";

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }


    /* zero the buffer */
    memset(buffer, 0, 1024);

    
    /*

     Testing
     =======
        
     To convert a floating-point value to 
     a string to save 7 significant digits
    */
    _gcvt(dValue, 7, buffer);
    if(strcmp(pChar7, buffer))
    {
        Fail("\nFailed to call _gcvt to convert a floating-point value "
                "to a string with 7 sigficants digits stored\n");
    }
   

    /* 
       Clean up and exit
    */

    PAL_Terminate();
    return PASS;
}
