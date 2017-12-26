/*=============================================================================
**
** Source: dllmain.c
**
** Purpose: Test to ensure DllMain() is called with THREAD_DETACH
**          when a thread in the application calls ExitThread().
** 
** Dependencies: none
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

#include <palsuite.h>

/* count of the number of times DllMain() was called with THREAD_DETACH */
static int g_detachCount = 0;


/* standard DllMain() */
BOOL PALAPI DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved)
{
    switch( reason )
    {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }
    
        case DLL_PROCESS_DETACH:
        {
            break;
        }
        
        case DLL_THREAD_ATTACH:
            break;
    
        case DLL_THREAD_DETACH:
            /* increment g_detachCount */
            g_detachCount++;
            break;
    }
    return TRUE;
}

#ifdef WIN32
BOOL __stdcall _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
#endif


/* function to return the current detach count */
#ifdef WIN32
__declspec(dllexport)
#endif
int PALAPI GetDetachCount( void )
{
    return g_detachCount;
}
