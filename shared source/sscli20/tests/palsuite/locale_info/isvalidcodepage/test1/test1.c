/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests IsValidCodePage with a collection of valid and invalid 
**          code pages.
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


UINT InvalidCodePages[] = 
{
    0, 0x1, 0x2, 0x3, 0xfff
};

int NumInvalidPages = sizeof(InvalidCodePages) / sizeof(InvalidCodePages[0]);

int __cdecl main(int argc, char *argv[])
{
    int i;

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

   

    for (i=0; i<NumInvalidPages; i++)
    {
        if (IsValidCodePage(InvalidCodePages[i]))
        {
            Fail("IsValidCodePage() found code page %#x valid!\n", 
                InvalidCodePages[i]);
        }
    }

    PAL_Terminate();

    return PASS;
}

