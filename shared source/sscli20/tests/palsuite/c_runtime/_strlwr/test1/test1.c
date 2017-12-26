/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
**      Using memcmp, check to see that after changing a string into all lowercase
**      that it is the lowercase string that was expected.
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
    char string[] = "aASdff";
    char checkstr[] = "aasdff";
    char *ret=NULL;

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    ret = _strlwr(string);

    if (memcmp(ret, checkstr, sizeof(checkstr)) != 0)
    {
        Fail ("ERROR: _strlwr returning incorrect value\n"
                "Expected %s, got %s\n", checkstr, ret);
    }
    
    PAL_Terminate();
    return PASS;
}
