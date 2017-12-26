/*============================================================
**
** Source:  
**
** Source : test1.c
**
** Purpose: Test for GetTickCount() function
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

    DWORD FirstCount = 0;
    DWORD SecondCount = 0;
    int i = 0;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* Grab a FirstCount, then loop for a bit to make the clock increase */
    FirstCount = GetTickCount();
  
    /* Make sure some time passes */
	Sleep(60); //Since the get tick count is accurate within 55 milliseconds.

    /* Get a second count */
    SecondCount = GetTickCount();

    /* Make sure the second one is bigger than the first. 
       This isn't the best test, but it at least shows that it's returning a
       DWORD which is increasing.
    */
  
    if(FirstCount >= SecondCount) 
    {
        Fail("ERROR: The first time (%d) was greater/equal than the second time "
             " (%d).  The tick count should have increased.\n",
             FirstCount,SecondCount);
    }
    
    PAL_Terminate();
    return PASS;
}



