/*============================================================================
**
** Source:  test1.c
**
** Purpose: General test to see if _snwprintf works correctly
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
#include "../_snwprintf.h"

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with wcslen */


int __cdecl main(int argc, char *argv[])
{
    WCHAR *checkstr;
    WCHAR buf[256] = { 0 };
    int ret;

    if (PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }

    checkstr = convert("hello world");
    _snwprintf(buf, 256, checkstr);
    if (memcmp(checkstr, buf, wcslen(checkstr)*2+2) != 0)
    {
        Fail("ERROR: Expected \"%s\", got \"%s\"\n", 
            convertC(checkstr), convertC(buf));
    }

    _snwprintf(buf, 256, convert("xxxxxxxxxxxxxxxxx"));
    ret = _snwprintf(buf, 8, checkstr);
    if (memcmp(checkstr, buf, 16) != 0)
    {
        Fail("ERROR: Expected \"%8s\", got \"%8s\"\n", 
            convertC(checkstr), convertC(buf));
    }
    if (ret >= 0)
    {
        Fail("ERROR: Expected negative return value, got %d.\n", ret);
    }
    if (buf[8] != (WCHAR) 'x')
    {
        Fail("ERROR: buffer overflow using \"%s\" with length 8.\n", 
            convertC(checkstr));
    }

    
    PAL_Terminate();
    return PASS;
}
