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
#include <winerror.h>
#include <shlwapi.h>
#include "fusionp.h"
#include "naming.h"
#include "asmimprt.h"
#include "imprthelpers.h"
#include "modimprt.h"
#include "policy.h"
#include "fusionheap.h"
#include "lock.h"
#include "cacheutils.h"
#include "memoryreport.h"

// ---------------------------------------------------------------------------
// CreateAssemblyManifestImport
// ---------------------------------------------------------------------------
HRESULT
CreateAssemblyManifestImport(
    LPCTSTR szManifestFilePath,
    LPASSEMBLY_MANIFEST_IMPORT *ppImport)
{
    HRESULT hr = S_OK;
    CAssemblyManifestImport *pImport = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyManifestImport");

    IfNullGo(pImport = NEW(CAssemblyManifestImport));
    IfFailGo(pImport->Init(szManifestFilePath));

    *ppImport = pImport;
    
ErrExit:
    if (FAILED(hr))
        SAFERELEASE(pImport);

    return hr;
}

HRESULT
CreateAssemblyManifestImport(
    LPCTSTR szManifestFilePath,
    IMetaDataAssemblyImport *pMDImport,
    LPASSEMBLY_MANIFEST_IMPORT *ppImport)
{
    HRESULT hr = S_OK;
    CAssemblyManifestImport *pImport = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyManifestImport");

    IfNullGo(pImport = NEW(CAssemblyManifestImport));
    IfFailGo(pImport->Init(szManifestFilePath, pMDImport));

    *ppImport = pImport;

ErrExit:
    if (FAILED(hr))
        SAFERELEASE(pImport);

    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyManifestImport constructor
// ---------------------------------------------------------------------------
CAssemblyManifestImport::CAssemblyManifestImport()
{
    _dwSig                  = 0x494e414d; /* 'INAM' */
    _pName                  = NULL;
    _pMDImport              = NULL;
    _pszManifestFilePath    = NULL;
    _cRef                   = 1;
    _ppModules              = NULL;
    _dwNumModules           = 0;
    _dwCacheType            = CACHE_INVALID;
    _isDll                  = FALSE;
    _rAssemblyRefTokens     = NULL;
    _cAssemblyRefTokens     = (DWORD)(-1);  // -1 means refTokens are not initialized. 
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport destructor
// ---------------------------------------------------------------------------
CAssemblyManifestImport::~CAssemblyManifestImport()
{
    SAFERELEASE(_pName);
    SAFERELEASE(_pMDImport);
    SAFEDELETEARRAY(_pszManifestFilePath);
    SAFEDELETEARRAY(_rAssemblyRefTokens);

    if (_dwNumModules && _ppModules) {
        for(DWORD i=0; i<_dwNumModules; i++) {
            SAFERELEASE(_ppModules[i]);
        }
    
        SAFEDELETEARRAY(_ppModules);
    }
}

// ---------------------------------------------------------------------------
// CAssembly::Init
// ---------------------------------------------------------------------------
HRESULT CAssemblyManifestImport::Init(LPCTSTR szManifestFilePath)
{
    HRESULT hr = S_OK;
    IMetaDataAssemblyImport                     *pMDImport = NULL;

    if (!szManifestFilePath) {
        hr = E_INVALIDARG;
        goto ErrExit;
    }

    // Make sure the path isn't relative
    _ASSERTE(!PathIsRelative(szManifestFilePath));

    IfFailGo(InternalSetManifestModulePath(szManifestFilePath));
    
    IfFailGo(CreateMetaDataImport(szManifestFilePath, &pMDImport));

    _pMDImport = pMDImport;
    _pMDImport->AddRef();

    IfFailGo(CopyMetaData());

ErrExit:
    SAFERELEASE(pMDImport);
    return hr;
}

HRESULT CAssemblyManifestImport::Init(LPCTSTR szManifestFilePath, IMetaDataAssemblyImport *pMDImport)
{
    HRESULT hr = S_OK;

    if (!szManifestFilePath || !pMDImport) {
        hr = E_INVALIDARG;
        goto ErrExit;
    }

    // Make sure the path isn't relative
    _ASSERTE(!PathIsRelative(szManifestFilePath));

    _pMDImport = pMDImport;
    _pMDImport->AddRef();

    IfFailGo(InternalSetManifestModulePath(szManifestFilePath));

    IfFailGo(CopyMetaData());

ErrExit:
    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyManifestImport::SetManifestModulePath
// ---------------------------------------------------------------------------
HRESULT CAssemblyManifestImport::SetManifestModulePath(LPCWSTR pszModulePath)
{
    HRESULT hr = S_OK;

    if (!pszModulePath) {
        hr = E_INVALIDARG;
        goto ErrExit;
    }

    if (PathIsRelative(pszModulePath)) {
        hr = E_INVALIDARG;
        goto ErrExit;
    }

    hr = InternalSetManifestModulePath(pszModulePath);

ErrExit:
    return hr;
}

HRESULT CAssemblyManifestImport::InternalSetManifestModulePath(LPCWSTR pszModulePath)
{
    HRESULT hr = S_OK;
    DWORD   dwSize;
    LPWSTR pwzExtension = NULL;

    _ASSERTE(pszModulePath);

    dwSize = lstrlenW(pszModulePath) + 1;
    if (dwSize > MAX_PATH) {
        hr = COR_E_PATHTOOLONG;
        goto ErrExit;
    }

    SAFEDELETEARRAY(_pszManifestFilePath);
    
    IfNullGo(_pszManifestFilePath = NEW(TCHAR[dwSize]));

    memcpy(_pszManifestFilePath, pszModulePath, dwSize * sizeof(WCHAR));

    // Always store the extension as lower case.
    pwzExtension = PathFindExtension(_pszManifestFilePath);
    if (pwzExtension && *pwzExtension) {
        if (!FusionCompareStringI(pwzExtension, L".DLL")) {
            memcpy(pwzExtension, L".dll", sizeof(L".dll"));
        }
        else if (!FusionCompareStringI(pwzExtension, L".EXE")) {
            memcpy(pwzExtension, L".exe", sizeof(L".exe"));
        }
    }

ErrExit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::GetAssemblyNameDef
// ---------------------------------------------------------------------------

STDMETHODIMP CAssemblyManifestImport::GetAssemblyNameDef(LPASSEMBLYNAME *ppName)
{
    HRESULT                                       hr = S_OK;

    if (!ppName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppName = NULL;

    _ASSERTE(_pName);

    *ppName = _pName;
    if (*ppName)
        (*ppName)->AddRef();

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::GetNextAssemblyNameRef
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyManifestImport::GetNextAssemblyNameRef(DWORD nIndex, LPASSEMBLYNAME *ppName)
{
    HRESULT     hr = S_OK;
    WCHAR       wzBuf[MAX_PATH];
    DWORD       dwSize = MAX_PATH;
    mdAssembly  *rAssemblyRefTokens = NULL;
    DWORD        cAssemblyRefTokens = 0;
    mdAssemblyRef    mdmar;

    if (!_pMDImport) {
        IfFailGo(GetManifestModulePath(wzBuf, &dwSize));

        IfFailGo(CreateMetaDataImport(wzBuf, &_pMDImport));
    }

    if (_cAssemblyRefTokens == (DWORD)(-1)) {
        IfFailGo(GetAssemblyRefTokens(_pMDImport, &rAssemblyRefTokens, &cAssemblyRefTokens));

        if (InterlockedCompareExchangePointer((void **)&_rAssemblyRefTokens, rAssemblyRefTokens, NULL)) {
            SAFEDELETEARRAY(rAssemblyRefTokens);
        }

        rAssemblyRefTokens = NULL;

        _cAssemblyRefTokens = cAssemblyRefTokens;
    } 

    _ASSERTE(_cAssemblyRefTokens != (DWORD)(-1));

    // Verify the index passed in. 
    if (nIndex >= _cAssemblyRefTokens)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto ErrExit;
    }

    // Reference indexed dep assembly ref token.
    mdmar = _rAssemblyRefTokens[nIndex];
    hr = GetAssemblyNameRefFromMDImport(_pMDImport, mdmar, ASM_IMPORT_NAME_HASH_VALUE, ppName);
        
ErrExit:        
    if (FAILED(hr)) {
        SAFEDELETEARRAY(rAssemblyRefTokens);
    }
    return hr;
}

STDMETHODIMP CAssemblyManifestImport::GetNextAssemblyModule(DWORD nIndex,
                                                            IAssemblyModuleImport **ppImport)
{
    HRESULT                                    hr = S_OK;

    if (nIndex >= _dwNumModules) {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto Exit;
    }

    *ppImport = _ppModules[nIndex];
    (*ppImport)->AddRef();

Exit:
    return hr;
}


HRESULT CAssemblyManifestImport::CopyMetaData()
{
    HRESULT                                      hr = S_OK;

    _ASSERTE(_pMDImport);

    IfFailGo(CopyNameDef(_pMDImport));
    IfFailGo(CopyModuleRefs(_pMDImport));

ErrExit:
    return hr;
}

HRESULT CAssemblyManifestImport::CopyModuleRefs(IMetaDataAssemblyImport *pMDImport)
{
    HRESULT                                      hr = S_OK;
    HCORENUM                                     hEnum = 0;
    mdFile                                       mdf;
    TCHAR                                        szModuleName[MAX_PATH];
    DWORD                                        ccModuleName = MAX_PATH;
    const VOID                                  *pvHashValue = NULL;
    DWORD                                        cbHashValue = 0;
    DWORD                                        ccPath = 0;
    DWORD                                        dwFlags = 0;
    TCHAR                                       *pszName = NULL;
    mdFile                                       rAssemblyModuleTokens[ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE];
    DWORD                                        cAssemblyModuleTokens = ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE;
    DWORD                                        i;
    IAssemblyModuleImport                       *pImport = NULL;
    LISTNODE                                     pos;
    List<IAssemblyModuleImport *>                listModules;
    WCHAR   wzBuf[MAX_PATH];
    DWORD   dwSize = MAX_PATH;

    _ASSERTE(pMDImport);
    _ASSERTE(_pName);
    
    // Form module file path from manifest path and module name.
    IfFailGo(GetManifestModulePath(wzBuf, &dwSize));

    pszName = PathFindFileName(wzBuf);

    ccPath = (DWORD)(pszName - wzBuf);
    _ASSERTE(ccPath < MAX_PATH);

    while (cAssemblyModuleTokens > 0) {
        IfFailGo(pMDImport->EnumFiles(&hEnum, rAssemblyModuleTokens,
                                  ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE,
                                  &cAssemblyModuleTokens));
    
        for (i = 0; i < cAssemblyModuleTokens; i++) {
            mdf = rAssemblyModuleTokens[i];
        
            hr = pMDImport->GetFileProps(
                mdf,            // [IN] The File for which to get the properties.
                szModuleName,   // [OUT] Buffer to fill with name.
                MAX_PATH,       // [IN] Size of buffer in wide chars.
                &ccModuleName,  // [OUT] Actual # of wide chars in name.
                &pvHashValue,   // [OUT] Pointer to the Hash Value Blob.
                &cbHashValue,   // [OUT] Count of bytes in the Hash Value Blob.
                &dwFlags);      // [OUT] Flags.
            
            if (FAILED(hr)) {
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = FUSION_E_INVALID_NAME;
                }
                goto ErrExit;
            }

            if (hr == CLDB_S_TRUNCATION) {
                // Cannot have a name greater than MAX_PATH
                hr = FUSION_E_ASM_MODULE_MISSING;
                goto ErrExit;
            }

            if (ccPath + ccModuleName >= MAX_PATH) {
                hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                goto ErrExit;
            }
        
            IfFailGo(CreateAssemblyModuleImport(szModuleName, (LPBYTE)pvHashValue, cbHashValue,
                                            dwFlags, _pName, this, &pImport));
    
            if (!listModules.AddTail(pImport))
            {
                hr = E_OUTOFMEMORY;
                SAFERELEASE(pImport);
                goto ErrExit;
            }

            pImport = NULL;
        }
    }

    hr = S_OK;

    _dwNumModules = listModules.GetCount();
    if (_dwNumModules) {
        _ppModules = NEW(IAssemblyModuleImport *[_dwNumModules]);
        if (!_ppModules) {
            _dwNumModules = 0;
            hr = E_OUTOFMEMORY;
            goto ErrExit;
        }

        memset(_ppModules, 0, sizeof(IAssemblyModuleImport *) * _dwNumModules);

        pos = listModules.GetHeadPosition();
        i = 0;
        while(pos && (i < _dwNumModules)) {
            _ppModules[i] = listModules.GetNext(pos);
            _ppModules[i]->AddRef();
            i++;
        }
    }

ErrExit:
    if (hEnum) {
        pMDImport->CloseEnum(hEnum);
    }

    pos = listModules.GetHeadPosition();
    while(pos) {
        pImport = listModules.GetNext(pos);
        SAFERELEASE(pImport);
    }
    listModules.RemoveAll();

    return hr;
}

HRESULT CAssemblyManifestImport::CopyNameDef(IMetaDataAssemblyImport *pMDImport)
{
    DWORD   dwFlags = ASM_IMPORT_NAME_HASH_ALGID |
                      ASM_IMPORT_NAME_SIGNATURE_BLOB |
                      ASM_IMPORT_NAME_MVID |
                      ASM_IMPORT_NAME_ARCHITECTURE |
                      ASM_IMPORT_NAME_PE_RUNTIME;

    return GetAssemblyNameDefFromMDImport(pMDImport, dwFlags, FALSE, &_pName);
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::GetModuleByName
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyManifestImport::GetModuleByName(LPCOLESTR pszModuleName, 
    IAssemblyModuleImport **ppImport)
{
    HRESULT                hr;
    LPTSTR                 pszName;
    DWORD                  dwIdx      = 0;
    IAssemblyModuleImport *pModImport = NULL;
    WCHAR   wzBuf[MAX_PATH];
    DWORD   dwSize = MAX_PATH;

    if (!ppImport) {
        return E_INVALIDARG;
    }
    
    // NULL indicated name means get manifest module.
    if (!pszModuleName) {
        // Parse manifest module name from file path.
        hr =GetManifestModulePath(wzBuf, &dwSize);
        if (FAILED(hr)) {
            goto exit;
        }
        pszName = PathFindFileName(wzBuf);
    }
    // Otherwise get named module.
    else {
        pszName = (LPTSTR) pszModuleName;
    }

    // Enumerate the modules in this manifest.
    while (SUCCEEDED(hr = GetNextAssemblyModule(dwIdx++, &pModImport)))
    {
        TCHAR szModName[MAX_PATH];
        DWORD ccModName;
        ccModName = MAX_PATH;
        if (FAILED(hr = pModImport->GetModuleName(szModName, &ccModName)))
            goto exit;
            
        // Compare module name against given.
        if (!FusionCompareStringI(szModName, pszName))
        {
            // Found the module
            break;
        }   
        SAFERELEASE(pModImport);
    }

    if (SUCCEEDED(hr)) {
        *ppImport = pModImport;
        (*ppImport)->AddRef();
    }

exit:
    SAFERELEASE(pModImport);
    return hr;    
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::GetManifestModulePath
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyManifestImport::GetManifestModulePath(__out_ecount_opt(*pccModulePath) LPOLESTR pszModulePath,
    LPDWORD pccModulePath)
{
    HRESULT hr = S_OK;
    DWORD dwSize = 0;

    if (_pszManifestFilePath) {
        dwSize = lstrlenW(_pszManifestFilePath)+1;
        
        if (*pccModulePath < dwSize)
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

        if (SUCCEEDED(hr))
            memcpy(pszModulePath, _pszManifestFilePath, dwSize * sizeof(TCHAR));

        *pccModulePath = dwSize;
    }
    else {
        _ASSERTE(IsGacType(_dwCacheType) || IsZapType(_dwCacheType));
        _ASSERTE(_pName);
        
        hr = GetManifestFilePathFromName(_pName, _isDll, _dwCacheType, pszModulePath, pccModulePath);
    }

    return hr;
}


// IUnknown methods

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::AddRef
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyManifestImport::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyManifestImport::Release()
{
    ULONG ulRef = 0;
   
    // only references left are this and all its modules
    if (_cRef == (LONG)_dwNumModules+1) {
        // add additional reference so that the same logic won't kick in.
        _cRef += _dwNumModules;
        if (_dwNumModules && _ppModules) {
            for(DWORD i=0; i<_dwNumModules; i++) {
                SAFERELEASE(_ppModules[i]);
            }
        
            SAFEDELETEARRAY(_ppModules);
        }
        _cRef -= _dwNumModules;
        _dwNumModules = 0;
    }
    
    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }

    return ulRef;
}

// ---------------------------------------------------------------------------
// CAssemblyManifestImport::QueryInterface
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyManifestImport::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT                                  hr = S_OK;

    if (!ppv) 
        return E_POINTER;
    
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IAssemblyManifestImport) || IsEqualIID(riid, IID_IUnknown)) {
        *ppv = (IAssemblyManifestImport *)this;
    }
    else {
        hr = E_NOINTERFACE;
    }

    if (*ppv) {
        AddRef();
    }

    return hr;
} 
