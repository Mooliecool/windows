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
//*****************************************************************************
// MSCoree.cpp
//*****************************************************************************
#include "stdafx.h"                     // Standard header.

#include <utilcode.h>                   // Utility helpers.
#include <posterror.h>                  // Error handlers
#define INIT_GUIDS  
#include <corpriv.h>
#include <winwrap.h>
#include <internaldebug.h>
#include <mscoree.h>
#include "shimload.h"


// Locals.
BOOL STDMETHODCALLTYPE EEDllMain( // TRUE on success, FALSE on error.
                       HINSTANCE    hInst,                  // Instance handle of the loaded module.
                       DWORD        dwReason,               // Reason for loading.
                       LPVOID       lpReserved);                // Unused.


// Meta data startup/shutdown routines.
void  InitMd();
void  UninitMd();
STDAPI  MetaDataDllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv);
STDAPI  GetMDInternalInterface(
    LPVOID      pData, 
    ULONG       cbData, 
    DWORD       flags,                  // [IN] MDInternal_OpenForRead or MDInternal_OpenForENC
    REFIID      riid,                   // [in] The interface desired.
    void        **ppIUnk);              // [out] Return interface on success.
STDAPI GetMDInternalInterfaceFromPublic(
    IUnknown    *pIUnkPublic,           // [IN] Given scope.
    REFIID      riid,                   // [in] The interface desired.
    void        **ppIUnkInternal);      // [out] Return interface on success.
STDAPI GetMDPublicInterfaceFromInternal(
    void        *pIUnkPublic,           // [IN] Given scope.
    REFIID      riid,                   // [in] The interface desired.
    void        **ppIUnkInternal);      // [out] Return interface on success.
STDAPI MDReOpenMetaDataWithMemory(
    void        *pImport,               // [IN] Given scope. public interfaces
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData);                // [in] Size of the data pointed to by pData.

STDAPI MDReOpenMetaDataWithMemoryEx(
    void        *pImport,               // [IN] Given scope. public interfaces
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData,                 // [in] Size of the data pointed to by pData.
    DWORD       dwReOpenFlags);         // [in] ReOpen flags       


// Buffer-overrun protection



// Globals.
HINSTANCE       g_hThisInst;            // This library.

//*****************************************************************************
// Handle lifetime of loaded library.
//*****************************************************************************

ROTOR_PAL_CTOR_TEST_BODY(MSCORWKS);


extern "C"
BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    STATIC_CONTRACT_NOTHROW;

    static HMODULE hModPALRT = NULL;
    static HMODULE hModSSCorEE = NULL;

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Save the module handle.
            g_hThisInst = (HINSTANCE)hInstance;


            ROTOR_PAL_CTOR_TEST_RUN (MSCORWKS);
            // Register libraries.
            if ((hModPALRT = PAL_RegisterLibrary(L"rotor_palrt")) == NULL ||
                (hModSSCorEE = PAL_RegisterLibrary(L"sscoree")) == NULL)
            {
                return FALSE;
            }

            // Prevent buffer-overruns
            // If buffer is overrun, it is possible the saved callback has been trashed.
            // The callback is unsafe.
            //SetBufferOverrunHandler();

            // Init unicode wrappers.
            OnUnicodeSystem();

            if (!EEDllMain((HINSTANCE)hInstance, dwReason, lpReserved))
                return FALSE;

            InitMd();

            // Debug cleanup code.
            _DbgInit((HINSTANCE)hInstance);
        }
        break;

    case DLL_PROCESS_DETACH:
        {
            EEDllMain((HINSTANCE)hInstance, dwReason, lpReserved);
    
            UninitMd();
            _DbgUninit();
        
            // Unregister libraries.
            PAL_UnregisterLibrary(hModSSCorEE);
            PAL_UnregisterLibrary(hModPALRT);
        }
        break;

    case DLL_THREAD_DETACH:
        {
            EEDllMain((HINSTANCE)hInstance, dwReason, lpReserved);
        }
        break;
    }

    return TRUE;
}


