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
#include "modimprt.h"
#include "asm.h"
#include "asmcache.h"
#include "dbglog.h"
#include "actasm.h"
#include "lock.h"
#include "scavenger.h"
#include "cacheutils.h"
#include "memoryreport.h"

extern CRITSEC_COOKIE g_csInitClb;

// ---------------------------------------------------------------------------
// CreateAssemblyFromTransCacheEntry
// ---------------------------------------------------------------------------
STDAPI
CreateAssemblyFromTransCacheEntry(CTransCache *pTransCache, 
    IAssemblyManifestImport *pImport, IAssembly **ppAssembly)
{   
    HRESULT hr = S_OK;
    LPWSTR  szManifestFilePath=NULL, szCodebase;
    FILETIME *pftCodebase;
    CAssembly *pAsm                    = NULL;
    CAssemblyManifestImport *pCImport = NULL;
    BOOL fCreated = FALSE;

    _ASSERTE(pTransCache);
    TRANSCACHEINFO *pTCInfo = (TRANSCACHEINFO*) pTransCache->_pInfo;

    szManifestFilePath = pTCInfo->pwzPath;

    szCodebase = pTCInfo->pwzCodebaseURL;
    pftCodebase = &(pTCInfo->ftLastModified);

    _ASSERTE(szManifestFilePath && ppAssembly);

    pAsm = NEW(CAssembly);
    if (!pAsm)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    // If a manifest import interface is not provided,
    // create one from the file path.
    if (!pImport)
    {
        if (FAILED(hr = CreateAssemblyManifestImport((LPTSTR)szManifestFilePath,
                                                    &pImport))) {
            goto exit;
        }

        fCreated = TRUE;
    }
    // Otherwise if one is passed in, revise the manifest path
    // to match that held by the transcache entry.
    else
    {
        pCImport = static_cast<CAssemblyManifestImport*>(pImport); // dynamic_cast
        _ASSERTE(pCImport);
        pCImport->SetManifestModulePath(szManifestFilePath);
    }
    
    if (IsGacType(pTransCache->GetCacheType())) {
        hr = pAsm->Init(pImport, pTransCache, NULL, NULL);
    }
    else {
        hr = pAsm->Init(pImport, pTransCache, szCodebase, pftCodebase);
    }

    if (FAILED(hr)) 
    {        
        goto exit;
    }

    *ppAssembly = pAsm;
    (*ppAssembly)->AddRef();

exit:

    if (fCreated)
        SAFERELEASE(pImport);

    SAFERELEASE(pAsm);
    
    return hr;
}

// ---------------------------------------------------------------------------
// CreateAssemblyFromManifestFile
// ---------------------------------------------------------------------------
STDAPI
CreateAssemblyFromManifestFile(
    LPCOLESTR   szManifestFilePath,
    LPCOLESTR   szCodebase,
    FILETIME   *pftCodebase,
    IAssembly  **ppAssembly)
{
    HRESULT hr = S_OK;
    LPASSEMBLY_MANIFEST_IMPORT pImport = NULL;
    CAssembly *pAsm                    = NULL;

    _ASSERTE(szManifestFilePath && ppAssembly);

    *ppAssembly = NULL;

    pAsm = NEW(CAssembly);
    if (!pAsm)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    if (FAILED(hr = CreateAssemblyManifestImport((LPTSTR)szManifestFilePath, &pImport))) {
        goto exit;
    }
    
    hr = pAsm->Init(pImport, NULL, szCodebase, pftCodebase);

    if (FAILED(hr)) {
        goto exit;
    }
    
    *ppAssembly = pAsm;
    (*ppAssembly)->AddRef();
    
exit:

    SAFERELEASE(pImport);
    SAFERELEASE(pAsm);

    return hr;
}

