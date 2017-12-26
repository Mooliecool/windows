/*============================================================================
**
** Source:  test1.c
**
** Purpose: Test #1 for the wcstoul function
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
 * Notes: wcstoul should depend on the current locale's LC_NUMERIC category, 
 * this is not currently tested.
 */


int __cdecl main(int argc, char *argv[])
{
    WCHAR teststr[] = {'1','2','3','4','5',0};
    WCHAR *end;
    unsigned long result = 27;
    unsigned long l;
        
    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    l = wcstoul(teststr, &end, 4);

    if (l != result)
    {
        Fail("ERROR: Expected wcstoul to return %d, got %d\n", result, l);
    }

    if (end != teststr + 3)
    {
        Fail("ERROR: Expected wcstoul to give an end value of %p, got %p\n",
            teststr + 3, end);
    }
    
    PAL_Terminate();
    return PASS;
}
