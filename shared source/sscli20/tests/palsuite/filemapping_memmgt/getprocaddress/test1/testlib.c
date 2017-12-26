/*=============================================================================
**
** Source: testlib.c (filemapping_memmgt\getprocaddress\test1)
**
** Purpose: Create a simple library containing one function
**          to test GetProcAddress
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
**===========================================================================*/
#include "rotor_pal.h"

#if WIN32
__declspec(dllexport)
#endif

/**
 * Simple function that returns i+1
 */
int __stdcall SimpleFunction(int i)
{
    return i+1;
}

#if WIN32
int __stdcall _DllMainCRTStartup(void *hinstDLL, int reason, void *lpvReserved)
{
    return 1;
}
#endif
