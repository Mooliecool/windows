/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests SetThreadLocale with every valid locale to see that it passes
**          and that it actually sets the locale.
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

int __cdecl main(int argc, char *argv[])
{
    int i;

 
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* Try all possible locale's */
    for (i=0; i<0x000fffff; i++)
    {
        if (!IsValidLocale(i, LCID_INSTALLED))
        {
            continue;
        }

        if (!SetThreadLocale(i))
        {
            Fail("SetThreadLocale failed for an installed locale!\n");
        }
        
        if (GetThreadLocale() != i)
        {
            Fail("SetThreadLocale didn't actually change to LCID %#x!\n", i);
        }
    }

    PAL_Terminate();

    return PASS;
}

