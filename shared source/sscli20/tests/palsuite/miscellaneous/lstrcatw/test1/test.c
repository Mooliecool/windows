/*============================================================
**
** Source: test.c
**
** Purpose: Test for lstrcatW() function
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
    WCHAR CorrectString[] = {'T','E','S','T','P','A','L','!','\0'};
    WCHAR* ReturnedPointer = NULL;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    ReturnedPointer = lstrcat(FirstString,SecondString);

    /* Check to see if the pointer returned points to the concat string */
    if(ReturnedPointer != &FirstString[0])
    {
        Fail("ERROR:  The function was supposed to return a pointer to "
             "the concatentated string, but it did not.\n");    
    }
  
    /* Check to make sure the Concat string is the same as the predetermined 
       'CorrectString' */
    if(memcmp(FirstString,CorrectString,
              wcslen(FirstString)*sizeof(WCHAR)) != 0) 
    {
        Fail("ERROR: The concatenated string should be %s but %s was the "
             "result.\n",
             convertC(CorrectString),
             convertC(FirstString));
    }
    
    PAL_Terminate();
    return PASS;
}



