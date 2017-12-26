/*============================================================================
**
** Source:  test.c
**
** Purpose: This test is an example of the basic structure of a PAL test 
**          suite test case.
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
    /* Initialize the PAL.
     */
    if(0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    Trace("\nTest #1...\n");

#ifdef WIN32
    Trace("\nWe are testing under Win32 environment.\n");
#else
    Trace("\nWe are testing under Non-Win32 environment.\n");
#endif

    Trace("\nThis test has passed.\n");

    /* Shutdown the PAL.
     */
    PAL_Terminate();

    return PASS;
}
