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
// File: cocreate.cpp
// 
// ===========================================================================
/*++

Abstract:

    Fake CoCreateInstance for PAL RT

Author:


Revision History:

--*/

#include "rotor_palrt.h"

#include "unknwn.h"

EXTERN_C const GUID CLSID_CorRuntimeHost;
EXTERN_C const GUID CLSID_CorMetaDataDispenser;
EXTERN_C const GUID CLSID_CorMetaDataDispenserRuntime;
EXTERN_C const GUID CLSID_CLRRuntimeHost;
EXTERN_C const GUID CLSID_TypeNameFactory;
EXTERN_C const GUID CLSID_CorSymWriter_SxS;
EXTERN_C const GUID CLSID_CorSymReader_SxS;
EXTERN_C const GUID CLSID_CorSymBinder_SxS;
EXTERN_C const GUID CLSID_CorpubPublish;

struct CoClass
{
    const CLSID *pClsid;
    LPCWSTR     dllName;
};

#define COCLASS(name, dll) { &CLSID_##name, MAKEDLLNAME_W(dll) },

static const struct CoClass g_CoClasses[] =
{
    COCLASS(CorRuntimeHost,              L"mscorwks")
    COCLASS(CorMetaDataDispenser,        L"mscorwks")
    COCLASS(CorMetaDataDispenserRuntime, L"mscorwks")
    COCLASS(CLRRuntimeHost,              L"mscorwks")
    COCLASS(TypeNameFactory,             L"mscorwks")
    COCLASS(CorSymWriter_SxS,            L"ildbsymbols")
    COCLASS(CorSymReader_SxS,            L"ildbsymbols")
    COCLASS(CorSymBinder_SxS,            L"ildbsymbols")
    COCLASS(CorpubPublish,               L"mscordbi")
};

typedef HRESULT __stdcall DLLGETCLASSOBJECT(REFCLSID rclsid,
                                            REFIID   riid,
                                            void   **ppv);

HRESULT PALAPI PAL_CoCreateInstance(REFCLSID   rclsid,
                             REFIID     riid,
                             void     **ppv)
{
    for (size_t i = 0; i < sizeof(g_CoClasses) / sizeof(g_CoClasses[0]); i++)
    {
        const CoClass *pCoClass = &g_CoClasses[i];

        if (*pCoClass->pClsid == rclsid)
        {
            HRESULT             hr;
            HINSTANCE           dll;
            DLLGETCLASSOBJECT   *dllGetClassObject;
            IClassFactory       *classFactory;
            WCHAR FullPath[_MAX_PATH];

            if (!PAL_GetPALDirectoryW(FullPath, _MAX_PATH)) {
                goto Win32Error;
            }
            if (wcslen(FullPath) + wcslen(pCoClass->dllName) >= _MAX_PATH) {
                SetLastError(ERROR_FILENAME_EXCED_RANGE);
                goto Win32Error;
            }
            wcsncat(FullPath, pCoClass->dllName, _MAX_PATH);
            
            dll = LoadLibraryW(FullPath);
            if (dll == NULL)
                goto Win32Error;

            dllGetClassObject = (DLLGETCLASSOBJECT*)GetProcAddress(dll, "DllGetClassObject");
            if (dllGetClassObject == NULL) {
                // The CLR shim exports a DllGetClassObject which in turn decides which DLL to load and
                // call DllGetClassObjectInternal on.  Without the shim, the PALRT must do the same
                // here.
                dllGetClassObject = (DLLGETCLASSOBJECT*)GetProcAddress(dll, "DllGetClassObjectInternal");
                if (dllGetClassObject == NULL) {
                    goto Win32Error;
                }
            }

            hr = (*dllGetClassObject)(rclsid, IID_IClassFactory, (void**)&classFactory);
            if (FAILED(hr))
                return hr;

            hr = classFactory->CreateInstance(NULL, riid, ppv);
            classFactory->Release();
            return hr;
        }
    }

    _ASSERTE(!"Unknown CLSID in PAL_CoCreateInstance");
    return CLASS_E_CLASSNOTAVAILABLE;

Win32Error:
    DWORD dwError = GetLastError();
    return HRESULT_FROM_WIN32(dwError);
}
