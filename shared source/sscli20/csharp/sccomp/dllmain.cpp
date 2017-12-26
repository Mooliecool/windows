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
// Defines the entry point for the DLL application.
// ===========================================================================

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    static HMODULE hCorEE = NULL;
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        if (FAILED (CTableImpl::Init())) {
            return FALSE;
        }
        hCorEE = PAL_RegisterLibraryW(L"sscoree");
        if (!hCorEE) {
            return FALSE;
        }
        W_IsUnicodeSystem();
        _Module.Init (ObjectMap, hInstance);

        // We don't need additional thread notifications
        DisableThreadLibraryCalls(hInstance);
        break;

    case DLL_PROCESS_DETACH:
        COptionData::Shutdown();
        _Module.Term();
        if (hModuleMessages) {
            PAL_FreeSatelliteResource((HSATELLITE)hModuleMessages);
            hModuleMessages = NULL;
        }
        PAL_UnregisterLibraryW(hCorEE);
        hCorEE = NULL;
        CTableImpl::Shutdown();
        break;

    default:
        break;
    }

    return TRUE;
}
