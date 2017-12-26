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
#include "bindresult.h"
#include "naming.h"
#include "util.h"
#include "helpers.h"
#include "lock.h"

// class CBindingInput

CBindingInput::CBindingInput()
:_cRef(1)
,_pwzAsmName(NULL)
,_pwzCodebase(NULL)
,_ctxType(LOADCTX_TYPE_DEFAULT)
,_pwzProbingBase(NULL)
{
}

CBindingInput::~CBindingInput()
{
    SAFEDELETEARRAY(_pwzAsmName);
    SAFEDELETEARRAY(_pwzCodebase);
    SAFEDELETEARRAY(_pwzProbingBase);
}

/* static */
HRESULT CBindingInput::Create(
                IAssemblyName *pName, 
                LPCWSTR pwzCodebase, 
                LOADCTX_TYPE ctxType, 
                LPCWSTR pwzProbingBase, 
                CBindingInput **ppBindInput)
{
    HRESULT hr = S_OK;
    CBindingInput *pBindInput = NULL;

    _ASSERTE(ppBindInput);
    _ASSERTE(pName || pwzCodebase);
    _ASSERTE( ctxType == LOADCTX_TYPE_DEFAULT || pwzProbingBase || pwzCodebase);

    *ppBindInput = NULL;

    pBindInput = NEW(CBindingInput);
    if (!pBindInput) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pBindInput->Init(pName, pwzCodebase, ctxType, pwzProbingBase);
    if (SUCCEEDED(hr)) {
        *ppBindInput = pBindInput;
    }

Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pBindInput);
    }
    return hr;
}

