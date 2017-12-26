/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
** Check the length of a string and the length of a 0 character string to 
** see that this function returns the correct values for each.
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
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    if (strlen("foo") != 3)
        Fail("ERROR: strlen(\"foo\") != 3\n");

    if (strlen("") != 0)
        Fail("ERROR: strlen(\"\") != 0\n");

    PAL_Terminate();
    return PASS;
}
