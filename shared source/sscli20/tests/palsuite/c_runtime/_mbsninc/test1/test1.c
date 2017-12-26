/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
** Ensure that this functions increases a string pointer by n characters.
** Use a for loop, and increase the pointer by a different number of characters
** on each iteration, ensure that it is indeed pointing to the correct location
** each time.  The second test checks to see if you attempt to increase the 
** pointer past the end of the string, the pointer should just point at the 
** last character.
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

/*
 * Note: it seems like these functions would only be useful if they
 *   didn't assume a character was equivalent to a single byte. Be that
 *   as it may, I haven't seen a way to get it to behave otherwise.
 */

int __cdecl main(int argc, char *argv[])
{
    unsigned char str[] = {0xC0, 0x80, 0xC0, 0x80, 0};
    int i=0;
    unsigned char *ret=NULL;

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    for (i=0; i<5; i++)
    {
        ret = _mbsninc(str, i);
        if (ret != str + i)
        {
            Fail ("ERROR: _mbsninc returned %p. Expected %p\n", ret, str+i);
        }
    }

    /* 
     * trying to advance past the end of the string should just 
     * return the end. 
     */
    ret = _mbsninc(str, 5);
    if (ret != str + 4)
    {
        Fail ("ERROR: _mbsninc returned %p. Expected %p\n", ret, str+4);
    }


    PAL_Terminate();
    return PASS;
}

