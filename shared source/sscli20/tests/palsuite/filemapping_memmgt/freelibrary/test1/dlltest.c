/*=====================================================================
** 
** Source:  dlltest.c (FreeLibrary test dll)
**
** Purpose: This file will be used to create the shared library
**          that will be used in the FreeLibraryAndExitThread
**          test. A very simple shared library, with one function
**          "DllTest" which merely returns 1.
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
#include "rotor_pal.h"

#if WIN32
__declspec(dllexport)
#endif

int __stdcall DllTest()
{
    return 1;
}

#ifdef WIN32
int __stdcall _DllMainCRTStartup(void *hinstDLL, int reason, void * lpvReserved)
{
    return 1;
}
#endif
