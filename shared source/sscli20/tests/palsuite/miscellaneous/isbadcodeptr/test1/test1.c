/*=============================================================================
**
** Source: test1.c
**
** Purpose: Test to ensure that IsBadCodePtr return 0 when
**          it can read memory or non zero when it can't.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**				 Fail
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
**===========================================================================*/

#include <palsuite.h>

/**
 * main
 * 
 * executable entry point
 */
INT __cdecl main(INT argc, CHAR **argv)
{
    BOOL ResultValue = 0;
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }     

    /* This should be readable, and return 0 */
    ResultValue = IsBadCodePtr((FARPROC)main);
    if(ResultValue != 0) 
    {
        Fail("ERROR: IsBadCodePtr returned %d instead of 0, when pointing "
             "at readable memory.\n",ResultValue);    
    }

    /* 0x00 is usually unreadable memory so the function should 
       return non zero */
    ResultValue = IsBadCodePtr((FARPROC)0x00);

    if(ResultValue == 0)
    {
        Fail("ERROR: IsBadCodePtr returned %d instead of non zero  "
             "when checking on unreadable memory.\n",ResultValue);
    }

    PAL_Terminate();
    return PASS;
}
