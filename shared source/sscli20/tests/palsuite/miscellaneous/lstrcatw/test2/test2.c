/*============================================================
**
** Source: test2.c
**
** Purpose: Negative test for lstrcatW() function
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

#define UNICODE

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) {
  
    WCHAR FirstString[10] = {'T','E','S','T','\0'};
    const WCHAR SecondString[] = {'P','A','L','!','\0'};

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* If either of these is NULL, function should fail and return NULL. */
    if(lstrcat(NULL,SecondString) != NULL)
    {
        Fail("ERROR: When NULL was passed to the first parameter of the "
             "function, it should have returned "
             "NULL as a result, but did not.\n"); 
    }

    if(lstrcat(FirstString,NULL) != NULL)
    {
        Fail("ERROR: When NULL was passed to the second parameter of the " 
             "function, it should have returned "
             "NULL as a result, but did not.\n");
    }
   
    
    PAL_Terminate();
    return PASS;
}