HINSTANCE GetModuleInst()
{
    LEAF_CONTRACT;
    return (g_hThisInst);
}

// 
// 
// 


STDAPI InternalDllGetClassObject(
    REFCLSID rclsid,
    REFIID riid,
    LPVOID FAR *ppv)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_SO_TOLERANT;

    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    
    if (rclsid == CLSID_CorMetaDataDispenser || rclsid == CLSID_CorMetaDataDispenserRuntime ||
        rclsid == CLSID_CorRuntimeHost || rclsid == CLSID_CLRRuntimeHost || rclsid == CLSID_TypeNameFactory)
    {
        hr = MetaDataDllGetClassObject(rclsid, riid, ppv);
    }

    END_SO_INTOLERANT_CODE;
    return hr;
}  // InternalDllGetClassObject

STDAPI DllGetClassObjectInternal(
                                 REFCLSID rclsid,
                                 REFIID riid,
                                 LPVOID FAR *ppv)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_ENTRY_POINT;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    
    // InternalDllGetClassObject exists to resolve an issue
    // on FreeBSD, where libsscoree.so's DllGetClassObject's
    // call to DllGetClassObjectInternal() was being bound to
    // the implementation in libmscordbi.so, not the one in
    // libsscoree.so.  The fix is to disambiguate the name.
    hr = InternalDllGetClassObject(rclsid, riid, ppv);
    END_ENTRYPOINT_NOTHROW;

    return hr;
}


// ---------------------------------------------------------------------------
// %%Function: MetaDataGetDispenser
// This function gets the Dispenser interface given the CLSID and REFIID.
// ---------------------------------------------------------------------------
STDAPI MetaDataGetDispenser(            // Return HRESULT
    REFCLSID    rclsid,                 // The class to desired.
    REFIID      riid,                   // Interface wanted on class factory.
    LPVOID FAR  *ppv)                   // Return interface pointer here.
{

    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(ppv));
    } CONTRACTL_END;
    
    NonVMComHolder<IClassFactory> pcf(NULL);
    HRESULT hr;
    BEGIN_ENTRYPOINT_NOTHROW;

    IfFailGo(MetaDataDllGetClassObject(rclsid, IID_IClassFactory, (void **) &pcf));
    hr = pcf->CreateInstance(NULL, riid, ppv);

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return (hr);
}

// ---------------------------------------------------------------------------
// %%Function: GetMetaDataInternalInterface
// This function gets the IMDInternalImport given the metadata on memory.
// ---------------------------------------------------------------------------
STDAPI  GetMetaDataInternalInterface(
    LPVOID      pData,                  // [IN] in memory metadata section
    ULONG       cbData,                 // [IN] size of the metadata section
    DWORD       flags,                  // [IN] MDInternal_OpenForRead or MDInternal_OpenForENC
    REFIID      riid,                   // [IN] desired interface
    void        **ppv)                  // [OUT] returned interface
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pData));
        PRECONDITION(CheckPointer(ppv));
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = GetMDInternalInterface(pData, cbData, flags, riid, ppv);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: GetMetaDataInternalInterfaceFromPublic
// This function gets the internal scopeless interface given the public
// scopeless interface.
// ---------------------------------------------------------------------------
STDAPI  GetMetaDataInternalInterfaceFromPublic(
    IUnknown    *pv,                    // [IN] Given interface.
    REFIID      riid,                   // [IN] desired interface
    void        **ppv)                  // [OUT] returned interface
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pv));
        PRECONDITION(CheckPointer(ppv));
    } CONTRACTL_END;
    
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = GetMDInternalInterfaceFromPublic(pv, riid, ppv);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: GetMetaDataPublicInterfaceFromInternal
// This function gets the public scopeless interface given the internal
// scopeless interface.
// ---------------------------------------------------------------------------
STDAPI  GetMetaDataPublicInterfaceFromInternal(
    void        *pv,                    // [IN] Given interface.
    REFIID      riid,                   // [IN] desired interface.
    void        **ppv)                  // [OUT] returned interface
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pv));
        PRECONDITION(CheckPointer(ppv));
        ENTRY_POINT;
    } CONTRACTL_END;
    
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = GetMDPublicInterfaceFromInternal(pv, riid, ppv);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}


