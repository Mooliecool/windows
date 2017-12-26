/*=============================================================
**
** Source: InterlockedExchangePointer
**
** Purpose: Positive test the InterlockedExchangePointer API.
**          Call InterlockedExchangePointer to exchange a pair of
**          value
**          
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
    int i1 = 10;
    int i2 = 20;
    int *pOldValue = &i1;
    int *pNewValue = &i2;
    PVOID pReturnValue;
   
    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }



    /*
      Testing
      =======
    */
        
    pReturnValue = InterlockedExchangePointer((PVOID)&pOldValue,
                                     (PVOID)pNewValue);
    /*check the returned value*/
    if(*(int *)pReturnValue != i1)
    {
        Fail("\nFailed to call InterlockedExchangePointer API, "
                "return pointer does not point to the origional value\n");
    }

    /*check the exchanged value*/
    if(*pOldValue != *pNewValue)
    {
        Fail("\nFailed to call InterlockedExchangePointer API, "
                "exchanged value is not right\n");
    }



    /*
      Clean Up
    */
    PAL_Terminate();
    return PASS;
}
