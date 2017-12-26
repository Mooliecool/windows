/*============================================================================
**
** Source:  test1.c
**
** Purpose: Calls the time function and verifies that the time returned
**          is at least a positive value.
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
**==========================================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    time_t t = 0;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    time(&t);
    /*I was going to test that the time returned didn't exceed some
      reasonable value, but decided not to, for fear of creating my own
      little Y2K-style disaster.*/

    if (t <= 0)
    {
        Fail("time() function doesn't return a time.\n");
    }
    t = 0;
    t = time(NULL);  
    if (t <= 0)
    {
        Fail("time() function doesn't return a time.\n");
    }
    PAL_Terminate();
    return PASS;
}







