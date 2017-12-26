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

#include "asmreflist.h"

/* static */
HRESULT CAssemblyReferenceList::Create(
        LPCWSTR     *ppwzAsmRefs,
        DWORD       dwNumOfRefs,
        ICLRAssemblyReferenceList **ppRefList)
{
    HRESULT hr = S_OK;
    CAssemblyReferenceList *pRefList = NULL;

    _ASSERTE(ppRefList);

    *ppRefList = NULL;

    pRefList = NEW(CAssemblyReferenceList());
    if (!pRefList) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pRefList->Init(ppwzAsmRefs, dwNumOfRefs);
    if (FAILED(hr)) {
        SAFERELEASE(pRefList);
        goto Exit;
    }

    *ppRefList = pRefList;

Exit:
    return hr;
}

CAssemblyReferenceList::CAssemblyReferenceList()
:_cRef(1)
,_dwAsmCount(0)
,_ppAsmList(NULL)
{
    _dwSig = 0x4C524143; // "LRAC"
}

CAssemblyReferenceList::~CAssemblyReferenceList()
{
    DWORD i;

    if (_dwAsmCount && _ppAsmList) {

        for (i = 0; i < _dwAsmCount; i++ ) {
            SAFERELEASE(_ppAsmList[i]);
        }

        SAFEDELETEARRAY(_ppAsmList);
    }
}

HRESULT CAssemblyReferenceList::Init(LPCWSTR *ppwzAsmRefs, DWORD dwNumOfRefs)
{
    HRESULT hr = S_OK;

    if (!dwNumOfRefs) {
        // Nothing to do here.
        goto Exit;
    }
  
    if (dwNumOfRefs) {
        _ppAsmList = NEW(LPASSEMBLYNAME[dwNumOfRefs]);
        if (!_ppAsmList) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        memset(_ppAsmList, 0, sizeof(IAssemblyName *) * dwNumOfRefs);

        _dwAsmCount = dwNumOfRefs;

        for (DWORD i = 0; i < _dwAsmCount; i++) {
            hr = CreateAssemblyNameObject(&(_ppAsmList[i]), ppwzAsmRefs[i], CANOF_PARSE_DISPLAY_NAME, NULL);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

Exit:
    return hr;
}

ULONG CAssemblyReferenceList::AddRef() 
{
    return InterlockedIncrement(&_cRef);
}

ULONG CAssemblyReferenceList::Release()
{
    ULONG lRet = InterlockedDecrement(&_cRef);
    if (!lRet) {
        delete this;
    }
    return lRet;
}

STDMETHODIMP
CAssemblyReferenceList::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;

    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_ICLRAssemblyReferenceList))
    {
        *ppv = static_cast<ICLRAssemblyReferenceList *> (this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

HRESULT CAssemblyReferenceList::IsStringAssemblyReferenceInList(LPCWSTR pwzAssemblyName)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;

    _ASSERTE(pwzAssemblyName);

    if (!pwzAssemblyName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (_dwAsmCount == 0) {
        hr = S_FALSE;
        goto Exit;
    }

    hr = CreateAssemblyNameObject(&pName, pwzAssemblyName, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = IsAssemblyReferenceInList(pName);

Exit:
    SAFERELEASE(pName);
    return hr;
}

HRESULT CAssemblyReferenceList::IsAssemblyReferenceInList(IUnknown *pUnk)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;

    _ASSERTE(pUnk);
    
    if (!pUnk) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (_dwAsmCount == 0) {
        hr = S_FALSE;
        goto Exit;
    }

    _ASSERTE(_ppAsmList);

    hr = pUnk->QueryInterface(IID_IAssemblyName, (void **)&pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    for (DWORD i=0; i < _dwAsmCount; i++) {
        hr = _ppAsmList[i]->IsEqual(pName, ASM_CMPF_DEFAULT);
        if (FAILED(hr) || hr == S_OK) {
            // fail or match is final
            goto Exit;
        }
    }

    hr = S_FALSE;

Exit:
    SAFERELEASE(pName);
    return hr;
}
