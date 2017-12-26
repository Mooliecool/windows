/*============================================================
**
** Source: test.c
**
** Purpose: Test for CloseHandle function, try to close an unopened HANDLE
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

int __cdecl main(int argc, char *argv[])
{

    HANDLE SomeHandle = NULL;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
 
    /* If the handle is already closed and you can close it again, 
     * something is wrong. 
     */
  
    if(CloseHandle(SomeHandle) != 0) 
    {
        Fail("ERROR: Called CloseHandle on an already closed Handle "
             "and it still returned as a success.\n");
    }
  
    
    PAL_Terminate();
    return PASS;
}



