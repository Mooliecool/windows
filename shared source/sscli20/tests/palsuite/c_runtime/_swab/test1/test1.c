/*============================================================================
**
** Source:  test1.c
**
** Purpose: Calls _swab on a buffer, and checks that it has correctly
**          swapped adjacent bytes
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
    char before[] = "abcdefghijklmn";
    char after[] =  "--------------";
    const char check[] = "badcfehgjilknm";

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    _swab(before, after, sizeof(before));
    if (memcmp(after, check, sizeof(after)) != 0)
    {
        Fail ("_swab did not correctly swap adjacent bytes in a buffer.\n");
    }

    PAL_Terminate();
    return PASS;

}





