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

#include <windows.h>
#include <winbase.h>
#include <winerror.h>
#include <shlwapi.h>
#include "common.h"
#include "simplerwlock.hpp"
#include "fusionp.h"
#include "naming.h"
#include "appctx.h"
#include "list.h"
#include "actasm.h"
#include "policy.h"
#include "fusionheap.h"
#include "helpers.h"
#include "asm.h"
#include "nodefact.h"
#include "lock.h"
#include "dbglog.h"



#include "dlwrap.h"


// ---------------------------------------------------------------------------
// CApplicationContext::CreateEntry
// 
// Private func; Allocates and copies data input.
// ---------------------------------------------------------------------------
HRESULT CApplicationContext::CreateEntry(LPCTSTR szName, LPVOID pvValue, 
    DWORD cbValue, DWORD dwFlags, Entry** ppEntry)
{
    HRESULT hr = S_OK;
    DWORD cbName;

    // Allocate the entry.
    Entry *pEntry = NEW(Entry);
    if (!pEntry)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    
    // Copy name.
    cbName = (lstrlenW(szName) + 1) * sizeof(TCHAR);
    pEntry->_szName = NEW(TCHAR[cbName]);
    if (!pEntry->_szName)
    {
        SAFEDELETE(pEntry);
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    memcpy(pEntry->_szName, szName, cbName);

    // Allocate and copy data, don't free existing data.
    hr = CopyData(pEntry, pvValue, cbValue, dwFlags, FALSE);
    if (FAILED(hr)) {
        SAFEDELETE(pEntry);
        goto exit;
    }
    
    *ppEntry = pEntry;

exit:
    return hr;
}

CApplicationContext::Entry::Entry()
: _szName(NULL)
, _pbValue(NULL)
, _cbValue(0)
, _dwFlags(0)
{
    _dwSig = 0x59544e45; /* 'YTNE' */
}

// ---------------------------------------------------------------------------
// CApplicationContext::Entry dtor
// ---------------------------------------------------------------------------
CApplicationContext::Entry::~Entry()
{
    if (_dwFlags & APP_CTX_FLAGS_INTERFACE)
        ((IUnknown*) _pbValue)->Release();       
    else
        SAFEDELETEARRAY(_pbValue);

    SAFEDELETEARRAY(_szName);
}



// ---------------------------------------------------------------------------
// CApplicationContext::CopyData
//
// Private func; used to create and update entries.
// ---------------------------------------------------------------------------
HRESULT CApplicationContext::CopyData(Entry *pEntry, LPVOID pvValue, 
    DWORD cbValue, DWORD dwFlags, BOOL fFree)
{
    HRESULT hr = S_OK;
    LPBYTE  pbValue = NULL;

    // Input is straight blob.
    if (!(dwFlags & APP_CTX_FLAGS_INTERFACE))
    {
        if (cbValue) {
            pbValue = NEW(BYTE[cbValue]);
            if (!pbValue)
            {
                hr = E_OUTOFMEMORY;
                goto exit;
            }
            memcpy(pbValue, pvValue, cbValue);    

            if (!FusionCompareString(pEntry->_szName, ACTAG_APP_DYNAMIC_DIRECTORY)
             || !FusionCompareString(pEntry->_szName, ACTAG_APP_CACHE_DIRECTORY)
             || !FusionCompareString(pEntry->_szName, ACTAG_APP_DYNAMIC_BASE)
             || !FusionCompareString(pEntry->_szName, ACTAG_APP_CACHE_BASE)) {
                // make sure it is null terminated
                LPWSTR pwzDir = (LPWSTR)(pbValue);
                DWORD  cbLen = (lstrlenW(pwzDir)+1) * sizeof(WCHAR);
                if (cbLen > cbValue ) {
                    SAFEDELETEARRAY(pbValue);
                    hr = E_INVALIDARG;
                    goto exit;
                }
                // We are setting some directory. Make sure we don't have trailing slash
                PathRemoveBackslash(pwzDir);
            }
        }

        if (fFree) {
            SAFEDELETEARRAY(pEntry->_pbValue);
        }

        pEntry->_pbValue = pbValue;
        pEntry->_cbValue = cbValue;
    }
    // Input is Interface ptr.
    else
    {
        if (fFree) {
            ((IUnknown*) pEntry->_pbValue)->Release();       
        }

        pEntry->_pbValue = (LPBYTE) pvValue;
        pEntry->_cbValue = sizeof(IUnknown*);
        ((IUnknown*) pEntry->_pbValue)->AddRef();
    }

    pEntry->_dwFlags = dwFlags;

exit:
    return hr;
}


// ---------------------------------------------------------------------------
// CreateApplicationContext
// ---------------------------------------------------------------------------
STDAPI
CreateApplicationContext(
    IAssemblyName *pName,       // In, Optional
    LPAPPLICATIONCONTEXT *ppCtx)
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;
    
    CApplicationContext *pCtx = NULL;

    if (!ppCtx)
    {
        hr = E_INVALIDARG;
        goto exit;
    }
 
    pCtx = NEW(CApplicationContext);
    if (!pCtx)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = pCtx->Init(pName);

    if (FAILED(hr)) 
    {
        SAFERELEASE(pCtx);
        goto exit;
    }
    
    *ppCtx = pCtx;
exit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// CApplicationContext ctor
// ---------------------------------------------------------------------------
CApplicationContext::CApplicationContext()
{
    _dwSig = 0x58544341; /* 'XTCA' */
    _pName = NULL;
    _cRef = 1;
    _listLock = NULL;
    _cs = NULL;
    _bAppBaseIsFileUrl = TRUE;
    _pConfig = NULL;
    _bReadConfigSettings = FALSE;
    _pBindingResultCache = NULL;
    _pDefaultCtx = NULL;
    _pLoadFromCtx = NULL;
}

// ---------------------------------------------------------------------------
// CApplicationContext dtor
// ---------------------------------------------------------------------------
CApplicationContext::~CApplicationContext()
{
    HRESULT                               hr;
    DWORD                                 dwSize;
    HANDLE                                hFile = NULL;

    Entry *pEntry = NULL;

    if (_cs) {
        ClrDeleteCriticalSection(_cs);
    }

    // Fix for OOM bug 32119
    // We should not do anything if _listLock is not initialized.
    if (_listLock) {

            
        // Release lock on app.cfg

        dwSize = sizeof(HANDLE);
        hr = Get(ACTAG_APP_CFG_FILE_HANDLE, (void *)&hFile, &dwSize, 0);
        if (hr == S_OK && hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
        }

        // Release associated IAssemblyName*
        SAFERELEASE(_pName);

        SAFERELEASE(_pConfig);

        int                                i;
        LISTNODE                           pos;

        for (i = 0; i < APP_CTX_PROPERTIES_HASH_SIZE; i++) {
            pos = _properties[i].GetHeadPosition();

            while (pos) {
                pEntry = _properties[i].GetNext(pos);
                _ASSERTE(pEntry);
                SAFEDELETE(pEntry);
            }

            _properties[i].RemoveAll();
        }
    }

    SAFEDELETE(_listLock);
    SAFERELEASE(_pBindingResultCache);
    SAFERELEASE(_pDefaultCtx);
    SAFERELEASE(_pLoadFromCtx);
}

// ---------------------------------------------------------------------------
// CApplicationContext::Init
// ---------------------------------------------------------------------------
HRESULT CApplicationContext::Init(LPASSEMBLYNAME pName)
{
    HRESULT                                      hr = S_OK;

    _cs = ClrCreateCriticalSection("Fusion: CApplicationContext", CrstFusionAppCtx, CRST_REENTRANCY);
    if (!_cs) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    _listLock = NEW(SimpleRWLock(COOPERATIVE_OR_PREEMPTIVE, LOCK_TYPE_DEFAULT));
    if (!_listLock) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    // Set name if any.
    _pName = pName;

    if (_pName) {
        _pName->AddRef();
    }

Exit:
    return hr;
}


// ---------------------------------------------------------------------------
// CApplicationContext::SetContextNameObject
// ---------------------------------------------------------------------------
STDMETHODIMP
CApplicationContext::SetContextNameObject(LPASSEMBLYNAME pName)
{
    // Free existing name if any
    SAFERELEASE(_pName);

    // Set name.
    _pName = pName;
    if (_pName)
        _pName->AddRef();
    return S_OK;
}

// ---------------------------------------------------------------------------
// CApplicationContext::GetContextNameObject
// ---------------------------------------------------------------------------
STDMETHODIMP
CApplicationContext::GetContextNameObject(LPASSEMBLYNAME *ppName)
{
    if (!ppName)
        return E_INVALIDARG;

    *ppName = _pName;

    if (*ppName)
        (*ppName)->AddRef();

    return S_OK;
}

// ---------------------------------------------------------------------------
// CApplicationContext::Set
// ---------------------------------------------------------------------------
STDMETHODIMP
CApplicationContext::Set(LPCOLESTR szName, LPVOID pvValue, 
    DWORD cbValue, DWORD dwFlags)
{
    HRESULT                                     hr = S_OK;
    BOOL                                        fUpdate = FALSE;
    Entry                                      *pEntry;
    DWORD                                       dwHash = 0;
    LISTNODE                                    pos = NULL;
    LPVOID                                      pvValueInternal = pvValue;
    DWORD                                       cbValueInternal = cbValue;
    DWORD                                       dwSize = 0;
    LPWSTR                                      pwzPropertyUnescaped = NULL;
    
    if (!szName) {
        hr = E_INVALIDARG;
        goto exit;
    }
    
    // If interface ptr, byte count is optional.
    if (!cbValueInternal && (dwFlags & APP_CTX_FLAGS_INTERFACE))
        cbValueInternal = sizeof(IUnknown*);
        
    // Validate input.
    if (!pvValueInternal && cbValueInternal) {
        hr = E_INVALIDARG;
        goto exit;
    }


    // If a setting the app name or cache base (ie. anything that affects the
    // cache directory), then make sure the any previously cached CCache
    // objects are released (ie. so they get rebuilt with the right new
    // location on next use).

    if (!FusionCompareString(ACTAG_APP_NAME, szName) || !FusionCompareString(ACTAG_APP_CACHE_BASE, szName)) {
        hr = Set(ACTAG_APP_CACHE, 0, 0, 0);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    if (!FusionCompareString(szName, ACTAG_APP_BASE_URL, FALSE)) {
        pwzPropertyUnescaped = NEW(WCHAR[MAX_URL_LENGTH+1]);
        if (!pwzPropertyUnescaped)
        {
            hr = E_OUTOFMEMORY;
            goto exit;
        }

        dwSize = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape((LPWSTR)pvValue, pwzPropertyUnescaped, &dwSize, 0);
        if (FAILED(hr)) {
            goto exit;
        }

        pvValueInternal = pwzPropertyUnescaped;
        cbValueInternal = (lstrlenW(pwzPropertyUnescaped) + 1) * sizeof(WCHAR);

        _bAppBaseIsFileUrl = UrlIsW(pwzPropertyUnescaped, URLIS_FILEURL);
    }

    dwHash = HashString(szName, 0, APP_CTX_PROPERTIES_HASH_SIZE, TRUE);
    
    // Grab crit sect.
    _listLock->EnterWrite();
   
    pos = _properties[dwHash].GetHeadPosition();

    while(pos)
    {
        pEntry = _properties[dwHash].GetAt(pos);
        _ASSERTE(pEntry);

        if (!FusionCompareString(pEntry->_szName, szName))
        {
            fUpdate = TRUE;
            break;
        }
        pos = (LISTNODE)(((ListNode<Entry *> *) pos)->GetNext());
    }
    
    // If updating a current entry.
    if (fUpdate)
    {
        if (cbValueInternal)
        {
            // Copy data over, freeing previous.
            if (FAILED(hr = CopyData(pEntry, pvValueInternal, cbValueInternal, dwFlags, TRUE))) {
                _listLock->LeaveWrite();
                goto exit;
            }
        }
        else
        {
            // 0 byte count means remove entry.
           
            _properties[dwHash].RemoveAt(pos);
            SAFEDELETE(pEntry);
            hr = S_OK;
            _listLock->LeaveWrite();
            goto exit;
        }
    }
    // otherwise allocate a new entry.
    else
    {
        if (cbValueInternal) 
        {
            // Create new and push onto list.
            if (FAILED(hr = CreateEntry((LPOLESTR) szName, pvValueInternal, 
                    cbValueInternal, dwFlags, &pEntry))) {
                _listLock->LeaveWrite();
                goto exit;
            }

            if (!_properties[dwHash].AddHead(pEntry))
            {
                hr = E_OUTOFMEMORY;
                SAFEDELETE(pEntry);
                _listLock->LeaveWrite();
                goto exit;
            }
        }
        else
        {
            // Trying to create a new entry, but no byte count.
            hr = S_FALSE;
            _listLock->LeaveWrite();
            goto exit;
        }  
    }
        
    _listLock->LeaveWrite();

exit:

    SAFEDELETEARRAY(pwzPropertyUnescaped);
    return hr;
}

// ---------------------------------------------------------------------------
// CApplicationContext::Get
// ---------------------------------------------------------------------------
STDMETHODIMP
CApplicationContext::Get(LPCOLESTR szName, LPVOID pvValue, 
    LPDWORD pcbValue, DWORD dwFlags)
{
    HRESULT         hr = S_OK;
    Entry           *pEntry = NULL;
    BOOL            fFound = FALSE;
    LISTNODE        pos = NULL;
    DWORD           dwHash = 0;
    
    // Validate input.
    if (!szName || !pcbValue || (!pvValue && *pcbValue))
    {
        hr = E_INVALIDARG;
        goto exit;
    }


    dwHash = HashString(szName, 0, APP_CTX_PROPERTIES_HASH_SIZE, TRUE);

    _listLock->EnterRead();
   
    pos = _properties[dwHash].GetHeadPosition();

    while(pos)
    {
        pEntry = _properties[dwHash].GetNext(pos);
        _ASSERTE(pEntry);

        if (!FusionCompareString(pEntry->_szName, szName))
        {
            fFound = TRUE;
            break;
        }
    }

    // Entry found.
    if (fFound)
    {
        // Insufficient buffer case.
        if (*pcbValue < pEntry->_cbValue)
        {        
            *pcbValue = pEntry->_cbValue;
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            _listLock->LeaveRead();
            goto exit;
        }
        
        // If interface pointer addref and hand out.
        if (pEntry->_dwFlags & APP_CTX_FLAGS_INTERFACE)
        {
            *((IUnknown**) pvValue) = (IUnknown*) pEntry->_pbValue;
            ((IUnknown*) pEntry->_pbValue)->AddRef();
        }
        // Otherwise just copy blob.
        else    
            memcpy(pvValue, pEntry->_pbValue, pEntry->_cbValue);

        // Indicate byte count.
        *pcbValue = pEntry->_cbValue;
        hr = S_OK;
    }
    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }


    _listLock->LeaveRead();

exit:

    return hr;
}


// IUnknown methods

// ---------------------------------------------------------------------------
// CApplicationContext::AddRef
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CApplicationContext::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

// ---------------------------------------------------------------------------
// CApplicationContext::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CApplicationContext::Release()
{
    ULONG ulRef = 0;

    BEGIN_CLEANUP_ENTRYPOINT;


    ulRef = InterlockedDecrement(&_cRef);
    if (ulRef == 0) 
    {
        delete this;        
    }

    END_CLEANUP_ENTRYPOINT;

    return ulRef;
}

// ---------------------------------------------------------------------------
// CApplicationContext::QueryInterface
// ---------------------------------------------------------------------------
STDMETHODIMP
CApplicationContext::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;
    
    *ppv = NULL;

    if (riid==IID_IApplicationContext || riid == IID_IUnknown)
        *ppv = (IApplicationContext *) this;

    if (*ppv == NULL)
        return E_NOINTERFACE;

    AddRef();

    return S_OK;

} 