// ---------------------------------------------------------------------------
// %%Function: ReopenMetaDataWithMemory
// This function gets the public scopeless interface given the internal
// scopeless interface.
// ---------------------------------------------------------------------------
STDAPI ReOpenMetaDataWithMemory(
    void        *pUnk,                  // [IN] Given scope. public interfaces
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData)                 // [in] Size of the data pointed to by pData.
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pUnk));
        PRECONDITION(CheckPointer(pData));
    } CONTRACTL_END;

    HRESULT hr = S_OK;    

    BEGIN_ENTRYPOINT_NOTHROW;
    hr = MDReOpenMetaDataWithMemory(pUnk, pData, cbData);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: ReopenMetaDataWithMemoryEx
// This function gets the public scopeless interface given the internal
// scopeless interface.
// ---------------------------------------------------------------------------
STDAPI ReOpenMetaDataWithMemoryEx(
    void        *pUnk,                  // [IN] Given scope. public interfaces
    LPCVOID     pData,                  // [in] Location of scope data.
    ULONG       cbData,                 // [in] Size of the data pointed to by pData.
    DWORD       dwReOpenFlags)          // [in] ReOpen flags              
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pUnk));
        PRECONDITION(CheckPointer(pData));
    } CONTRACTL_END;

    HRESULT hr = S_OK;    

    BEGIN_ENTRYPOINT_NOTHROW;
    hr = MDReOpenMetaDataWithMemoryEx(pUnk, pData, cbData, dwReOpenFlags);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}



// ---------------------------------------------------------------------------
// %%Function: GetAssemblyMDImport
// This function gets the IMDAssemblyImport given the filename
// ---------------------------------------------------------------------------
STDAPI GetAssemblyMDImport(             // Return code.
    LPCWSTR     szFileName,             // [in] The scope to open.
    REFIID      riid,                   // [in] The interface desired.
    IUnknown    **ppIUnk)               // [out] Return interface on success.
{
    CONTRACTL
    {
        NOTHROW;
        ENTRY_POINT;
    }
    CONTRACTL_END;
    HRESULT hr=S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr=GetAssemblyMDInternalImport(szFileName, riid, ppIUnk);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: GetAssemblyMDImport
// This function gets the IMDAssemblyImport given the IStream
// ---------------------------------------------------------------------------
STDAPI GetAssemblyMDImportByStream(     // Return code.
    IStream     *pIStream,              // [in] The IStream for the file
    UINT64      AssemblyId,             // [in] Unique Id for the file
    REFIID      riid,                   // [in] The interface desired.
    IUnknown    **ppIUnk)               // [out] Return interface on success.
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pIStream));
        PRECONDITION(CheckPointer(ppIUnk));
    } CONTRACTL_END;
    
    return GetAssemblyMDInternalImportByStream(pIStream, AssemblyId, riid, ppIUnk);
}

// ---------------------------------------------------------------------------
// %%Function: CoInitializeCor
// 
// Parameters:
//  fFlags                  - Initialization flags for the engine.  See the
//                              COINITICOR enumerator for valid values.
// 
// Returns:
//  S_OK                    - On success
// 
// Description:
//  Reserved to initialize the Cor runtime engine explicitly.  This currently
//  does nothing.
// ---------------------------------------------------------------------------
STDAPI          CoInitializeCor(DWORD fFlags)
{
    WRAPPER_CONTRACT;

    BEGIN_ENTRYPOINT_NOTHROW;

    // Since the CLR doesn't currently support being unloaded, we don't hold a ref
    // count and don't even pretend to try to unload.
    END_ENTRYPOINT_NOTHROW;

    return (S_OK);
}

