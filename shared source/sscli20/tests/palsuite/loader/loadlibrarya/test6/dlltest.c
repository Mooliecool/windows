/*=============================================================================
**
** Source:    dllmain.c
**
** Purpose: Test to ensure DllMain() is called with DLL_THREAD_DETACH
**          only the initial time that the library is loaded.
** 
** Depends: None
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

/* count of the number of times DllMain()
 * was called with THREAD_ATTACH.
 */
static int g_attachCount = 0;

/* standard DllMain() */
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved)
{
    switch( reason )
    {
        case DLL_PROCESS_ATTACH:
        {
            g_attachCount++;
            break;
        }
    
        case DLL_PROCESS_DETACH:
        {
            break;
        }
        
        case DLL_THREAD_ATTACH:
        {
            break;
        }
    
        case DLL_THREAD_DETACH:
        {
            break;
        }
    }

    return TRUE;
}

#if _WIN32
BOOL __stdcall _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD reason, LPVOID lpvReserved)
{
    return DllMain(hinstDLL, reason, lpvReserved);
}
#endif



/* function to return the current attach count */
#if _WIN32
__declspec(dllexport)
#endif
int PALAPI GetAttachCount( void )
{
    return g_attachCount;
}

