/*============================================================
**
** Source:    test3.c
**
** Purpose:   Testing lstrcatw with two NULL strings passed on
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

int __cdecl main(int argc, char *argv[])
{
  


    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }


    /* testing the behaviour of lstrcatW with two NULL strings */
    if( lstrcat(NULL,NULL) != NULL)
    {
        
        Fail("lstrcat:ERROR: the function should returned NULL\n");

    }

    PAL_Terminate();
    return PASS;
}



