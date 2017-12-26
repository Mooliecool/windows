// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: dllmain.cpp
// 
// ===========================================================================
/*++

Abstract:

    Dll Main entry point


--*/

#include "rotor_palrt.h"

#include "convert.h"
void InitNumInfo(BOOL fInit);

ROTOR_PAL_CTOR_TEST_BODY(ROTOR_PALRT);

//----------------------------------------------------------------------------
extern "C"
BOOL WINAPI DllMain( HINSTANCE hInst, DWORD dwReason, LPVOID pvReserved )
{

    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            ROTOR_PAL_CTOR_TEST_RUN(ROTOR_PALRT);
            // initialize PAL
            if (PAL_Initialize(NULL, NULL) != 0)
                return FALSE;

            // initialize APP_DATA
            if ((g_itlsAppData = TlsAlloc()) == ITLS_EMPTY) {
                return FALSE;
            }
            InitNumInfo(TRUE);
            break;

        case DLL_PROCESS_DETACH:
            ReleaseAppData();
            InitNumInfo(FALSE);
            if (g_itlsAppData != ITLS_EMPTY) {
                TlsFree(g_itlsAppData);
            }

            PAL_Terminate();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            ReleaseAppData();
            break;
    }
    return TRUE;
}
