/*=====================================================================
**
** Source:  testlib.c
**
** Purpose: Simple library that counts thread attach/detach notifications
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
**===================================================================*/

#include <palsuite.h>

static int Count;

BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        Count = 0;
    }
    else if (fdwReason == DLL_THREAD_ATTACH ||
        fdwReason == DLL_THREAD_DETACH)
    {
        Count++;
    }

    return TRUE;
}

#ifdef WIN32
BOOL __stdcall _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
#endif

#ifdef WIN32
__declspec(dllexport)
#endif
int __stdcall GetCallCount()
{
    return Count;
}
