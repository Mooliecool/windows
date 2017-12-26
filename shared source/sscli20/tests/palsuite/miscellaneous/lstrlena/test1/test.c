/*============================================================
**
** Source: test.c
**
** Purpose: Test for lstrlenA() function
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

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) {

    char * FirstString = "Pal Testing"; /* 11 characters */

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
  
    /* The string size should be 11 */
    if(lstrlen(FirstString) != 11) 
    {
        Fail("ERROR: The string size returned was %d but it should have "
             "been 11 in this test.\n",lstrlen(FirstString));    
    }

    /* A NULL pointer should return 0 length */
    if(lstrlen(NULL) != 0) 
    {
        Fail("ERROR: Checking the length of NULL pointer should return "
             "a value of 0, but %d was returned.\n",lstrlen(NULL));
    }

    
    PAL_Terminate();
    return PASS;
}



