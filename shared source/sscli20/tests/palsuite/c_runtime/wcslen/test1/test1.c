/*============================================================================
**
** Source:  test1.c
**
** Purpose:
** Tests that wcslen correctly returns the length (in wide characters,
** not byte) of a wide string
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
    WCHAR str1[] = {'f','o','o',' ',0};
    WCHAR str2[] = {0};
    int ret;

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    ret = wcslen(str1);
    if (ret != 4)
    {
        Fail("ERROR: Expected wcslen of \"foo \" to be 4, got %d\n", ret);
    }
        
    ret = wcslen(str2);
    if (ret != 0)
    {
        Fail("ERROR: Expected wcslen of \"\" to be 0, got %d\n", ret);
    }


    PAL_Terminate();
    return PASS;
}

