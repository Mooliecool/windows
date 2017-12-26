/*============================================================================
**
** Source:  test1.c
**
** Purpose:
** Tests that wcschr correctly finds the first occurrence of a character in a
** string
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
    WCHAR str[] = {'f','o','o',' ','b','a','r',' ',0};
    WCHAR c = (WCHAR)' ';
    WCHAR c2 = (WCHAR)'$';
    WCHAR *ptr;

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    ptr = wcschr(str, c);
    if (ptr != str + 3)
    {
        Fail("ERROR: expected wcschr to return pointer to %p, got %p\n", 
            str + 3, ptr);
    }

    ptr = wcschr(str, c2);
    if (ptr != NULL)
    {
        Fail("ERROR: expected wcschr to return pointer to %p, got %p\n", 
            NULL, ptr);
    }
        
    PAL_Terminate();
    return PASS;
}

