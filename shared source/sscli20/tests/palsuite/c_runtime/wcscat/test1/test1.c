/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
** Test to that wcscat correctly concatanates wide strings, including placing 
** null pointers.
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
 * Notes: uses memcmp and the (pal) sprintf
 */

int __cdecl main(int argc, char *argv[])
{
    WCHAR dest[80];
    WCHAR test[] = {'f','o','o',' ','b','a','r',' ','b','a','z',0};
    WCHAR str1[] = {'f','o','o',' ',0};
    WCHAR str2[] = {'b','a','r',' ',0};
    WCHAR str3[] = {'b','a','z',0};
    WCHAR *ptr;
    char buffer[256];

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    dest[0] = 0;

    ptr = wcscat(dest, str1);
    if (ptr != dest)
    {
        Fail("ERROR: Expected wcscat to return ptr to %p, got %p", dest, ptr);
    }

    ptr = wcscat(dest, str2);
    if (ptr != dest)
    {
        Fail("ERROR: Expected wcscat to return ptr to %p, got %p", dest, ptr);
    }

    ptr = wcscat(dest, str3);
    if (ptr != dest)
    {
        Fail("ERROR: Expected wcscat to return ptr to %p, got %p", dest, ptr);
    }

    if (memcmp(dest, test, sizeof(test)) != 0)
    {
        sprintf(buffer, "%S", dest);
        Fail("ERROR: Expected wcscat to give \"%s\", got \"%s\"\n", 
            "foo bar baz", buffer);
    }

    PAL_Terminate();
    return PASS;
}