HRESULT CApplicationContext::IsAppBaseFile(LPBOOL pbFile)
{
    HRESULT hr = S_OK;

    *pbFile = _bAppBaseIsFileUrl;

    return hr;
}

STDMETHODIMP CApplicationContext::GetDynamicDirectory(
                    __out_ecount_opt(*pdwSize) LPWSTR wzDynamicDir, 
                    __inout DWORD *pdwSize)
{
    HRESULT                                 hr = S_OK;
    LPWSTR                                  wzDynamicBase = NULL;
    LPWSTR                                  wzAppName = NULL;
    LPWSTR                                  wzAppCtxDynamicDir = NULL;
    WCHAR                                   wzDir[MAX_PATH];
    DWORD                                   dwLen;

    wzDir[0] = L'\0';

    if (!pdwSize) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Check if the dynamic directory has already been set.

    hr = ::AppCtxGetWrapper(this, ACTAG_APP_DYNAMIC_DIRECTORY, &wzAppCtxDynamicDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (wzAppCtxDynamicDir) {
        _ASSERTE(lstrlenW(wzAppCtxDynamicDir));

        dwLen = lstrlenW(wzAppCtxDynamicDir) + 1;
        if (!wzDynamicDir || *pdwSize < dwLen) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            *pdwSize = dwLen;
            goto Exit;
        }

        *pdwSize = dwLen;

        hr = StringCchCopy(wzDynamicDir, *pdwSize, wzAppCtxDynamicDir);

        goto Exit;
    }

    // Dynamic directory not set. Calculate it.

    hr = ::AppCtxGetWrapper(this, ACTAG_APP_DYNAMIC_BASE, &wzDynamicBase);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ::AppCtxGetWrapper(this, ACTAG_APP_NAME, &wzAppName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!wzAppName || !wzDynamicBase) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

    PathRemoveBackslashW(wzDynamicBase);

    hr = StringCchPrintf(wzDir, MAX_PATH, L"%ws\\%ws", wzDynamicBase, wzAppName);
    if (FAILED(hr)) {
        goto Exit;
    }
    dwLen = lstrlenW(wzDir) + 1;


    if (!wzDynamicDir || *pdwSize < dwLen) {
        *pdwSize = dwLen;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    hr = StringCchCopy(wzDynamicDir, *pdwSize, wzDir);
    if (FAILED(hr)) {
        goto Exit;
    }
    *pdwSize = dwLen;

    // Cache this for future use.

    Set(ACTAG_APP_DYNAMIC_DIRECTORY, wzDynamicDir, dwLen * sizeof(WCHAR), 0);

Exit:
    SAFEDELETEARRAY(wzDynamicBase);
    SAFEDELETEARRAY(wzAppName);
    SAFEDELETEARRAY(wzAppCtxDynamicDir);

    return hr;
}

STDMETHODIMP CApplicationContext::GetAppCacheDirectory(
                    __out_ecount_opt(*pdwSize) LPWSTR wzCacheDir, 
                    __inout DWORD *pdwSize)
{
    HRESULT                                 hr = S_OK;
    LPWSTR                                  wzCacheBase = NULL;
    LPWSTR                                  wzAppName = NULL;
    LPWSTR                                  wzAppCtxCacheDir = NULL;
    WCHAR                                   wzDir[MAX_PATH];
    DWORD                                   dwLen;

    wzDir[0] = L'\0';

    if (!pdwSize) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Check if the cache directory has already been set.

    hr = ::AppCtxGetWrapper(this, ACTAG_APP_CACHE_DIRECTORY, &wzAppCtxCacheDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (wzAppCtxCacheDir) {
        _ASSERTE(lstrlenW(wzAppCtxCacheDir));

        dwLen = lstrlenW(wzAppCtxCacheDir) + 1;
        if (!wzCacheDir || *pdwSize < dwLen) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            *pdwSize = dwLen;
            goto Exit;
        }

        hr = StringCchCopy(wzCacheDir, *pdwSize, wzAppCtxCacheDir);
        if (FAILED(hr)) {
            goto Exit;
        }

        *pdwSize = dwLen;

        goto Exit;
    }


    // Always recalculate cache directory, so it can be changed on-demand

    hr = ::AppCtxGetWrapper(this, ACTAG_APP_CACHE_BASE, &wzCacheBase);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ::AppCtxGetWrapper(this, ACTAG_APP_NAME, &wzAppName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!wzAppName || !wzCacheBase) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

    PathRemoveBackslashW(wzCacheBase);

    hr = StringCchPrintf(wzDir, MAX_PATH, L"%ws\\%ws", wzCacheBase, wzAppName);
    if (FAILED(hr)) {
        goto Exit;
    }
    dwLen = lstrlenW(wzDir) + 1;

    if (!wzCacheDir || *pdwSize < dwLen) {
        *pdwSize = dwLen;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    hr = StringCchCopy(wzCacheDir, *pdwSize, wzDir);
    if (FAILED(hr)) {
        goto Exit;
    }

    *pdwSize = dwLen;

    // Cache this for future use.

    Set(ACTAG_APP_CACHE_DIRECTORY, wzCacheDir, dwLen * sizeof(WCHAR), 0);

Exit:
    SAFEDELETEARRAY(wzCacheBase);
    SAFEDELETEARRAY(wzAppName);
    SAFEDELETEARRAY(wzAppCtxCacheDir);

    return hr;
}

//
// RegisterKnownAssembly
//
// Params:
//
// [in]  pName      : IAssemblyName describing the known assembly
// [in]  pwzAsmURL  : Full URL to assembly described by pName
// [out] ppAsmOut   : Output IAssembly to be passed to BindToObject
//

STDMETHODIMP CApplicationContext::RegisterKnownAssembly(IAssemblyName *pName,
                                                        LPCWSTR pwzAsmURL,
                                                        IAssembly **ppAsmOut)
{
    // not used anymore
    return E_NOTIMPL;
}

//
// PrefetchAppConfigFile
//

STDMETHODIMP CApplicationContext::PrefetchAppConfigFile()
{
    _ASSERTE(!"Not implemented!");
    return E_NOTIMPL;
}

HRESULT CApplicationContext::GetAssemblyBindingClosure(
                            IUnknown *pUnk,
                            IAssemblyBindingClosure **ppAsmClosure)
{
    return E_NOTIMPL;
}


HRESULT CApplicationContext::Lock()
{
    HRESULT                                      hr = S_OK;

    ClrEnterCriticalSection(_cs);

    return hr;
}

HRESULT CApplicationContext::Unlock()
{
    ClrLeaveCriticalSection(_cs);

    return S_OK;
}

HRESULT CApplicationContext::ReadConfigSettings(CDebugLog *pdbglog, CAppCtxPolicyConfigs **ppConfig)
{
    HRESULT                                  hr = S_OK;
    CAppCtxPolicyConfigs                    *pConfig = NULL;

    if (_bReadConfigSettings) {
        _ASSERTE(_pConfig);
        if (ppConfig) {
            *ppConfig = _pConfig;
            (*ppConfig)->AddRef();
        }
        return S_OK;
    }

    hr = Lock();
    if (FAILED(hr)) {
        return hr;
    }

    if (_bReadConfigSettings) {
        _ASSERTE(_pConfig);
        if (ppConfig) {
            *ppConfig = _pConfig;
            (*ppConfig)->AddRef();
        }
        Unlock();
        return S_OK;
    }

    hr = CAppCtxPolicyConfigs::Create(&pConfig);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = ReadConfigSettingsInternal(pdbglog, pConfig);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (ppConfig) {
        *ppConfig = pConfig;
        (*ppConfig)->AddRef();
    }

    // save the result internally
    _ASSERTE(!_pConfig);
    _pConfig = pConfig;
    _pConfig->AddRef();

    _bReadConfigSettings = TRUE;

    hr = S_OK;

Exit:

    Unlock();

    SAFERELEASE(pConfig);

    return hr;
}

HRESULT CApplicationContext::ReadConfigSettingsInternal(CDebugLog *pdbglog, CAppCtxPolicyConfigs *pAppCtxConfig)
{
    HRESULT hr = S_OK;
    LPWSTR  pwzAppCfg = NULL;
    LPWSTR  pwzPrivatePath = NULL;
    LPWSTR  pwzHostCfg = NULL;
    LPWSTR  pwzMachineCfg = NULL;
    CPolicyConfig *pConfig = NULL;
    BOOL    bHasFxRedirect = FALSE;
    BYTE    *pbAppConfigBlob = NULL;
    DWORD   cbAppConfigBlob = 0;
    

    _ASSERTE(pAppCtxConfig);

    
    hr = Get(ACTAG_APP_CONFIG_BLOB, pbAppConfigBlob, &cbAppConfigBlob, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        pbAppConfigBlob = NEW(BYTE[cbAppConfigBlob]);
        if (!pbAppConfigBlob) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = Get(ACTAG_APP_CONFIG_BLOB, pbAppConfigBlob, &cbAppConfigBlob, 0);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CPolicyConfig::Create(pbAppConfigBlob, cbAppConfigBlob, ePolicyLevelApp, pdbglog, &pConfig);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = ::AppCtxGetWrapper(this, ACTAG_APP_CFG_LOCAL_FILEPATH, &pwzAppCfg);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (pwzAppCfg && pwzAppCfg[0]) {
            hr = CPolicyConfig::Create(pwzAppCfg, ePolicyLevelApp, pdbglog, &pConfig);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }
    
    if (pConfig) {
        hr = pConfig->GetPrivatePath(&pwzPrivatePath);
        if (FAILED(hr)) {
            goto Exit;
        }

        pAppCtxConfig->SetPolicyConfig(ePolicyLevelApp, pConfig);

        hr = pConfig->HasFrameworkRedirect(&bHasFxRedirect);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    SAFERELEASE(pConfig);

    hr = ::AppCtxGetWrapper(this, ACTAG_HOST_CONFIG_FILE, &pwzHostCfg);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (pwzHostCfg && pwzHostCfg[0]) {
        hr = CPolicyConfig::Create(pwzHostCfg, ePolicyLevelHost, pdbglog, &pConfig);
        if (SUCCEEDED(hr)) {
            pAppCtxConfig->SetPolicyConfig(ePolicyLevelHost, pConfig);
            if (!bHasFxRedirect) {
                hr = pConfig->HasFrameworkRedirect(&bHasFxRedirect);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
        }
        else if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) && hr != HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
            goto Exit;
        }
        else {
            DEBUGOUT1(pdbglog, 0, ID_FUSLOG_CFG_NOT_EXIST, pwzHostCfg);
        }
       
        hr = S_OK; 
    }

    SAFERELEASE(pConfig);

    hr = ::AppCtxGetWrapper(this, ACTAG_MACHINE_CONFIG, &pwzMachineCfg);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pwzMachineCfg && pwzMachineCfg[0]) {
        hr = CPolicyConfig::Create(pwzMachineCfg, ePolicyLevelAdmin, pdbglog, &pConfig);
        if (SUCCEEDED(hr)) {
            pAppCtxConfig->SetPolicyConfig(ePolicyLevelAdmin, pConfig);
            if (!bHasFxRedirect) {
                hr = pConfig->HasFrameworkRedirect(&bHasFxRedirect);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
        }
        else if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) && hr != HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
            goto Exit;
        }
        else {
            DEBUGOUT1(pdbglog, 0, ID_FUSLOG_CFG_NOT_EXIST, pwzMachineCfg);
        }
       
        hr = S_OK; 
    }

    SAFERELEASE(pConfig);

    if (SUCCEEDED(hr)) {
        if (pwzPrivatePath) {
            hr = Set(ACTAG_APP_CFG_PRIVATE_BINPATH, pwzPrivatePath,
                    (lstrlenW(pwzPrivatePath) + 1) * sizeof(WCHAR), 0);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            DEBUGOUT1(pdbglog, 0, ID_FUSLOG_CFG_PRIVATE_PATH, pwzPrivatePath);
        }

        pAppCtxConfig->SetHasFrameworkRedirect(bHasFxRedirect);
    }

Exit:

    SAFERELEASE(pConfig);
    SAFEDELETEARRAY(pwzAppCfg);
    SAFEDELETEARRAY(pwzHostCfg);
    SAFEDELETEARRAY(pwzMachineCfg);
    SAFEDELETEARRAY(pwzPrivatePath);
    SAFEDELETEARRAY(pbAppConfigBlob);

    return hr;
}

HRESULT CApplicationContext::GetBindingResultCache(CBindingResultCache **ppBindResult)
{
    HRESULT hr = S_OK;
    CBindingResultCache *pCache = NULL;

    if (!_pBindingResultCache) {
        hr = CBindingResultCache::Create(&pCache);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (InterlockedCompareExchangePointer((void **)&_pBindingResultCache, pCache, NULL)) {
            SAFERELEASE(pCache);
        }
    }

    *ppBindResult = _pBindingResultCache;
    (*ppBindResult)->AddRef();

Exit:
    return hr;
}

HRESULT CApplicationContext::GetLoadContext(CLoadContext **ppLoadContext, LOADCTX_TYPE type)
{
    HRESULT hr = S_OK;
    CLoadContext *pCtx = NULL;
    CLoadContext **ppInternalCtx = (type == LOADCTX_TYPE_DEFAULT)? &_pDefaultCtx:&_pLoadFromCtx;

    _ASSERTE(type == LOADCTX_TYPE_DEFAULT || type == LOADCTX_TYPE_LOADFROM);

    if (!(*ppInternalCtx)) {
        hr = CLoadContext::Create(&pCtx, type);
        if (FAILED(hr)) {
            goto Exit;
        }
        if (InterlockedCompareExchangePointer((void **)ppInternalCtx, pCtx, NULL)) {
            SAFERELEASE(pCtx);
        }
    }

    *ppLoadContext = *ppInternalCtx;
    (*ppLoadContext)->AddRef();

Exit:
    return hr;
}

