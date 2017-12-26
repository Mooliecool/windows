/*============================================================================
**
** Source:  test1.c
**
** Purpose: Checks every character against the known range of digits.
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


    for (i=0; i<256; i++)
    {
        if (isdigit(i))
        {
            if (i < '0' || i > '9')
            {
                Fail("ERROR: isdigit returned true for '%c' (%d)!\n", i, i);
            }
        }
        else
        {
            if (i >= '0' && i <= '9')
            {
                Fail("ERROR: isdigit returned false for '%c' (%d)!\n", i, i);
            }
        }
    }

    PAL_Terminate();
    return PASS;
}
