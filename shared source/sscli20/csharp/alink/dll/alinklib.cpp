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
// File: alinklib.cpp
//
// Implementation of DLL Exports.
// ===========================================================================

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f alinkps.mk in the project directory.

#include "stdafx.h"
#include "asmlink.h"
#include "common.h"
#include "satellite.h"

static HINSTANCE hModuleMessages = NULL;

CComModule _Module;

CComModule & GetATLModule() 
{
    return _Module;
}

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_AssemblyLinker, CAsmLink)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    static HMODULE hCorEE = NULL;
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        hCorEE = PAL_RegisterLibraryW(L"sscoree");
        if (!hCorEE) {
            return FALSE;
        }
        W_IsUnicodeSystem();
        _Module.Init(ObjectMap, hInstance);

        DisableThreadLibraryCalls(hInstance);
        break;

    case DLL_PROCESS_DETACH:
        _Module.Term();

        if (hModuleMessages) {
            PAL_FreeSatelliteResource((HSATELLITE)hModuleMessages);
            hModuleMessages = NULL;
        }
        PAL_UnregisterLibraryW(hCorEE);
        hCorEE = NULL;
        break;

    default:
        break;
    }

    return TRUE;    // ok
}

HRESULT WINAPI CreateALink(REFIID riid, IUnknown** ppInterface)
{
    HRESULT                 hr;
    CComObject<CAsmLink>    *pObj;

    if (SUCCEEDED (hr = CComObject<CAsmLink>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->Init()) ||
            FAILED (hr = pObj->QueryInterface (riid, (void **)ppInterface)))
        {
            delete pObj;
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// GetMesageDll -- find and load the message DLL. Returns 0 if the message
// DLL could not be located or loaded. The message DLL should be either in a 
// subdirectory whose name is a language id, or in the current directory. We have
// a complex set of rules to figure out which language ids to try.
HINSTANCE WINAPI GetALinkMessageDll ()
{

    return GetMessageDllWorker(hModuleMessages, (LANGID)-1, MESSAGE_DLLA, NULL);
}