// ---------------------------------------------------------------------------
// CreateAssemblyFromManifestImport
// ---------------------------------------------------------------------------
STDAPI CreateAssemblyFromManifestImport(IAssemblyManifestImport *pImport,
                                        LPCOLESTR szCodebase, FILETIME *pftCodebase,
                                        IAssembly **ppAssembly)
{
    HRESULT                                   hr = S_OK;
    CAssembly                                *pAsm = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssembly");

    _ASSERTE(pImport && ppAssembly);

    *ppAssembly = NULL;

    pAsm = NEW(CAssembly);
    if (!pAsm) {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = pAsm->Init(pImport, NULL, szCodebase, pftCodebase);
    if (FAILED(hr)) {
        SAFERELEASE(pAsm);
        goto exit;
    }
    
    *ppAssembly = pAsm;
    (*ppAssembly)->AddRef();
    
exit:
    SAFERELEASE(pAsm);

    return hr;
}

// ---------------------------------------------------------------------------
// CreateAssemblyFromCacheLookup
// ---------------------------------------------------------------------------
HRESULT CreateAssemblyFromCacheLookup(IApplicationContext *pAppCtx,
                                      IAssemblyName *pNameRef,
                                      IAssembly **ppAsm, CDebugLog *pdbglog)
{
    HRESULT         hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    DWORD           dwSize = 0;
    PEKIND          pe = peNone;
    BOOL            bHasPE = FALSE;
    CACHE_FLAGS     dwCacheType;

    _ASSERTE(pNameRef && ppAsm);
    _ASSERTE(CAssemblyName::IsStronglyNamed(pNameRef));
    _ASSERTE(!(CAssemblyName::IsCustom(pNameRef)));

    _ASSERTE(CAssemblyName::IsStronglyNamed(pNameRef));

    *ppAsm = NULL;
   
    dwSize = sizeof(pe);
    hr = pNameRef->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&pe, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pe != peNone) {
        bHasPE = TRUE;
        MapAssemblyArchitectureToCacheType(pe, &dwCacheType);
    }

    if (bHasPE) {
        hr = CheckExistsInGAC(pNameRef, dwCacheType, NULL, NULL, ppAsm);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        // PA unknown. Need to try all of them.
        if (IsProcess32()) {
            dwCacheType = CACHE_GAC_32;
        }
        else {
            dwCacheType = CACHE_GAC_64;
        }

        hr = CheckExistsInGAC(pNameRef, dwCacheType, NULL, NULL, ppAsm);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_FALSE) {
            dwCacheType = CACHE_GAC_MSIL;
            hr = CheckExistsInGAC(pNameRef, dwCacheType, NULL, NULL, ppAsm);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        if (hr == S_FALSE) {
            dwCacheType = CACHE_GAC;
            hr = CheckExistsInGAC(pNameRef, dwCacheType, NULL, NULL, ppAsm);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    if (hr == S_FALSE){
        DEBUGOUT(pdbglog, 1, ID_FUSLOG_ASSEMBLY_LOOKUP_FAILURE);
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

Exit:
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
        DEBUGOUT(pdbglog, 1, ID_FUSLOG_ASSEMBLY_CREATION_FAILURE);
    }

    if (hr == S_OK) {
        _ASSERTE(*ppAsm);
        CAssembly *pCAsm = static_cast<CAssembly *>(*ppAsm);
        pCAsm->SetApplicationContextID(pAppCtx);
        pCAsm->SetAssemblyLocation(ASMLOC_GAC);
    }

    return hr;
}


// ---------------------------------------------------------------------------
// CAssembly constructor
// ---------------------------------------------------------------------------
CAssembly::CAssembly()
{
    _dwSig  = ASSEMBLY_CLASS_SIGNATURE;
    _pImport = NULL;
    _pName   = NULL;
    _dwAsmLoc = ASMLOC_UNKNOWN;
    _isSystem = FALSE;
    _pLoadContext = NULL;
    _pwzProbingBase = NULL;
    _cRef = 1;
    _hFile  = INVALID_HANDLE_VALUE;
    _pAppCtx = NULL;
}

// ---------------------------------------------------------------------------
// CAssembly destructor
// ---------------------------------------------------------------------------
CAssembly::~CAssembly()
{
    SAFERELEASE(_pLoadContext);
    SAFERELEASE(_pImport);
    SAFERELEASE(_pName);

    if(_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(_hFile);

    SAFEDELETEARRAY(_pwzProbingBase);
}

// ---------------------------------------------------------------------------
// CAssembly::Init
// ---------------------------------------------------------------------------
HRESULT CAssembly::Init(LPASSEMBLY_MANIFEST_IMPORT pImport, 
    CTransCache *pTransCache, LPCOLESTR szCodebase, FILETIME *pftCodebase)
{
    HRESULT  hr = S_OK;

    _ASSERTE(pImport);

    _pImport = pImport;
    _pImport->AddRef();

    hr = pImport->GetAssemblyNameDef(&_pName);
    if (FAILED(hr)) {
        goto exit;
    }

    // set url and last modified on name def if present.
    if (szCodebase)
    {
        hr = _pName->SetProperty(ASM_NAME_CODEBASE_URL, (void *)szCodebase, (lstrlenW(szCodebase) + 1) * sizeof(WCHAR));
        if (FAILED(hr)) {
            goto exit;
        }
    }

    if (pftCodebase) {
        hr = _pName->SetProperty(ASM_NAME_CODEBASE_LASTMOD, (void *)pftCodebase, sizeof(FILETIME));
        if (FAILED(hr)) {
            goto exit;
        }
    }

exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssembly::SetFileHandle
// ---------------------------------------------------------------------------
void CAssembly::SetFileHandle(HANDLE hFile)
{
    _hFile = hFile;
}

// ---------------------------------------------------------------------------
// CAssembly::GetAssemblyNameDef
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssembly::GetAssemblyNameDef(LPASSEMBLYNAME *ppName)
{
    HRESULT hr = S_OK;
    _ASSERTE(_pName);

    if (!ppName) {
        return E_INVALIDARG;
    }

    *ppName = _pName;
    if (*ppName) {
        (*ppName)->AddRef();
    }
    
    return hr;
}

// ---------------------------------------------------------------------------
// CAssembly::GetNextAssemblyNameRef
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssembly::GetNextAssemblyNameRef(DWORD nIndex, LPASSEMBLYNAME *ppName)
{
    if (!ppName) {
        return E_INVALIDARG;
    }

    return _pImport->GetNextAssemblyNameRef(nIndex, ppName);
}

// ---------------------------------------------------------------------------
// CAssembly::GetNextAssemblyModule
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssembly::GetNextAssemblyModule(DWORD nIndex, LPASSEMBLY_MODULE_IMPORT *ppModImport)
{
    HRESULT hr = S_OK;

    if (!ppModImport) {
        return E_INVALIDARG;
    }

    // Get the ith module import interface.
    hr = _pImport->GetNextAssemblyModule(nIndex, ppModImport);
        
    return hr;
}

// ---------------------------------------------------------------------------
// CAssembly::GetModuleByName
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssembly::GetModuleByName(LPCOLESTR pszModuleName, LPASSEMBLY_MODULE_IMPORT *ppModImport)
{
    HRESULT                                        hr = S_OK;

    if (!pszModuleName || !ppModImport) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = _pImport->GetModuleByName(pszModuleName, ppModImport);

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssembly::GetManifestModulePath
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssembly::GetManifestModulePath(
        __out_ecount_opt(*pccModulePath)LPOLESTR pszModulePath, 
        __inout LPDWORD pccModulePath)
{
    HRESULT                                   hr = S_OK;

    if (!pccModulePath) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = _pImport->GetManifestModulePath(pszModulePath, pccModulePath);

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssembly::GetAssemblyPath
// ---------------------------------------------------------------------------
HRESULT CAssembly::GetAssemblyPath(
        __out_ecount_opt(*lpcwBuffer)LPOLESTR pStr, 
        __inout LPDWORD lpcwBuffer)
{
    HRESULT                           hr = S_OK;
    WCHAR                             wzBuf[MAX_PATH + 1];
    DWORD                             dwBuf = MAX_PATH + 1;
    DWORD                             dwLen = 0;
    LPWSTR                            pwzTmp = NULL;

    if (!lpcwBuffer) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    _ASSERTE(_pImport);
    
    hr = _pImport->GetManifestModulePath(wzBuf, &dwBuf);
    if (SUCCEEDED(hr)) {
        pwzTmp = PathFindFileName(wzBuf);

        _ASSERTE(pwzTmp);

        *pwzTmp = L'\0';
        dwLen = lstrlenW(wzBuf) + 1;

        if (*lpcwBuffer < dwLen) {
            *lpcwBuffer = dwLen;
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto Exit;
        }

        hr = StringCchCopy(pStr, *lpcwBuffer, wzBuf);
        if (FAILED(hr)) {
            goto Exit;
        }

        *lpcwBuffer = dwLen;
    }

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssembly::GetAssemblyLocation
// ---------------------------------------------------------------------------
STDMETHODIMP CAssembly::GetAssemblyLocation(DWORD *pdwAsmLocation)
{
    HRESULT                                  hr = S_OK;

    if (!pdwAsmLocation) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *pdwAsmLocation = _dwAsmLoc;

Exit:
    return hr;
}

// IUnknown methods

// ---------------------------------------------------------------------------
// CAssembly::AddRef
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssembly::AddRef()
{
    LONG                                lRef = -1;
    
    if (_pLoadContext) {
        _pLoadContext->Lock();
        lRef = InterlockedIncrement(&_cRef);
        _pLoadContext->Unlock();
    }
    else {
        lRef = InterlockedIncrement(&_cRef);
    }

    return lRef;
}

// ---------------------------------------------------------------------------
// CAssembly::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssembly::Release()
{
    LONG                                      lRef = -1;

    // we will do the trick if we have probed the native image,
    // or it is the left over by a partial bind.
    if (_pLoadContext) {
        _pLoadContext->Lock();
        lRef = InterlockedDecrement(&_cRef);
        if (lRef == 1) {
            // There is a circular reference count between the 
            // load context and the CAssembly (CAssembly holds 
            // back pointer to load context, and load context 
            // holds on to activated node, which contains a ref 
            // to the CAssembly). When release causes the ref 
            // count to go to 1, we know the only ref count 
            // left is from the load context (as long as nobody 
            // messed up the ref counting). Thus, at this time, 
            // we need to remove ourselves from the load context 
            // list, which will in turn, cause a release of this 
            // object, so it is properly destroyed.

            // In RemoveActication we have to QI ourself to get 
            // the assembly name def. That will increase the ref 
            // count to 2. When we release the QI, this logic kicks 
            // in again, causing an infinite loop.
            // We have to add some ref here to protect against this.
            //
            // We need to add 2 ref because we will do two release
            // in RemoveActivation, one for the QI, one for the copy
            // in load context.
            InterlockedIncrement(&_cRef);
            InterlockedIncrement(&_cRef);
            _pLoadContext->RemoveActivation((IAssembly *)this);
            InterlockedDecrement(&_cRef);
            lRef = InterlockedDecrement(&_cRef);
            _ASSERTE(lRef == 0);
        }

        _pLoadContext->Unlock();
    }
    else {
        lRef = InterlockedDecrement(&_cRef);
    }
    
    if (!lRef) {
        delete this;
    }

    return lRef;
}

// ---------------------------------------------------------------------------
// CAssembly::QueryInterface
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssembly::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) {
        return E_POINTER;
    }

    *ppv = NULL;

    if (riid==IID_IAssembly || riid == IID_IUnknown) {
        *ppv = (IAssembly *)this;
    }
    else {
        return E_NOINTERFACE;
    }

    ((IUnknown*)(*ppv))->AddRef();

    return S_OK;

} 

HRESULT CAssembly::SetAssemblyLocation(DWORD dwAsmLoc)
{
    _dwAsmLoc = dwAsmLoc;

    return S_OK;
}

HRESULT CAssembly::SetLoadContext(CLoadContext *pLoadContext)
{
    _ASSERTE(!_pLoadContext);

    _pLoadContext = pLoadContext;
    _pLoadContext->AddRef();

    return S_OK;
}

HRESULT CAssembly::SetProbingBase(LPCWSTR pwzProbingBase)
{
    HRESULT hr = S_OK;
    DWORD   dwSize = 0;
    
    _ASSERTE(pwzProbingBase && lstrlenW(pwzProbingBase) < MAX_URL_LENGTH);
    
    dwSize = lstrlenW(pwzProbingBase) + 1;
    SAFEDELETEARRAY(_pwzProbingBase);
    _pwzProbingBase = NEW(WCHAR[dwSize]);
    if (!_pwzProbingBase)
    {
        return E_OUTOFMEMORY;
    }

    hr = StringCchCopy(_pwzProbingBase, dwSize, pwzProbingBase);
    if (FAILED(hr)) {
        return hr;
    }
       
    return S_OK;
}

HRESULT CAssembly::GetProbingBase(
        __out_ecount_opt(*pccLength) LPWSTR pwzProbingBase, 
        __inout DWORD *pccLength)
{
    HRESULT                                 hr = S_OK;
    DWORD                                   dwLen;

    if (!pccLength) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (_pwzProbingBase) {
        dwLen = lstrlenW(_pwzProbingBase) + 1;

        if (!pwzProbingBase || *pccLength < dwLen) {
            *pccLength = dwLen;
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto Exit;
        }

        hr = StringCchCopy(pwzProbingBase, *pccLength, _pwzProbingBase);
        if (FAILED(hr)) {
            goto Exit;
        }

        *pccLength = dwLen;
    }
    else {
        *pccLength = 0;
    }

Exit:
    return hr;
}
    
LOADCTX_TYPE CAssembly::GetFusionLoadContext()
{
    if (!_pLoadContext) {
        return LOADCTX_TYPE_UNKNOWN;
    }

    return _pLoadContext->GetContextType();
}

STDMETHODIMP
CAssembly::GetNextHardBoundDependency(
        /* [in]   */ DWORD dwIndex, 
        /* [out]  */ IAssembly **ppILAsm, 
        /* [out]  */ IAssembly **ppNIAsm)
{
    _ASSERTE(!"This method can only be called on a native image assembly!!!");
    return E_NOTIMPL;
}

CNativeImageAssembly::CNativeImageAssembly()
: CAssembly()
{
    _dwSig = NATIVEIMAGEASSEMBLY_CLASS_SIGNATURE;
    _pILAsm = NULL;
    _NIStatus = 0;
    _dwNumHardBoundDeps = 0;
}

STDMETHODIMP_(ULONG)
CNativeImageAssembly::Release()
{
    LONG                                      lRef = -1;

    if (_pLoadContext) {
        _pLoadContext->Lock();
        lRef = InterlockedDecrement(&_cRef);
        if (lRef == 1) {
            // There is a circular reference count between the 
            // load context and the CAssembly (CAssembly holds 
            // back pointer to load context, and load context 
            // holds on to activated node, which contains a ref 
            // to the CAssembly). When release causes the ref 
            // count to go to 1, we know the only ref count 
            // left is from the load context (as long as nobody 
            // messed up the ref counting). Thus, at this time, 
            // we need to remove ourselves from the load context 
            // list, which will in turn, cause a release of this 
            // object, so it is properly destroyed.

            _ASSERTE(_cRef == 1);
            _cRef += 2;
            _pLoadContext->SetNativeImage(_pILAsm, NULL, NULL);
            _ASSERTE(_cRef == 2);
            _cRef -= 2;
            lRef = _cRef;
            _ASSERTE(lRef == 0);
        }

        _pLoadContext->Unlock();
    }
    else {
        lRef = InterlockedDecrement(&_cRef);
    }
    
    if (!lRef) {
        delete this;
    }

    return lRef;
}

CNativeImageAssembly::~CNativeImageAssembly()
{
    LISTNODE pos = NULL;
    IAssembly *pAsm = NULL;
    IAssemblyName *pName = NULL;

    SAFERELEASE(_pILAsm);

    pos = _listDepAsms.GetHeadPosition();
    while(pos) {
        pAsm = _listDepAsms.GetNext(pos);
        SAFERELEASE(pAsm);
    }
    _listDepAsms.RemoveAll();

    pos = _listFailureDepAsms.GetHeadPosition();
    while(pos) {
        pName = _listFailureDepAsms.GetNext(pos).pName;
        SAFERELEASE(pName);
    }
    _listDepAsms.RemoveAll();
}

HRESULT CNativeImageAssembly::AddDependentAssembly(IAssembly *pAsm)
{
    DWORD dwAsmLoc;
    
    _ASSERTE(pAsm);

    pAsm->GetAssemblyLocation(&dwAsmLoc);

    // Native image in the head    
    if (dwAsmLoc == ASMLOC_ZAP) {
        if (!_listDepAsms.AddHead(pAsm)) {
            return E_OUTOFMEMORY;
        }
        _dwNumHardBoundDeps++;
    }
    else {
        if (!_listDepAsms.AddTail(pAsm)) {
            return E_OUTOFMEMORY;
        }
    }

    pAsm->AddRef();
    return S_OK;
}

HRESULT CNativeImageAssembly::AddFailureDependentAssembly(IAssemblyName *pName, HRESULT hResult)
{
    _ASSERTE(pName);
    AssemblyBindFailure failure={pName, hResult};

    _ASSERTE(FAILED(hResult));

    if (!_listFailureDepAsms.AddTail(failure)) {
        return E_OUTOFMEMORY;
    }

    pName->AddRef();
    return S_OK;
}

STDMETHODIMP
CNativeImageAssembly::GetNextHardBoundDependency(
        /* [in]   */ DWORD dwIndex, 
        /* [out]  */ IAssembly **ppILAsm, 
        /* [out]  */ IAssembly **ppNIAsm)
{
    LISTNODE pos;
    IAssembly *pAsm;

    if (dwIndex >= _dwNumHardBoundDeps) {
        return HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
    }

    pos = _listDepAsms.GetHeadPosition();
    for (DWORD i = 0; i <= dwIndex; i++) {
        pAsm = _listDepAsms.GetNext(pos);
    }
#if _debug
    DWORD dwAsmLoc;
    pAsm->GetAssemblyLocation(&dwAsmLoc);
    _ASSERTE(dwAsmLoc == ASMLOC_ZAP);
#endif
    
    *ppNIAsm = pAsm;
    (*ppNIAsm)->AddRef();

    *ppILAsm = ((CNativeImageAssembly *)pAsm)->_pILAsm;
    (*ppILAsm)->AddRef();
    
    return S_OK;
}
