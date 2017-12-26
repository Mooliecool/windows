/*============================================================
**
** Source : test.c
**
** Purpose: InterlockedExchangeAdd() function
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

/*
** The InterlockedExchangeAdd function performs an atomic addition of Value 
** to the value pointed to by Addend. 
** The result is stored in the address specified by Addend. 
** The initial value of the variable pointed to by Addend is returned as the function value.
*/
#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) 
{
    
    long TheReturn;
  
    long *ptrValue = NULL;
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }



    PAL_Terminate();
    return PASS; 
} 





