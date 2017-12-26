/*========================================================================
**
** Source:  test1.c
**
** Purpose:
** Check the length of a number of arrays.  The first is a normal string
** which should return its length.  The second has two bytes and a null 
** character, which only returns a size of 2, and the last is just a NULL
** array which should return 0.
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
**========================================================================*/

#include <palsuite.h>

/*
 * Note: it seems like these functions would only be useful if they
 *  didn't assume a character was equivalent to a single byte. Be that
 *  as it may, I haven't seen a way to get it to behave otherwise
 *  (eg locale)
 */

int __cdecl main(int argc, char *argv[])
{
    unsigned char *str1 = (unsigned char*) "foo";
    unsigned char str2[] = {0xC0, 0x80, 0};  /* the char U+0080 */
    unsigned char str3[] = {0};
    int ret=0;

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    ret = _mbslen(str1);
    if (ret != 3)
    {
        Fail ("ERROR: _mbslen(\"%s\") returned %d. Expected %d\n",
                str1, ret, 3);
    }

    ret = _mbslen(str2);
    if (ret != 2)
    {
        Fail ("ERROR: _mbslen(\"%s\") returned %d. Expected %d\n",
                str2, ret, 2);
    }

    ret = _mbslen(str3);
    if (ret != 0)
    {
        Fail ("ERROR: _mbslen(\"%s\") returned %d. Expected %d\n",
                str3, ret, 0);
    }

    PAL_Terminate();
    return PASS;
}

