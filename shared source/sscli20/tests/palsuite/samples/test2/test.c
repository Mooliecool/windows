/*=====================================================================
**
** Source:   test.c
**
** Purpose:  A sample to show how to structure a test case.
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
**===================================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    int exampleInt = 9;
    
    /* Initialize the PAL.
     */
    if(0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    Trace("\nTest #2...\n");

#ifdef WIN32
    Trace("\nWe are testing under Win32 environment.\n");
#else
    Trace("\nWe are testing under Non-Win32 environment.\n");
#endif

    if (exampleInt == 9)
    {
        Fail("This is an example to how to code a failure. "
             "This failure was caused by exampleInt equalling %d\n",
             exampleInt);
    }

    /* Shutdown the PAL.
     */
    PAL_Terminate();

    return PASS;
}
