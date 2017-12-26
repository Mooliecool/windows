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
#include "fusionpriv.h"
#include "cblist.h"

// CCodebaseEntry

CCodebaseEntry::CCodebaseEntry()
: _pwzCodebase(NULL)
, _dwFlags(0)
{
}

CCodebaseEntry::~CCodebaseEntry()
{
    SAFEDELETEARRAY(_pwzCodebase);
}

// CCodebaseList

CCodebaseList::CCodebaseList()
: _cRef(1)
{
    _dwSig = 0x204c4243; /* ' LBC' */
}

CCodebaseList::~CCodebaseList()
{
    RemoveAll();
}

STDMETHODIMP_(ULONG) CCodebaseList::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CCodebaseList::Release()
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }
    
    return ulRef;
}

HRESULT CCodebaseList::QueryInterface(REFIID iid, void **ppvObj)
{
    HRESULT hr = NOERROR;
    
    if (!ppvObj) 
        return E_POINTER;

    *ppvObj = NULL;
    
    if (iid == IID_IUnknown  || iid == IID_ICodebaseList) {
        *ppvObj = static_cast<ICodebaseList *>(this);
    } 
    else {
        hr = E_NOINTERFACE;
    }

    if (*ppvObj) {
        AddRef();
    }

    return hr;
}    


HRESULT CCodebaseList::AddCodebase(LPCWSTR wzCodebase, DWORD dwFlags)
{
    HRESULT                            hr = S_OK;
    DWORD                              dwLen;
    CCodebaseEntry                    *pEntry = NULL;
    
    if (!wzCodebase) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    pEntry = NEW(CCodebaseEntry);
    if (!pEntry) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    dwLen = lstrlenW(wzCodebase) + 1;

    pEntry->_pwzCodebase = NEW(WCHAR[dwLen]);
    if (!pEntry->_pwzCodebase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = StringCchCopy(pEntry->_pwzCodebase, dwLen, wzCodebase);
    if (FAILED(hr)) {
        goto Exit;
    }

    pEntry->_dwFlags = dwFlags;

    if (!_listCodebase.AddTail(pEntry)) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pEntry);
    }

    return hr;
}

HRESULT CCodebaseList::RemoveCodebase(DWORD dwIndex)
{
    HRESULT                              hr = S_OK;
    LISTNODE                             pos = NULL;
    LISTNODE                             oldpos = NULL;
    CCodebaseEntry                      *pEntry = NULL;
    DWORD                                dwNumCodebases = 0;
    DWORD                                i;
    

    dwNumCodebases = _listCodebase.GetCount();

    if (dwIndex > dwNumCodebases) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (!dwNumCodebases) {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto Exit;
    }
    
    pos = _listCodebase.GetHeadPosition();

    _ASSERTE(pos);

    for (i = 0; i < dwNumCodebases; i++) {
        oldpos = pos;
        pEntry = _listCodebase.GetNext(pos);
        _ASSERTE(pEntry);

        if (i == dwIndex) {
            SAFEDELETE(pEntry);
            _listCodebase.RemoveAt(oldpos);
            break;
        }
    }
        
Exit:
    return hr;
}

HRESULT CCodebaseList::GetCodebase(DWORD dwIndex, DWORD *pdwFlags, __out_ecount(*pcbCodebase) LPWSTR wzCodebase,
                                   DWORD *pcbCodebase)
{
    HRESULT                              hr = E_UNEXPECTED;
    LISTNODE                             pos = NULL;
    LISTNODE                             oldpos = NULL;
    CCodebaseEntry                      *pEntry = NULL;
    DWORD                                dwNumCodebases;
    DWORD                                i;
    DWORD                                dwCodebaseLen;

    dwNumCodebases = _listCodebase.GetCount();

    if (dwIndex > dwNumCodebases || !pcbCodebase || !pdwFlags) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (!dwNumCodebases) {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto Exit;
    }

    pos = _listCodebase.GetHeadPosition();

    _ASSERTE(pos);

    for (i = 0; i < dwNumCodebases; i++) {
        oldpos = pos;
        pEntry = _listCodebase.GetNext(pos);
        _ASSERTE(pEntry);

        if (i == dwIndex) {
            *pdwFlags = pEntry->_dwFlags;
            
            dwCodebaseLen = lstrlenW(pEntry->_pwzCodebase) + 1;

            if (!wzCodebase || (*pcbCodebase < dwCodebaseLen)) {
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
            else {
                hr = StringCchCopy(wzCodebase, *pcbCodebase, pEntry->_pwzCodebase);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            *pcbCodebase = dwCodebaseLen;
            break;
        }
    }

Exit:
    return hr;
}


HRESULT CCodebaseList::RemoveAll()
{
    LISTNODE                      pos = NULL;
    CCodebaseEntry               *pEntry = NULL;

    pos = _listCodebase.GetHeadPosition();

    while (pos) {
        pEntry = _listCodebase.GetNext(pos);
        _ASSERTE(pEntry);

        SAFEDELETE(pEntry);
    }

    _listCodebase.RemoveAll();

    return S_OK;
}

HRESULT CCodebaseList::GetCount(DWORD *pdwCount)
{
    HRESULT                           hr = S_OK;

    if (!pdwCount) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *pdwCount = _listCodebase.GetCount();

Exit:
    return hr;
}