// ---------------------------------------------------------------------------
// %%Function: CoUninitializeCor
// 
// Parameters:
//  none
// 
// Returns:
//  Nothing
// 
// Description:
//  Function to indicate the client is done with the CLR. This currently does
//  nothing.
// ---------------------------------------------------------------------------
STDAPI_(void)   CoUninitializeCor(void)
{
    WRAPPER_CONTRACT;

    BEGIN_ENTRYPOINT_VOIDRET;

    // Since the CLR doesn't currently support being unloaded, we don't hold a ref
    // count and don't even pretend to try to unload.
    END_ENTRYPOINT_VOIDRET;

}

// Undef LoadStringRC & LoadStringRCEx so we can export these functions.
#undef LoadStringRC
#undef LoadStringRCEx

// ---------------------------------------------------------------------------
// %%Function: LoadStringRC
// 
// Parameters:
//  none
// 
// Returns:
//  Nothing
// 
// Description:
//  Function to load a resource based on it's ID.
// ---------------------------------------------------------------------------
STDAPI LoadStringRC(
    UINT iResourceID, 
    __out_ecount(iMax) __out_z LPWSTR szBuffer, 
    int iMax, 
    int bQuiet
)
{
    WRAPPER_CONTRACT;

    HRESULT hr = S_OK;

    if (NULL == szBuffer)
        return E_INVALIDARG;
    if (0 == iMax)
        return E_INVALIDARG;
    
    BEGIN_ENTRYPOINT_NOTHROW;
    hr = UtilLoadStringRC(iResourceID, szBuffer, iMax, bQuiet);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: LoadStringRCEx
// 
// Parameters:
//  none
// 
// Returns:
//  Nothing
// 
// Description:
//  Ex version of the function to load a resource based on it's ID.
// ---------------------------------------------------------------------------
STDAPI LoadStringRCEx(
    LCID lcid,
    UINT iResourceID, 
    __out_ecount(iMax) __out_z LPWSTR szBuffer, 
    int iMax, 
    int bQuiet,
    int *pcwchUsed
)
{
    WRAPPER_CONTRACT;
    HRESULT hr = S_OK;

    if (NULL == szBuffer)
        return E_INVALIDARG;
    if (0 == iMax)
        return E_INVALIDARG;
    
    BEGIN_ENTRYPOINT_NOTHROW;   
    hr = UtilLoadStringRCEx(lcid, iResourceID, szBuffer, iMax, bQuiet, pcwchUsed);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// Redefine them as errors to prevent people from using these from inside the rest of the compilation unit.
#define LoadStringRC __error("From inside the CLR, use UtilLoadStringRC; LoadStringRC is only meant to be exported.")
#define LoadStringRCEx __error("From inside the CLR, use UtilLoadStringRCEx; LoadStringRC is only meant to be exported.")


#ifndef ROTOR_VERSION_NUMBER
#define ROTOR_VERSION_NUMBER (L"v2.0.0")
#endif

// Note: These are shim-free versions of 
//      GetCORRequiredVersion
//      GetCORVersion
//      GetCORSystemDirectory
//      LoadLibraryShim
//  for FEATURE_PAL. The real shim-aware versions are in clr\src\dlls\shim.cpp

STDAPI GetCORRequiredVersion(__out_ecount_part(cchBuffer, *pdwlength) LPWSTR pbuffer, DWORD cchBuffer, __out DWORD* pdwlength)
{

    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pbuffer, NULL_OK));
        PRECONDITION(CheckPointer(pdwlength));
    } CONTRACTL_END;

    BEGIN_ENTRYPOINT_NOTHROW;
    HRESULT hr = S_OK;
    hr = GetCORVersion(pbuffer, cchBuffer, pdwlength);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

