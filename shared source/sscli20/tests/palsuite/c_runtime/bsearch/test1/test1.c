/*============================================================================
**
** Source:  test1.c
**
** Purpose: Calls bsearch to find a character in a sorted buffer, and
**          verifies that the correct position is returned.
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

int __cdecl charcmp(const void *pa, const void *pb)
{
    return memcmp(pa, pb, 1);
}

int __cdecl main(int argc, char **argv)
{

    const char array[] = "abcdefghij";
    char * found=NULL;

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    found = (char *)bsearch(&"d", array, sizeof(array) - 1, (sizeof(char))
                            , charcmp);
    if (found != array + 3)
    {
        Fail ("bsearch was unable to find a specified character in a "
                "sorted list.\n");
    }
    PAL_Terminate();
    return PASS;
}



