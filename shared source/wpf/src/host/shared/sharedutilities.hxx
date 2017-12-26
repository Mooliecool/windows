//+-----------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//    This file defines managed functions that we do not want to be
//    part of the exe.
//
//  History
//      2005/05/04-[....]
//          Moved InvokeBrowser to a shared location
//      2007/09/20-[....]
//          Ported Windows->DevDiv. See SourcesHistory.txt.
//
//------------------------------------------------------------------------

#pragma once

#include "String.hxx"
#include "StringMap.hxx"
#include <unknwn.h>

HRESULT GetDefaultBrowser(__inout_ecount(nBufferSize)LPCWSTR pszCommandlineToExec, size_t nBufferSize);
HRESULT GetURLFromCommandLine(__in LPWSTR pszCommandStart, __deref_out_ecount(1) LPWSTR* ppszURL);
HRESULT InvokeBrowser(__in LPCWSTR pszURL); 

HRESULT GetRegistryDWORD(__in HKEY hKey, __in LPWSTR pswzSubKey, __in LPWSTR pswzName, __out_ecount(1) DWORD& dwValue, __in DWORD dwDefaultValue);
HRESULT GetRegistryString(__in HKEY hKey, __in LPWSTR pswzSubKey, __in LPWSTR pswzName, CString& strValue);
HRESULT SetRegistryString(__in HKEY hKey, __in LPCWSTR pswzSubKey, __in LPCWSTR pswzName, CString& strValue);
HRESULT GetStringMapFromRegistry(__in HKEY hKey, __in LPWSTR pswzSubKey, CStringMap<CString*>& map);
HRESULT GetVersionMapFromRegistry(__in HKEY hKey, CStringMap<CString*>& map);

DWORD MsgWaitForQuitAndMultipleObjects(DWORD cEvents, __in_ecount(cEvents) HANDLE *phEvents, DWORD dwMilliSeconds, __out_ecount(1) BOOL* fWmQuit);

int GetProcessIntegrityLevel(DWORD processId);

template<typename PF>
bool GetProcAddress(HMODULE hModule, LPCSTR functionName, __out PF *pf)
{
    *pf = reinterpret_cast<PF>(::GetProcAddress(hModule, functionName));
    return *pf != NULL;
}
template<typename PF>
bool GetProcAddress(LPCWSTR moduleName, LPCSTR functionName, __out PF *pf)
{
    HMODULE hMod = GetModuleHandle(moduleName);
    return hMod && GetProcAddress(hMod, functionName, pf);
}

void SignalAvalonStartupRelatedEvent(HANDLE hEvent);

HANDLE TerminateIfNoHost(DWORD dwWaitTime);
HRESULT CancelTerminateIfNoHost(HANDLE hNoHostTimer);

HRESULT GetDeploymentDataFromManifest(
    __in LPCWSTR pwzDeploymentUri,
    __in_opt LPCWSTR pwzDeploymentManifestPath,
    __in CString& strApplicationIdentity,
    __in CString& strArchitecture,
    __in CString& strCodebase,
    __in CString& strProvider
    );

STDMETHODIMP
QueryService( __in_ecount(1) IUnknown* pUnk,
              __in_ecount(1) REFGUID guidService,
              __in_ecount(1) REFIID iid,
              __deref_out_ecount(1) void ** pResult);


struct UriSecurityId
{
    BYTE id[MAX_SIZE_SECURITY_ID];
    DWORD idLen;

    HRESULT Set(LPCWSTR uri, IInternetSecurityManager *pSecMgr)
    {
        idLen = MAX_SIZE_SECURITY_ID;
        return pSecMgr->GetSecurityId(uri, id, &idLen, 0);
    }

    bool operator==(const UriSecurityId &another) const
    {
        // CompareSecurityIds() is wrong in general. It ignores subdomains and port numbers.
        return idLen == another.idLen && memcmp(id, another.id, idLen) == 0;
    }
    bool operator!=(const UriSecurityId &another) const { return !(*this == another); }
};

HRESULT CheckSameDomain(LPCWSTR url1, LPCWSTR url2, IInternetSecurityManager *pSecMgr, bool &sameDomain);


//*********************************************************************************
// Debug Help functions
//*********************************************************************************
#if DEBUG
    void DebugInit(__in LPTSTR pszModule);
    void DebugInit(__in DWORD dwLevel);
    void DebugPrintf(__in DWORD dwLevel, __in LPTSTR pszFormat, ...);
    void DebugAssert(__in LPTSTR pszFile, int nLine, __in LPTSTR pszCondition);
    
    HRESULT ShellExecUri(__in PWSTR pwzUrl);

    #define DPFINIT     DebugInit
    #define DPF         DebugPrintf

#else
    // macro trickery.  anything with DPF in retail becomes a comment.
    #define SLASH /
    #define DPFINIT /SLASH
    #define DPF /SLASH
#endif

// Define a custom nofication id for WM_NOTIFY messages used to
// signal that avalon can be started.
#define AVALON_STARTUP_CHECK 0x01
