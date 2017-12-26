/*============================================================
**
** Source : test.c
**
** Purpose: InterlockedDecrement64() function
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

/* This test is FINISHED.  Note:  The biggest feature of this function is that
   it locks the value before it increments it -- in order to make it so only 
   one thread can access it.  But, I really don't have a great test to make 
   sure it's thread safe. Any ideas?
*/

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) 
{
    LONGLONG TheValue = 0;
    LONGLONG TheReturn;
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

/*
**  Run only on 64 bit platforms
*/
    PAL_Terminate();
    return PASS; 
} 





