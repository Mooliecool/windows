/*============================================================================
**
** Source:  test6.c
**
** Purpose: Test #6 for the wcstol function. Tests strings with octal/hex
**          number specifers
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
 * Notes: wcstol should depend on the current locale's LC_NUMERIC category, 
 * this is not currently tested.
 */


int __cdecl main(int argc, char *argv[])
{
    WCHAR test1[] = {'0','x','1','2', 0};
    WCHAR test2[] = {'0','1','2',0};
    WCHAR *end;    
    long l;
        
    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    l = wcstol(test1, &end, 16);
    if (l != 0x12)
    {
        Fail("ERROR: Expected wcstol to return %d, got %d\n", 0x12, l);
    }
    if (end != test1 + 4)
    {
        Fail("ERROR: Expected wcstol to give an end value of %p, got %p\n",
            test1 + 4, end);
    }
    
    l = wcstol(test1, &end, 10);
    if (l != 0)
    {
        Fail("ERROR: Expected wcstol to return %d, got %d\n", 0, l);
    }
    if (end != test1+1)
    {
        Fail("ERROR: Expected wcstol to give an end value of %p, got %p\n",
            test1+1, end);
    }

    l = wcstol(test2, &end, 8);
    if (l != 10)
    {
        Fail("ERROR: Expected wcstol to return %d, got %d\n", 10, l);
    }
    if (end != test2 + 3)
    {
        Fail("ERROR: Expected wcstol to give an end value of %p, got %p\n",
            test2 + 3, end);
    }

    l = wcstol(test2, &end, 10);
    if (l != 12)
    {
        Fail("ERROR: Expected wcstol to return %d, got %d\n", 12, l);
    }
    
    if (end != test2 + 3)
    {
        Fail("ERROR: Expected wcstol to give an end value of %p, got %p\n",
            test2 + 3, end);
    }

    PAL_Terminate();
    return PASS;
}