HRESULT CBindingInput::Init(IAssemblyName *pName, LPCWSTR pwzCodebase, LOADCTX_TYPE ctxType, LPCWSTR pwzProbingBase)
{
    HRESULT hr = S_OK;
    WCHAR   wzName[MAX_PATH];
    DWORD   dwSize = MAX_PATH;
    BOOL    bWhereRefBind = FALSE;
    DWORD   dwLen;   

    dwLen = 0;
    if (pName->GetName(&dwLen, NULL) != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bWhereRefBind = TRUE;
    }

    _ctxType = ctxType;

    if (pwzCodebase) {
        _pwzCodebase = WSTRDupDynamic(pwzCodebase);
        if (!_pwzCodebase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    if (pwzProbingBase) {
        _pwzProbingBase = WSTRDupDynamic(pwzProbingBase);
        if (!_pwzProbingBase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    if (pName && !bWhereRefBind) {
        DWORD   dwCmpFlags = 0;
        DWORD   dwDispFlags = ASM_DISPLAYF_FULL;
        BOOL fIsStronglyNamed = CAssemblyName::IsStronglyNamed(pName);

        CAssemblyName::IsPartial(pName, &dwCmpFlags);

        // input is not strongly named, don't store version
        if ((dwCmpFlags & ASM_CMPF_PUBLIC_KEY_TOKEN) && !fIsStronglyNamed) {
            dwDispFlags = ASM_DISPLAYF_FULL_NO_VERSION;
        }

        hr = CAssemblyName::GetCustomDisplayName(pName, wzName, &dwSize, dwDispFlags);
        if (FAILED(hr) && (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))){
            goto Exit;
        }

        _pwzAsmName = NEW(WCHAR[dwSize]);
        if (!_pwzAsmName) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        if (SUCCEEDED(hr)) {
            memcpy(_pwzAsmName, wzName, dwSize * sizeof(WCHAR));
        }
        else {
            hr = CAssemblyName::GetCustomDisplayName(pName, _pwzAsmName, &dwSize, dwDispFlags);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

Exit:
    return hr;
}

ULONG CBindingInput::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CBindingInput::Release()
{
    LONG              lRef = InterlockedDecrement(&_cRef);

    if (!lRef) {
        delete this;
    }

    return lRef;
}

DWORD CBindingInput::GetHash()
{
    DWORD dwHash = 0;

    dwHash = HashBlob((LPBYTE)&_ctxType, sizeof(LOADCTX_TYPE), dwHash, 0);

    if (_pwzAsmName) {
        dwHash = HashString(_pwzAsmName, dwHash, 0, FALSE);
    }

    if (_pwzCodebase) {
        dwHash = HashString(_pwzCodebase, dwHash, 0, FALSE);
    }

    if (_pwzProbingBase) {
        dwHash = HashString(_pwzProbingBase, dwHash, 0, FALSE);
    }

    return dwHash;
}

HRESULT CBindingInput::IsEqual(CBindingInput *pBindInput)
{
    _ASSERTE(pBindInput);

    if (_ctxType != pBindInput->_ctxType) {
        return S_FALSE;
    }

    if (_pwzAsmName && pBindInput->_pwzAsmName) {
        if (FusionCompareStringI(_pwzAsmName, pBindInput->_pwzAsmName)) {
            return S_FALSE;
        }
    }
    else if (_pwzAsmName ||  pBindInput->_pwzAsmName) {
        return S_FALSE;
    }

    if (_pwzCodebase && pBindInput->_pwzCodebase) {
        if (FusionCompareStringI(_pwzCodebase, pBindInput->_pwzCodebase)) {
            return S_FALSE;
        }
    }
    else if (_pwzCodebase ||  pBindInput->_pwzCodebase) {
        return S_FALSE;
    }

    if (_pwzProbingBase && pBindInput->_pwzProbingBase) {
        if (FusionCompareStringI(_pwzProbingBase, pBindInput->_pwzProbingBase)) {
            return S_FALSE;
        }
    }
    else if (_pwzProbingBase ||  pBindInput->_pwzProbingBase) {
        return S_FALSE;
    }

    return S_OK;
}

// class CBindingOutput
CBindingOutput::CBindingOutput()
:_cRef(1)
,_pwzAsmName(NULL)
,_ctxType(LOADCTX_TYPE_DEFAULT)
,_hr(S_OK)
{
}

CBindingOutput::~CBindingOutput()
{
    SAFEDELETEARRAY(_pwzAsmName);
}

/* static */
HRESULT CBindingOutput::Create(
                IAssemblyName *pName, 
                LOADCTX_TYPE ctxType, 
                HRESULT hrIn, 
                CBindingOutput **ppBindOutput)
{
    HRESULT hr = S_OK;
    CBindingOutput *pBindOutput = NULL;

    _ASSERTE(ppBindOutput);

    *ppBindOutput = NULL;

    pBindOutput = NEW(CBindingOutput);
    if (!pBindOutput) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pBindOutput->Init(pName, ctxType, hrIn);
    if (SUCCEEDED(hr)) {
        *ppBindOutput = pBindOutput;
    }

Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pBindOutput);
    }
    return hr;
}

HRESULT CBindingOutput::Init(IAssemblyName *pName, LOADCTX_TYPE ctxType, HRESULT hrIn)
{
    HRESULT hr = S_OK;
    WCHAR   wzName[MAX_PATH];
    DWORD   dwSize = MAX_PATH;

    _ctxType = ctxType;

    _hr = hrIn;

    if (pName) {
        hr = CAssemblyName::GetCustomDisplayName(pName, wzName, &dwSize, ASM_DISPLAYF_FULL);
        if (FAILED(hr) && (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))){
            goto Exit;
        }

        _pwzAsmName = NEW(WCHAR[dwSize]);
        if (!_pwzAsmName) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        if (SUCCEEDED(hr)) {
            memcpy(_pwzAsmName, wzName, dwSize * sizeof(WCHAR));
        }
        else {
            hr = CAssemblyName::GetCustomDisplayName(pName, _pwzAsmName, &dwSize, ASM_DISPLAYF_FULL);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

Exit:
    return hr;
}

ULONG CBindingOutput::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CBindingOutput::Release()
{
    LONG              lRef = InterlockedDecrement(&_cRef);

    if (!lRef) {
        delete this;
    }

    return lRef;
}

// class CBindingResult
CBindingResult::CBindingResult(CBindingInput *pBindInput, CBindingOutput *pBindOutput)
:_pBindInput(pBindInput)
,_pBindOutput(pBindOutput)
{
    _ASSERTE(pBindInput);
    _ASSERTE(pBindOutput);

    _pBindInput->AddRef();

    _pBindOutput->AddRef();
}

CBindingResult::~CBindingResult()
{
    SAFERELEASE(_pBindInput);
    SAFERELEASE(_pBindOutput);
}

// class CBindingResultCache
CBindingResultCache::CBindingResultCache()
:_cRef(1)
{
}

CBindingResultCache::~CBindingResultCache()
{
    int                 i;
    LISTNODE            pos;
    CBindingResult      *pResult;

    for (i = 0; i < BINDING_RESULT_CACHE_SIZE; i++) {
        pos = _result[i].GetHeadPosition();

        while (pos) {
            pResult = _result[i].GetNext(pos);
            _ASSERTE(pResult);

            SAFEDELETE(pResult);
        }

        _result[i].RemoveAll();
    }

    if (_cs) {
        ClrDeleteCriticalSection(_cs);
    }
}

ULONG CBindingResultCache::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CBindingResultCache::Release()
{
    LONG              lRef = InterlockedDecrement(&_cRef);

    if (!lRef) {
        delete this;
    }

    return lRef;
}

/* static */
HRESULT CBindingResultCache::Create(CBindingResultCache **ppBindCache)
{
    HRESULT             hr = S_OK;
    CBindingResultCache *pBindCache = NULL;

    _ASSERTE(ppBindCache);

    *ppBindCache = NULL;

    pBindCache = NEW(CBindingResultCache());
    if (!pBindCache) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pBindCache->Init();
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppBindCache = pBindCache;
    (*ppBindCache)->AddRef();

Exit:
    SAFERELEASE(pBindCache);

    return hr;
}

HRESULT CBindingResultCache::Init()
{
    HRESULT                              hr = S_OK;
    
    _cs = ClrCreateCriticalSection("Fusion: Binding Result Cache", CrstFusionBindResult, CRST_REENTRANCY);
    if (!_cs) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CBindingResultCache::CheckBindingResult(
                    CBindingInput *pBindInput,
                    CBindingOutput **ppBindOutput)
{
    HRESULT hr = S_OK;
    DWORD dwHash = 0;
    CBindingResult *pResult = NULL;
    LISTNODE pos;
    CCriticalSection cs(_cs);

    _ASSERTE(pBindInput);
    _ASSERTE(ppBindOutput);

    *ppBindOutput = NULL;
    
    dwHash = pBindInput->GetHash() % BINDING_RESULT_CACHE_SIZE;

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _result[dwHash].GetHeadPosition();
    while(pos) {
        pResult = _result[dwHash].GetNext(pos);

        if (pResult->_pBindInput->IsEqual(pBindInput) == S_OK) {
            *ppBindOutput = pResult->_pBindOutput;
            (*ppBindOutput)->AddRef();
            hr = S_OK;
            goto Exit;
        }
    }

    hr = S_FALSE;

Exit:
    return hr;
}

HRESULT CBindingResultCache::AddBindingResult(CBindingInput *pBindInput, CBindingOutput *pBindOutput)
{
    HRESULT hr = S_OK;
    DWORD   dwHash = 0;
    CBindingResult *pResult = NULL;
    CCriticalSection cs(_cs);
    LISTNODE pos;

    _ASSERTE(pBindInput);
    _ASSERTE(pBindOutput);
    
    dwHash = pBindInput->GetHash() % BINDING_RESULT_CACHE_SIZE;
    
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _result[dwHash].GetHeadPosition();
    while(pos) {
        pResult = _result[dwHash].GetNext(pos);
        if (pResult->_pBindInput->IsEqual(pBindInput) == S_OK) {
            hr = S_OK;
            goto Exit;
        }
    }

    pResult = NEW(CBindingResult(pBindInput, pBindOutput));
    if (!pResult) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    if (!_result[dwHash].AddTail(pResult)) {
        SAFEDELETE(pResult);
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pResult = NULL;

Exit:
    return hr;
}
