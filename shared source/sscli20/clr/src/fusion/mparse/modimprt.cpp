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
#include "fusionp.h"
#include "asmstrm.h"
#include "asmimprt.h"
#include "modimprt.h"
#include "asmcache.h"
#include "adl.h"
#include "mdlmgr.h"
#include "naming.h"
#include "lock.h"

// ---------------------------------------------------------------------------
// CreateAssemblyNameModuleImport
// ---------------------------------------------------------------------------
STDAPI
CreateAssemblyModuleImport(
    LPCWSTR            szModuleName,
    LPBYTE             pbHashValue,
    DWORD              cbHashValue,
    DWORD              dwFlags,
    LPASSEMBLYNAME     pNameDef,
    CAssemblyManifestImport *pManImport,
    LPASSEMBLY_MODULE_IMPORT *ppImport)
{
    HRESULT hr = S_OK;
    CAssemblyModuleImport *pImport = NULL;

    pImport = NEW(CAssemblyModuleImport);
    if (!pImport)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = pImport->Init(
        szModuleName,
        pbHashValue,
        cbHashValue,
        pNameDef,
        pManImport,
        dwFlags);

    if (FAILED(hr)) 
    {
        SAFERELEASE(pImport);
        goto exit;
    }

exit:

    *ppImport = pImport;
    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyModuleImport ctor
// ---------------------------------------------------------------------------
CAssemblyModuleImport::CAssemblyModuleImport()
{
    _dwSig         = 0x49444f4d; /* 'IDOM' */
    _pszModuleName = NULL;   
    _pbHashValue   = NULL;
    _cbHashValue   = 0;
    _dwFlags       = 0;
    _pNameDef      = NULL;
    _pManImport    = NULL; 

    _cRef          = 1;
}


// ---------------------------------------------------------------------------
// CAssemblyModuleImport dtor
// ---------------------------------------------------------------------------
CAssemblyModuleImport::~CAssemblyModuleImport()
{
    SAFEDELETEARRAY(_pszModuleName);
    SAFEDELETEARRAY(_pbHashValue);
    SAFERELEASE(_pNameDef);
    SAFERELEASE(_pManImport);
}


// ---------------------------------------------------------------------------
// CAssemblyModuleImport::Init
// ---------------------------------------------------------------------------
HRESULT CAssemblyModuleImport::Init (
    LPCWSTR            szModuleName,
    LPBYTE             pbHashValue,
    DWORD              cbHashValue,
    LPASSEMBLYNAME     pNameDef,
    CAssemblyManifestImport *pManImport,
    DWORD              dwFlags)
{
    HRESULT hr = S_OK;
    DWORD   dwSize;

    // Module path.
    dwSize = lstrlenW(szModuleName) + 1;
    _pszModuleName = NEW(TCHAR[dwSize]);
    if (!_pszModuleName)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    memcpy(_pszModuleName, szModuleName, dwSize * sizeof(TCHAR));

    // Hash value and count
    _cbHashValue = cbHashValue;
    _pbHashValue = NEW(BYTE[_cbHashValue]);
    if (!_pbHashValue)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }    
    memcpy(_pbHashValue, pbHashValue, _cbHashValue);

    // Name def back pointer
    _pNameDef = pNameDef;
    if (_pNameDef) {
        _pNameDef->AddRef();
    }

    _pManImport = pManImport;
    if (_pManImport)
        _pManImport->AddRef();
        
    // Flags
    _dwFlags = dwFlags;

exit:

    return hr;
}


