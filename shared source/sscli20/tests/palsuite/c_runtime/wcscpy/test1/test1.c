/*============================================================================
**
** Source:  test1.c
**
** Purpose: Tests that wcscpy correctly copies a null-terminated wide string.
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
 * Notes: uses memcmp and sprintf.
 */

int __cdecl main(int argc, char *argv[])
{
    WCHAR str[] = {'f','o','o',0,'b','a','r',0};
    WCHAR dest[80];
    WCHAR result[] = {'f','o','o',0};
    WCHAR *ret;
    char buffer[256];

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    ret = wcscpy(dest, str);
    
    if (ret != dest || memcmp(dest, result, sizeof(result)) != 0)
    {
        sprintf(buffer, "%S", dest);
        Fail("Expected wcscpy to give \"%s\" with a return value of %p, got \"%s\" "
            "with a return value of %p.\n", "foo", dest, buffer, ret);
    }

    PAL_Terminate();
    return PASS;
}
