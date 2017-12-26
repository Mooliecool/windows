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

#include "asmidmgr.h"
#include "imprthelpers.h"
#include "helpers.h"
#include "util.h"
#include "naming.h"


// class CAssemblyIdentityManager

CAssemblyIdentityManager CAssemblyIdentityManager::_IdentityManager;

STDMETHODIMP
CAssemblyIdentityManager::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;

    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_ICLRAssemblyIdentityManager))
    {
        *ppv = static_cast<ICLRAssemblyIdentityManager*> (this);
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

ULONG CAssemblyIdentityManager::AddRef() 
{
    return 1;
}

ULONG CAssemblyIdentityManager::Release()
{
    return 1;
}

HRESULT CAssemblyIdentityManager::GetCLRAssemblyReferenceList(
        LPCWSTR *ppwzAssemblyReferences,
        DWORD   dwNumOfReferences,
        ICLRAssemblyReferenceList **ppRefList)
{
    HRESULT hr = S_OK;

    if (!ppRefList) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    hr = CAssemblyReferenceList::Create(ppwzAssemblyReferences, dwNumOfReferences, ppRefList);
    
Exit:
    return hr;
}

HRESULT CAssemblyIdentityManager::GetBindingIdentityFromFile(
        LPCWSTR pwzFilePath,
        DWORD   dwFlags,
        __out_ecount_opt(*pcchBufferSize) LPWSTR  pwzBuffer,
        __inout DWORD   *pcchBufferSize)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;
    IMetaDataAssemblyImport *pImport = NULL;
    DWORD dwImportFlags = ASM_IMPORT_NAME_ARCHITECTURE|ASM_IMPORT_NAME_PE_RUNTIME;
    DWORD dwDispFlags =  ASM_DISPLAYF_FULL;
    WORD  wVer = 0;

    if (!pwzFilePath || !pcchBufferSize) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    hr = CreateMetaDataImport(pwzFilePath, MDInternalImport_NoCache, &pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = GetAssemblyNameDefFromMDImport(pImport, dwImportFlags, FALSE, &pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!CAssemblyName::IsStronglyNamed(pName)) {
        for (DWORD i=0; i < 4; i++) {
            hr = pName->SetProperty(ASM_NAME_MAJOR_VERSION+i, (LPBYTE)&wVer, sizeof(WORD));
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    hr = pName->GetDisplayName(pwzBuffer, pcchBufferSize, dwDispFlags);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CanonicalizeIdentity(pwzBuffer);
    if (FAILED(hr)) {
        goto Exit;
    }   

Exit:
    SAFERELEASE(pName);
    SAFERELEASE(pImport);
    return hr;
}

HRESULT CAssemblyIdentityManager::GetBindingIdentityFromStream(
        IStream *pStream,
        DWORD   dwFlags,
        __out_ecount_opt(*pcchBufferSize) LPWSTR  pwzBuffer,
        __inout DWORD   *pcchBufferSize)
{
    HRESULT                         hr = S_OK;
    IAssemblyName                   *pName = NULL;
    IMetaDataAssemblyImport         *pImport = NULL;
    DWORD                           dwImportFlags = ASM_IMPORT_NAME_ARCHITECTURE|ASM_IMPORT_NAME_PE_RUNTIME;
    DWORD                           dwDispFlags =  ASM_DISPLAYF_FULL;

    WORD                            wVer = 0;

    if (!pStream || !pcchBufferSize) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    hr = CreateMetaDataImport(pStream, 0, MDInternalImport_NoCache, &pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = GetAssemblyNameDefFromMDImport(pImport, dwImportFlags, FALSE, &pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!CAssemblyName::IsStronglyNamed(pName)) {
        for (DWORD i=0; i < 4; i++) {
            hr = pName->SetProperty(ASM_NAME_MAJOR_VERSION+i, (LPBYTE)&wVer, sizeof(WORD));
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    hr = pName->GetDisplayName(pwzBuffer, pcchBufferSize, dwDispFlags);
    if (FAILED(hr)) {
        goto Exit;
    }


    hr = CanonicalizeIdentity(pwzBuffer);
    if (FAILED(hr)) {
        goto Exit;
    }
    
Exit:
    SAFERELEASE(pName);
    SAFERELEASE(pImport);
    return hr;
}

HRESULT CAssemblyIdentityManager::GetReferencedAssembliesFromFile(
        LPCWSTR                     pwzFilePath,
        DWORD                       dwFlags,
        ICLRAssemblyReferenceList   *pExcludeAssembliesList,
        ICLRReferenceAssemblyEnum   **ppReferenceEnum)
{
    HRESULT                     hr = S_OK;
    IMetaDataAssemblyImport     *pImport = NULL;

    if (!pwzFilePath || !ppReferenceEnum) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    hr = CreateMetaDataImport(pwzFilePath, MDInternalImport_NoCache, &pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CReferenceAssemblyEnum::Create(pImport, dwFlags, pExcludeAssembliesList, ppReferenceEnum);

Exit:
    SAFERELEASE(pImport);
    return hr;
}

HRESULT CAssemblyIdentityManager::GetReferencedAssembliesFromStream(
        IStream                     *pStream,
        DWORD                       dwFlags,
        ICLRAssemblyReferenceList   *pExcludeAssembliesList,
        ICLRReferenceAssemblyEnum   **ppReferenceEnum)
{
    HRESULT                     hr = S_OK;
    IMetaDataAssemblyImport     *pImport = NULL;

    if (!pStream || !ppReferenceEnum) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    hr = CreateMetaDataImport(pStream, 0, MDInternalImport_NoCache, &pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CReferenceAssemblyEnum::Create(pImport, dwFlags, pExcludeAssembliesList, ppReferenceEnum);

Exit:
    SAFERELEASE(pImport);
    return hr;
}

HRESULT CAssemblyIdentityManager::GetProbingAssembliesFromReference(
        DWORD           dwMachineType,
        DWORD           dwFlags,
        LPCWSTR         pwzReferenceIdentity,
        ICLRProbingAssemblyEnum     **ppProbingAssemblyEnum)
{
    HRESULT hr = S_OK;


    if (!pwzReferenceIdentity || !ppProbingAssemblyEnum) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    hr = CProbingAssemblyEnum::Create(dwMachineType, dwFlags, pwzReferenceIdentity, ppProbingAssemblyEnum);

Exit:
    return hr;
}

HRESULT CAssemblyIdentityManager::IsStronglyNamed(
        LPCWSTR     pwzAssemblyIdentity,
        BOOL        *pbIsStronglyNamed)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;

    if (!pwzAssemblyIdentity || !pwzAssemblyIdentity[0] || !pbIsStronglyNamed) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = CreateAssemblyNameObject(&pName, pwzAssemblyIdentity, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    *pbIsStronglyNamed = CAssemblyName::IsStronglyNamed(pName);

Exit:
    SAFERELEASE(pName);
    return hr;
}

// class CReferenceAssemblyEum

CReferenceAssemblyEnum::CReferenceAssemblyEnum()
: _cRef(1)
, _dwRefCount(0)
, _ppAsmRefs(NULL)
{
}

CReferenceAssemblyEnum::~CReferenceAssemblyEnum()
{
    if (_dwRefCount && _ppAsmRefs) {
        for (DWORD i = 0; i < _dwRefCount; i++) {
            SAFERELEASE(_ppAsmRefs[i]);
        }
        SAFEDELETEARRAY(_ppAsmRefs);
    }
}

/* static */
HRESULT 
CReferenceAssemblyEnum::Create(
        IMetaDataAssemblyImport *pImport, 
        DWORD   dwFlags,
        ICLRAssemblyReferenceList *pExcludeAssembliesList,
        ICLRReferenceAssemblyEnum **ppRefEnum)
{
    HRESULT hr = S_OK;
    CReferenceAssemblyEnum *pRefEnum = NULL;
    
    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    *ppRefEnum = NULL;

    pRefEnum = NEW(CReferenceAssemblyEnum);
    if (!pRefEnum) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pRefEnum->Init(pImport, dwFlags, pExcludeAssembliesList);
    if (FAILED(hr)) {
        SAFERELEASE(pRefEnum);
        goto Exit;
    }

    *ppRefEnum = pRefEnum;
    
Exit:
    return hr;
}

STDMETHODIMP
CReferenceAssemblyEnum::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;

    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_ICLRReferenceAssemblyEnum))
    {
        *ppv = static_cast<ICLRReferenceAssemblyEnum*> (this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

ULONG CReferenceAssemblyEnum::AddRef() 
{
    return InterlockedIncrement(&_cRef);
}

ULONG CReferenceAssemblyEnum::Release()
{
    ULONG lRes = InterlockedDecrement(&_cRef);

    if (lRes == 0) {
        delete this;
    }

    return lRes;
}

HRESULT CReferenceAssemblyEnum::Get(
        DWORD dwIndex,
        __out_ecount_opt(*pcchBufferSize) LPWSTR  pwzBuffer,
        __inout DWORD   *pcchBufferSize)
{
    HRESULT hr = S_OK;
    DWORD dwDispFlags = ASM_DISPLAYF_VERSION
            | ASM_DISPLAYF_CULTURE
            | ASM_DISPLAYF_PUBLIC_KEY_TOKEN;
    
    if (!pcchBufferSize) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (dwIndex >= _dwRefCount) {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto Exit;
    }

    hr = _ppAsmRefs[dwIndex]->GetDisplayName(pwzBuffer, pcchBufferSize, dwDispFlags);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CanonicalizeIdentity(pwzBuffer);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CReferenceAssemblyEnum::Init(
        IMetaDataAssemblyImport *pImport,
        DWORD   dwFlags,
        ICLRAssemblyReferenceList *pExcludeAssembliesList)
{
    HRESULT     hr              = S_OK;
    mdAssembly  *rAssemblyRefTokens = NULL;
    DWORD       cAssemblyRefTokens = 0;
    DWORD       dwCurrent;
    WORD        wVer = 0;
    DWORD       i;
    DWORD       j;
    
    _dwFlags = dwFlags;

    hr = GetAssemblyRefTokens(pImport, &rAssemblyRefTokens, &cAssemblyRefTokens);
    if (FAILED(hr))
        goto Exit;
    
    if (cAssemblyRefTokens) {
        _ppAsmRefs = NEW(LPASSEMBLYNAME[cAssemblyRefTokens]);
        if (!_ppAsmRefs) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        memset(_ppAsmRefs, 0, sizeof(LPASSEMBLYNAME)*cAssemblyRefTokens);

        _dwRefCount = cAssemblyRefTokens;

        dwCurrent = 0;
        for (i = 0; i < cAssemblyRefTokens; i++) {
            hr = GetAssemblyNameRefFromMDImport(pImport, rAssemblyRefTokens[i], 0, &_ppAsmRefs[dwCurrent]);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (pExcludeAssembliesList) {
                hr = pExcludeAssembliesList->IsAssemblyReferenceInList(_ppAsmRefs[dwCurrent]);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (hr == S_OK) {
                    // re-use the same slot.
                    SAFERELEASE(_ppAsmRefs[dwCurrent]);
                    _dwRefCount--;
                    continue;
                }
            }

            if (!CAssemblyName::IsStronglyNamed(_ppAsmRefs[dwCurrent])) {
                for (j=0; j < 4; j++) {
                    hr = (_ppAsmRefs[dwCurrent])->SetProperty(ASM_NAME_MAJOR_VERSION+j, (LPBYTE)&wVer, sizeof(WORD));
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }

            dwCurrent++;
        }
        if (!_dwRefCount) {
            SAFEDELETEARRAY(_ppAsmRefs);
        }
    }

    hr = S_OK;
        
Exit:        
    SAFEDELETEARRAY(rAssemblyRefTokens);
        
    return hr;
}

// class CProbingAssemblyEnum

CProbingAssemblyEnum::CProbingAssemblyEnum()
: _cRef(1)
, _dwFlags(CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT)
, _pe(peInvalid)
, _pName(NULL)
, _bHasPE(FALSE)
{
}

CProbingAssemblyEnum::~CProbingAssemblyEnum()
{
    SAFERELEASE(_pName);
}

/* static */
HRESULT 
CProbingAssemblyEnum::Create(
        DWORD dwMachineType,
        DWORD dwFlags,
        LPCWSTR pwzRefIdentity,
        ICLRProbingAssemblyEnum **ppProbingEnum)
{
    HRESULT hr = S_OK;
    CProbingAssemblyEnum *pProbingEnum = NULL;

    *ppProbingEnum = NULL;
    
    if (dwFlags != CLR_ASSEMBLY_IDENTITY_FLAGS_DEFAULT) {
        return E_INVALIDARG;
    }

    pProbingEnum = NEW(CProbingAssemblyEnum);
    if (!pProbingEnum) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pProbingEnum->Init(dwMachineType, dwFlags, pwzRefIdentity);
    if (FAILED(hr)) {
        SAFERELEASE(pProbingEnum);
        goto Exit;
    }

    *ppProbingEnum = pProbingEnum;
    
Exit:
    return hr;
}

STDMETHODIMP
CProbingAssemblyEnum::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;

    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_ICLRProbingAssemblyEnum))
    {
        *ppv = static_cast<ICLRProbingAssemblyEnum*> (this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

ULONG CProbingAssemblyEnum::AddRef() 
{
    return InterlockedIncrement(&_cRef);
}

ULONG CProbingAssemblyEnum::Release()
{
    ULONG lRes = InterlockedDecrement(&_cRef);

    if (lRes == 0) {
        delete this;
    }

    return lRes;
}

HRESULT CProbingAssemblyEnum::Get(
        DWORD dwIndex,
        __out_ecount_opt(*pcchBufferSize) LPWSTR  pwzBuffer,
        __inout DWORD   *pcchBufferSize)
{
    HRESULT hr = S_OK;

    _ASSERTE(_pName);

    if (!pcchBufferSize) {
        return E_INVALIDARG;
    }

    if (dwIndex > 0) {
        return HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
    }
    
    hr = _pName->GetDisplayName(pwzBuffer, pcchBufferSize, ASM_DISPLAYF_FULL);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = CanonicalizeIdentity(pwzBuffer);
    if (FAILED(hr)) {
        goto Exit;
    }   

Exit:
    return hr;
}

HRESULT CProbingAssemblyEnum::Init(
        DWORD dwMachineType,
        DWORD dwFlags,
        LPCWSTR pwzRefIdentity
        )
{
    HRESULT hr = S_OK;
    PEKIND  pe = peInvalid;
    DWORD   dwSize;

    _dwFlags = dwFlags;

    hr = CreateAssemblyNameObject(&_pName, pwzRefIdentity, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    // set version to 0 for simply named assemblies.
    if (!CAssemblyName::IsStronglyNamed(_pName)) {
        WORD wVer = 0;
        for (int i = ASM_NAME_MAJOR_VERSION; i < ASM_NAME_MAJOR_VERSION + 4; i++) {
            dwSize = sizeof(WORD);
            hr = _pName->GetProperty(i, &wVer, &dwSize);
            if (FAILED(hr)) {
                goto Exit;
            }

            // version is set, re-set to 0
            if (dwSize) {
                wVer = 0;
                hr = _pName->SetProperty(i, &wVer, sizeof(WORD));
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
        }
    }

    dwSize = sizeof(PEKIND);
    hr = _pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&pe, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pe != peInvalid) {
        _bHasPE = TRUE;
    }

Exit:
    return hr;
}

