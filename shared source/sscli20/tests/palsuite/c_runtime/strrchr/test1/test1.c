/*============================================================================
**
** Source:  test1.c
**
** Purpose: \
** Search a string for a given character.  Search for a character contained
** in the string, and ensure the pointer returned points to it.  Then search
** for the null character, and ensure the pointer points to that.  Finally 
** search for a character which is not in the string and ensure that it 
** returns NULL.
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
    char *str = "foo bar baz";
    char *ptr;

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    ptr = strrchr(str, 'b');
    if (ptr != str + 8)
    {
        Fail("Expected strrchr() to return %p, got %p!\n", str + 8, ptr);
    }

    ptr = strrchr(str, 0);
    if (ptr != str + 11)
    {
        Fail("Expected strrchr() to return %p, got %p!\n", str + 11, ptr);
    }

    ptr = strrchr(str, 'x');
    if (ptr != NULL)
    {
        Fail("Expected strrchr() to return NULL, got %p!\n", ptr);
    }

    PAL_Terminate();

    return PASS;
}