STDAPI GetCORVersion(__out_ecount_part(cchBuffer, *pdwlength) LPWSTR pbuffer, 
                     DWORD cchBuffer,
                     __out DWORD* pdwlength)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pbuffer, NULL_OK));
        PRECONDITION(CheckPointer(pdwlength));
    } CONTRACTL_END;
        
    HRESULT hr;
    BEGIN_ENTRYPOINT_NOTHROW;

    DWORD lgth = (DWORD)(wcslen(ROTOR_VERSION_NUMBER) + 1);
    if(lgth > cchBuffer)
    {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else
    {
        if (!pbuffer)
        {
            hr = E_POINTER;
        }
        else
        {
            wcsncpy(pbuffer, ROTOR_VERSION_NUMBER, lgth);
            hr = S_OK;
        }
    }
    *pdwlength = lgth;
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

STDAPI GetCORSystemDirectory(__out_ecount_part_opt(cchBuffer, *pdwlength) LPWSTR pbuffer, 
                             DWORD  cchBuffer,
                             __out_opt DWORD* pdwlength)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pbuffer, NULL_OK));
        PRECONDITION(CheckPointer(pdwlength, NULL_OK));
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;
    
    if(pdwlength == NULL)
        IfFailGo(E_POINTER);

    if (pbuffer == NULL)
        IfFailGo(E_POINTER);

    if (!PAL_GetPALDirectory(pbuffer, cchBuffer)) {
        IfFailGo(HRESULT_FROM_GetLastError());
    }

    // Include the null terminator in the length
    *pdwlength = wcslen(pbuffer)+1;
ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

STDAPI LoadLibraryShim(LPCWSTR szDllName, LPCWSTR szVersion, LPVOID pvReserved, HMODULE *phModDll)
{

    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(szDllName, NULL_OK));
        PRECONDITION(CheckPointer(szVersion));
        PRECONDITION(CheckPointer(pvReserved, NULL_OK));
        PRECONDITION(CheckPointer(phModDll));
    } CONTRACTL_END;

    if (szDllName == NULL)
        return E_POINTER;

    HRESULT hr = S_OK;
    
    BEGIN_ENTRYPOINT_NOTHROW;

    WCHAR szDllPath[_MAX_PATH];

    if (!PAL_GetPALDirectoryW(szDllPath, _MAX_PATH)) {
        IfFailGo(HRESULT_FROM_GetLastError());
    }
    wcsncat(szDllPath, szDllName, _MAX_PATH - wcslen(szDllPath));
    
    if ((*phModDll = WszLoadLibrary(szDllPath)) == NULL)
        IfFailGo(HRESULT_FROM_GetLastError());


ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

// Note: These are shim-free versions of 
//      GetInternalSystemDirectory
//      SetInternalSystemDirectory
//  for FEATURE_PAL. The real shim-aware versions are in clr\src\shimload\delayload.cpp

static DWORD g_dwSystemDirectory = 0;
static WCHAR g_pSystemDirectory[_MAX_PATH + 1];

HRESULT GetInternalSystemDirectory(__out_ecount_part(*pdwLength,*pdwLength) LPWSTR buffer, __inout DWORD* pdwLength)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(buffer, NULL_OK));
        PRECONDITION(CheckPointer(pdwLength));
    } CONTRACTL_END;
    
    if (g_dwSystemDirectory == 0)
        SetInternalSystemDirectory();

    //
    // g_dwSystemDirectory includes the NULL in its count!
    //
    if(*pdwLength < g_dwSystemDirectory) 
    {
        *pdwLength = g_dwSystemDirectory;
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    //
    // wcsncpy_s will automatically append a null and g_dwSystemDirectory 
    // includes the null in its count, so we have to subtract 1.
    //
    wcsncpy_s(buffer, *pdwLength, g_pSystemDirectory, g_dwSystemDirectory-1);
    *pdwLength = g_dwSystemDirectory;
    return S_OK;
}

HRESULT SetInternalSystemDirectory()
 {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    HRESULT hr = S_OK;

    if(g_dwSystemDirectory == 0) {
        DWORD len;
        
        hr = GetCORSystemDirectory(g_pSystemDirectory, _MAX_PATH+1, &len);

        if(FAILED(hr)) {
            g_pSystemDirectory[0] = L'\0';
            g_dwSystemDirectory = 1;
        }
        else{
            g_dwSystemDirectory = len;
        }
    }

    return hr;
}



