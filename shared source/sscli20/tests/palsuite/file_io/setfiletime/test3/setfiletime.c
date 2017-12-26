/*=====================================================================
**
** Source:  SetFileTime.c
**
** Purpose: Tests the PAL implementation of the SetFileTime function.  
** This test checks to ensure that the function fails when passed an
** invalid file HANDLE
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

    FILETIME SetCreation, SetLastWrite, SetLastAccess;
    HANDLE TheFileHandle = NULL;
    BOOL result;
    
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* Populate some FILETIME structures with values 
       These values are valid Creation, Access and Write times
       which I generated, and should work properly.
    */

    SetCreation.dwLowDateTime = 458108416;
    SetCreation.dwHighDateTime = 29436904;

    SetLastAccess.dwLowDateTime = 341368832;
    SetLastAccess.dwHighDateTime = 29436808;

    SetLastWrite.dwLowDateTime = -1995099136;
    SetLastWrite.dwHighDateTime = 29436915;

    
    /* Pass this function an invalid file HANDLE and it should
       fail.
    */
    
    result = SetFileTime(TheFileHandle,
                         &SetCreation,&SetLastAccess,&SetLastWrite);
    
    if(result != 0)
    {
        Fail("ERROR: Passed an invalid file HANDLE to SetFileTime, but it "
               "returned non-zero.  This should return zero for failure.");
    }
    

    PAL_Terminate();
    return PASS;
}
