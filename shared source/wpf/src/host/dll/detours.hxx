//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Various Win32 function detours. 
//      See also CCookieShim.
//      The Detours library can be found on http://toolbox. It has a nice help file with it.
//
//  History:
//      2009/04/09   [....]     Created
//
//------------------------------------------------------------------------

#pragma once

class Detours
{
    Detours();
public:
    static HRESULT Init();
    static void Uninit();

private:
    static HWND (WINAPI *s_pfGetActiveWindow)();
    static HWND WINAPI GetActiveWindowDetour();
};
