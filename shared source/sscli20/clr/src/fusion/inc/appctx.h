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
#ifndef ASM_CTX_H
#define ASM_CTX_H

#include "list.h"
#include "fusconfig.h"
#include "bindresult.h"


// Private app context variable
#define ACTAG_APP_CFG_DOWNLOAD_ATTEMPTED   L"__FUSION_APPCFG_DOWNLOAD_ATTEMPTED__"
#define ACTAG_APP_CFG_FILE_HANDLE           L"__FUSION_APPCFG_FILE_HANDLE__"
#define ACTAG_APP_CFG_DOWNLOAD_INFO         L"__FUSION_APPCFG_DOWNLOAD_INFO__"
#define ACTAG_APP_CFG_DOWNLOAD_CS           L"__FUSION_APPCFG_DOWNLOAD_CS__"
#define ACTAG_APP_DYNAMIC_DIRECTORY         L"__FUSION_DYNAMIC_DIRECTORY__"
#define ACTAG_APP_CACHE_DIRECTORY           L"__FUSION_APP_CACHE_DIRECTORY__"
#define ACTAG_APP_BIND_HISTORY              L"__FUSION_BIND_HISTORY_OBJECT__"
#define ACTAG_APP_CACHE                     L"__FUSION_CACHE__"
#define ACTAG_APP_POLICY_CACHE              L"__FUSION_POLICY_CACHE__"
#define ACTAG_APP_CFG_PRIVATE_BINPATH       L"__FUSION_APP_CFG_PRIVATE_BINPATH__"

#define APP_CTX_PROPERTIES_HASH_SIZE       53

class SimpleRWLock;
class CDebugLog;

class CApplicationContext : public IApplicationContext
{

public:
    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();
    
    // IApplicationContext methods

    // Sets associated IAssemblyName*
    STDMETHOD(SetContextNameObject)(
        /* in */ LPASSEMBLYNAME pName);

    // Retrieves associated IAssemblyName*
    STDMETHOD(GetContextNameObject)(
        /* out */ LPASSEMBLYNAME *ppName);

    // Generic blob set keyed by string.
    STDMETHOD(Set)( 
        /* in */ LPCOLESTR szName, 
        /* in */ LPVOID pvValue, 
        /* in */ DWORD cbValue,
        /* in */ DWORD dwFlags);

    // Generic blob get keyed by string.
    STDMETHOD(Get)( 
        /* in      */ LPCOLESTR szName,
        /* out     */ LPVOID  pvValue,
        /* in, out */ LPDWORD pcbValue,
        /* in      */ DWORD   dwFlags);

    STDMETHOD(GetAssemblyBindingClosure)(
        /* [in]  */ IUnknown *pUnk,
        /* [out] */ IAssemblyBindingClosure **ppAsmClosure
        );

    HRESULT IsAppBaseFile(LPBOOL pbFile);

    STDMETHODIMP GetDynamicDirectory(
                    __out_ecount_opt(*pdwSize) LPWSTR wzDynamicDir, 
                    __inout DWORD *pdwSize);
    STDMETHODIMP GetAppCacheDirectory(
                    __out_ecount_opt(*pdwSize) LPWSTR wzCacheDir, 
                    __inout DWORD *pdwSize);
    STDMETHODIMP RegisterKnownAssembly(IAssemblyName *pName, LPCWSTR pwzAsmURL,
                                       IAssembly **ppAsmOut);
    STDMETHODIMP PrefetchAppConfigFile();

    HRESULT Lock();
    HRESULT Unlock();

    HRESULT ReadConfigSettings(CDebugLog *pdbglog, CAppCtxPolicyConfigs **ppConfig);

    CApplicationContext();
    ~CApplicationContext();

    HRESULT Init(LPASSEMBLYNAME pName);


    HRESULT GetBindingResultCache(CBindingResultCache **ppBindResult);

    HRESULT GetLoadContext(CLoadContext **ppLoadContext, LOADCTX_TYPE type);

private:
    
    HRESULT ReadConfigSettingsInternal(CDebugLog *pdbglog, CAppCtxPolicyConfigs *pConfig);

private:

    // Class manages linked list of Entrys
    class Entry 
    {
    public:
        DWORD  _dwSig;
        LPTSTR _szName;
        LPBYTE _pbValue;
        DWORD  _cbValue;
        DWORD  _dwFlags;    

        // d'tor nukes Entries
        Entry();
        ~Entry();
        
    };

    // Creates entries.
    HRESULT CreateEntry(LPCWSTR szName, LPVOID pvValue, 
        DWORD cbValue, DWORD dwFlags, Entry** pEntry);

    // Copies blob data, optionally freeing existing.
    HRESULT CopyData(Entry *pEntry, LPVOID pvValue, 
        DWORD cbValue, DWORD dwFlags, BOOL fFree);

    DWORD _dwSig;
    LONG  _cRef;
    CRITSEC_COOKIE _cs;

    BOOL _bReadConfigSettings;
    CAppCtxPolicyConfigs *_pConfig;

    BOOL  _bAppBaseIsFileUrl;

    // Associated IAssemblyName*
    LPASSEMBLYNAME _pName;

    // The managed list.
    SimpleRWLock*  _listLock;
    List<Entry *>  _properties[APP_CTX_PROPERTIES_HASH_SIZE];


    CBindingResultCache *_pBindingResultCache;

    CLoadContext *_pDefaultCtx;
    CLoadContext *_pLoadFromCtx;
};

BOOL InitSxsProcs();

#endif // ASM_CTX_H
