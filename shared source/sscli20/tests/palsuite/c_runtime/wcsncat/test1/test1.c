/*============================================================================
**
** Source:  test1.c
**
** Purpose:
** Tests that wcsncat correctly appends wide strings, making sure it handles
** count argument correctly (appending no more than count characters, always
** placing a null, and padding the string if necessary).
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
    WCHAR dest[80];
    WCHAR test[] = {'f','o','o',' ','b','a','r','b','a','z',0};
    WCHAR str1[] = {'f','o','o',' ',0};
    WCHAR str2[] = {'b','a','r',' ',0};
    WCHAR str3[] = {'b','a','z',0};
    WCHAR *ptr;
    int i;

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    dest[0] = 0;
    for (i=1; i<80; i++)
    {
        dest[i] = (WCHAR)'x';
    }

    ptr = wcsncat(dest, str1, wcslen(str1));
    if (ptr != dest)
    {
        Fail("ERROR: Expected wcsncat to return ptr to %p, got %p", dest, ptr);
    }

    ptr = wcsncat(dest, str2, 3);
    if (ptr != dest)
    {
        Fail("ERROR: Expected wcsncat to return ptr to %p, got %p", dest, ptr);
    }
    if (dest[7] != 0)
    {
        Fail("ERROR: wcsncat did not place a terminating NULL!");
    }

    ptr = wcsncat(dest, str3, 20);
    if (ptr != dest)
    {
        Fail("ERROR: Expected wcsncat to return ptr to %p, got %p", dest, ptr);
    }
    if (wcscmp(dest, test) != 0)
    {
        Fail("ERROR: Expected wcsncat to give \"%S\", got \"%S\"\n", 
            test, dest);
    }
    if (dest[wcslen(test)+1] != (WCHAR)'x')
    {
        Fail("wcsncat went out of bounds!\n");
    }

    PAL_Terminate();

    return PASS;
}
