/*=============================================================
**
** Source: test1.c
**
** Purpose: Intended to be the child process of a debugger.  Calls 
**          OutputDebugStringW once with a normal string, once with an empty
**          string
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
**============================================================*/

#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    WCHAR *str1;
    WCHAR *str2;

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    str1 = convert("Foo!");
    str2 = convert("");

    OutputDebugStringW(str1);

    OutputDebugStringW(str2);

    free(str1);
    free(str2);

    PAL_Terminate();
    return PASS;
}