//
// IStream::Read is only IStream implemented method.
//

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::Read
// Implements IStream::Read
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyModuleImport::Read(VOID *pv, ULONG cb, ULONG *pcbRead)
{    
    return E_NOTIMPL;
}

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::GetModuleName
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyModuleImport::GetModuleName(__out_ecount_opt(*pccModuleName) LPOLESTR pszModuleName,
    LPDWORD pccModuleName)
{
    HRESULT hr     = S_OK;
    DWORD  ccName  = NULL;
    
    ccName = lstrlenW(_pszModuleName) + 1;
    if (*pccModuleName < ccName)
    {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto exit;
    }

    memcpy(pszModuleName, _pszModuleName, ccName * sizeof(TCHAR));

exit:
    *pccModuleName = ccName;
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::GetHashAlgId
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyModuleImport::GetHashAlgId(LPDWORD pdwHashAlgId)
{
    DWORD cbHashAlgId = sizeof(DWORD);
    if (_pNameDef)
    {
        return _pNameDef->GetProperty(ASM_NAME_HASH_ALGID, 
            pdwHashAlgId, &cbHashAlgId);
    }
    else
        return E_FAIL;
}

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::GetHashValue
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyModuleImport::GetHashValue(LPBYTE pbHashValue,
    LPDWORD pcbHashValue)
{
    HRESULT hr = S_OK;
    if (*pcbHashValue < _cbHashValue)
    {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto exit;
    }

    memcpy(pbHashValue, _pbHashValue, _cbHashValue);
        
exit:
    *pcbHashValue = _cbHashValue;
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::GetFlags
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyModuleImport::GetFlags(LPDWORD pdwFlags)
{
    *pdwFlags = _dwFlags;
    return S_OK;
}


// ---------------------------------------------------------------------------
// CAssemblyModuleImport::GetModulePath
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyModuleImport::GetModulePath(__out_ecount_opt(*pccModuleName) LPOLESTR pszModuleName,
    LPDWORD pccModuleName)
{
    HRESULT hr = S_OK;
    WCHAR   wzBuf[MAX_PATH];
    DWORD   dwSize = MAX_PATH;
    LPWSTR  pwzFileName = NULL;
    DWORD   cchPathSize;
    SIZE_T  size;

    hr = _pManImport->GetManifestModulePath(wzBuf, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    pwzFileName = PathFindFileName(wzBuf);

    cchPathSize = (DWORD)(pwzFileName-wzBuf);
    dwSize = cchPathSize + lstrlenW(_pszModuleName) + 1;

    _ASSERTE(dwSize <= MAX_PATH);

    size = MAX_PATH - cchPathSize;
    hr = StringCchCopy(pwzFileName, size, _pszModuleName);

    if (*pccModuleName < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        hr = StringCchCopy(pszModuleName, *pccModuleName, wzBuf);
    }

    *pccModuleName = dwSize;

Exit:
    return hr;
}

//
// IStream methods not implemented...
//

STDMETHODIMP CAssemblyModuleImport::Commit(DWORD dwCommitFlags)
{
    return E_NOTIMPL;
}


STDMETHODIMP CAssemblyModuleImport::Write(VOID const *pv, ULONG cb,
            ULONG *pcbWritten)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
            ULARGE_INTEGER *plibNewPosition)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::SetSize(ULARGE_INTEGER libNewSize)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::CopyTo(IStream *pStm, ULARGE_INTEGER cb,
            ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::Revert()
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::LockRegion(ULARGE_INTEGER libOffset,
            ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::UnlockRegion(ULARGE_INTEGER libOffset,
            ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::Stat(STATSTG *pStatStg, DWORD grfStatFlag)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAssemblyModuleImport::Clone(IStream **ppStm)
{
    return E_NOTIMPL;
}

//
// IUnknown boilerplate...
//

STDMETHODIMP CAssemblyModuleImport::QueryInterface
    (REFIID riid, LPVOID * ppvObj)
{
    if (!ppvObj) 
        return E_POINTER;
    
    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IAssemblyModuleImport)
       )
    {
        *ppvObj = static_cast<IAssemblyModuleImport*> (this);
        AddRef();
        return S_OK;
    }
    else if ( IsEqualIID(riid, IID_IStream))
    {
        *ppvObj = static_cast<IStream*> (this);
        AddRef();
        return S_OK;
    }        
    else
    {
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG) CAssemblyModuleImport::AddRef(void)
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CAssemblyModuleImport::Release(void)
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }

    return ulRef;
}

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::IsAvailable
// ---------------------------------------------------------------------------

STDMETHODIMP_(BOOL) CAssemblyModuleImport::IsAvailable()
{
    HRESULT hr = S_OK;
    WCHAR   wzBuf[MAX_PATH];
    DWORD   dwSize = MAX_PATH;
    BOOL bExists = FALSE;

    if (!_pszModuleName) {
        return FALSE;
    }
  
    if (_pManImport->IsInGAC()) {
       return TRUE;
    }

    hr = GetModulePath(wzBuf, &dwSize);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = CheckFileExistence(wzBuf, &bExists, NULL);
    if (FAILED(hr)) {
        return FALSE;
    }
    
    return bExists;
}

// ---------------------------------------------------------------------------
// CAssemblyModuleImport::BindToObject
// ---------------------------------------------------------------------------

STDMETHODIMP CAssemblyModuleImport::BindToObject(IAssemblyBindSink *pBindSink,
                                                 IApplicationContext *pAppCtx,
                                                 LONGLONG llFlags,
                                                 LPVOID *ppv)
{
    HRESULT                              hr = S_OK;
    LPTSTR                               ptszTemp = NULL;
    LPWSTR                               pwzCodebase = NULL;
    LPWSTR                               pwzCodebaseUrl = NULL;
    TCHAR                                ptszModuleName[MAX_PATH];
    WCHAR                                pwszModuleName[MAX_PATH];
    LPWSTR                               wzCanonicalized=NULL;
    DWORD                                dwLen = 0;
    DWORD                                cbModName = 0;
    int                                  iLen = 0;
    CTransCache                         *pTransCache = NULL;
    CModDownloadMgr                     *pDLMgr = NULL;
    CAssemblyDownload                   *padl = NULL;
    CDebugLog                           *pdbglog = NULL;

    if (!pAppCtx || !pBindSink || !ppv) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (IsAvailable()) {
        // Client did not need to call BTO, but they did anyway!
        *ppv = this;
        AddRef();
        hr = S_OK;
        goto Exit;
    }

    // Get the CODEBASE of the assembly from the assembly name def.
    if (FAILED(hr = NameObjGetWrapper(_pNameDef, ASM_NAME_CODEBASE_URL, 
        (LPBYTE*) &pwzCodebaseUrl, &dwLen)))
        goto Exit;
        
    if (!pwzCodebaseUrl) {
        // We don't have a codebase! The assembly was populated by
        // some other means other than BindToObject. Abort.
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto Exit;
    }

    // Construct the codebase for the module

    cbModName = MAX_PATH;
    hr = GetModuleName(ptszModuleName, &cbModName);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCbCopy(pwszModuleName, sizeof(pwszModuleName), ptszModuleName);
    if (FAILED(hr)) {
        goto Exit;
    }

    ptszTemp = PathFindFileName(pwzCodebaseUrl) ;
    *ptszTemp = TEXT('\0');
    iLen = lstrlenW(pwzCodebaseUrl) + lstrlenW(ptszModuleName) + 1;

    pwzCodebase = NEW(WCHAR[iLen]);
    if (!pwzCodebase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = StringCchPrintf(pwzCodebase, iLen, TEXT("%ws%ws"), pwzCodebaseUrl, ptszModuleName);
    if (FAILED(hr)) {
        goto Exit;
    }

    wzCanonicalized = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzCanonicalized)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    dwLen = MAX_URL_LENGTH;
    hr = UrlCanonicalizeUnescape(pwzCodebase, wzCanonicalized, &dwLen, 0);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    // Create debug log

    CreateLogObject(&pdbglog, pwszModuleName, pAppCtx);

    // Download the module

    hr = CModDownloadMgr::Create(&pDLMgr, _pNameDef, _pManImport, 
        pAppCtx, wzCanonicalized, pwszModuleName, pdbglog);
    if (FAILED(hr)) {
        goto Exit;
    }                             

    hr = CAssemblyDownload::Create(&padl, pDLMgr, pDLMgr, pdbglog, llFlags);
    if (SUCCEEDED(hr)) {
        hr = padl->AddClient(pBindSink, TRUE);
        if (FAILED(hr)) {
            SAFERELEASE(pDLMgr);
            SAFERELEASE(padl);
            goto Exit;
        }

        hr = padl->KickOffDownload(TRUE);
    }

Exit:

    SAFERELEASE(pTransCache);
    SAFERELEASE(padl);
    SAFERELEASE(pDLMgr);

    SAFEDELETEARRAY(pwzCodebase);
    SAFEDELETEARRAY(pwzCodebaseUrl);
    SAFERELEASE(pdbglog);
    SAFEDELETEARRAY(wzCanonicalized);
    return hr;
}

// Hands out a pointer to this interface's owned name def.
HRESULT CAssemblyModuleImport::GetNameDef(LPASSEMBLYNAME *ppName)
{
    _ASSERTE(_pNameDef);

    *ppName = _pNameDef;
    (*ppName)->AddRef();

    return S_OK;
}

HRESULT CAssemblyModuleImport::CreateLogObject(CDebugLog **ppdbglog,
                                               LPCWSTR pwszModuleName,
                                               IApplicationContext *pAppCtx)
{
    HRESULT                              hr = S_OK;
    LPWSTR                               pwzAsmName = NULL;
    LPWSTR                               pwzDbgName = NULL;
    DWORD                                dwSize;
    int                                  iLen = 0;

    if (!ppdbglog || !pwszModuleName || !pAppCtx) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    pwzAsmName = NULL;
    pwzDbgName = NULL;
    dwSize = 0;

    hr = _pNameDef->GetDisplayName(NULL, &dwSize, 0);
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        goto Exit;
    }

    pwzAsmName = NEW(WCHAR[dwSize]);
    if (!pwzAsmName) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = _pNameDef->GetDisplayName(pwzAsmName, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    iLen = lstrlenW(pwzAsmName) + lstrlenW(pwszModuleName) + 2; // +1 for delimiter +1 for NULL

    pwzDbgName = NEW(WCHAR[iLen]);
    if (!pwzDbgName) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = StringCchCopy(pwzDbgName, iLen, pwszModuleName);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzDbgName, iLen, L"!");
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzDbgName, iLen, pwzAsmName);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = CDebugLog::Create(pAppCtx, pwzDbgName, ppdbglog);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    SAFEDELETEARRAY(pwzAsmName);
    SAFEDELETEARRAY(pwzDbgName);
    
    return hr;
}

