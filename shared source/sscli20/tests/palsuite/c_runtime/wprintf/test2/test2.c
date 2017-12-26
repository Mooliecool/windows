/*============================================================================
**
** Source:  test2.c
**
** Purpose: Test #2 for the wprintf function. Tests the string specifier
**          (%s).
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
#include "../wprintf.h"



int __cdecl main(int argc, char *argv[])
{

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    DoStrTest(L"foo %s", L"bar", L"foo bar");
    DoStrTest(L"foo %ws", L"bar", L"foo bar");
    DoStrTest(L"foo %ls", L"bar", L"foo bar");
    DoStrTest(L"foo %ws", L"bar", L"foo bar");
    DoStrTest(L"foo %Ls", L"bar", L"foo bar");
    DoStrTest(L"foo %I64s", L"bar", L"foo bar");
    DoStrTest(L"foo %5s", L"bar", L"foo   bar");
    DoStrTest(L"foo %.2s", L"bar", L"foo ba");
    DoStrTest(L"foo %5.2s", L"bar", L"foo    ba");
    DoStrTest(L"foo %-5s", L"bar", L"foo bar  ");
    DoStrTest(L"foo %05s", L"bar", L"foo 00bar");

    PAL_Terminate();
    return PASS;    
}

