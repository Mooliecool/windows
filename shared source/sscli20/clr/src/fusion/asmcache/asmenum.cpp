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
#include "asmenum.h"
#include "naming.h"
#include <shlwapi.h>
#include "enum.h"


// ---------------------------------------------------------------------------
// CreateAssemblyEnum
// ---------------------------------------------------------------------------
STDAPI CreateAssemblyEnum(IAssemblyEnum** ppEnum, IUnknown *pUnkAppCtx,
    IAssemblyName *pName, DWORD dwFlags, LPVOID pvReserved)    
{
    HRESULT                          hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;
    
    IApplicationContext             *pAppCtx = NULL;
    CAssemblyEnum                   *pEnum = NULL;

    if(!ppEnum) {
        hr = E_INVALIDARG;
        goto exit;
    }

    if (pUnkAppCtx) {
        hr = pUnkAppCtx->QueryInterface(IID_IApplicationContext, (void **)&pAppCtx);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    pEnum = NEW(CAssemblyEnum);
    if (!pEnum)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    if (FAILED(hr = pEnum->Init(pAppCtx, pName, dwFlags)))
    {
    	SAFERELEASE(pEnum);			
    }
    else
    {
    	*ppEnum = (IAssemblyEnum*) pEnum;
    }

exit:
    SAFERELEASE(pAppCtx);

    END_ENTRYPOINT_NOTHROW;
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyEnum ctor
// ---------------------------------------------------------------------------
CAssemblyEnum::CAssemblyEnum()
{
    _dwSig = 0x4d554e45; /* 'MUNE' */
    _cRef = 1;
    _pAppCtx = NULL;
    _pName = NULL;
    _dwFlags = 0;
}


// ---------------------------------------------------------------------------
// CAssemblyEnum dtor
// ---------------------------------------------------------------------------
CAssemblyEnum::~CAssemblyEnum()
{
    ReleaseAssemblyList();

    SAFERELEASE(_pAppCtx);
    SAFERELEASE(_pName);
}


// ---------------------------------------------------------------------------
// CAssemblyEnum::Init
// ---------------------------------------------------------------------------
HRESULT CAssemblyEnum::Init(IApplicationContext *pAppCtx, IAssemblyName *pName, DWORD dwFlags)
{
    HRESULT hr = S_OK;

    _pAppCtx = pAppCtx;
    if (_pAppCtx) {
        _pAppCtx->AddRef();
    }

    _pName = pName;
    if (_pName) {
        _pName->AddRef();
    }

    _dwFlags = dwFlags;

    if (IsZapType(_dwFlags)) {
        hr = PopulateZapAssemblyList();
        if (FAILED(hr)) {
            goto Exit;
        }
        
        _pos = _listAsms.GetHeadPosition();
    }
    else {
        hr = PopulateAssemblyList();
        if (FAILED(hr)) {
            goto Exit;
        }

        _pos = _listAsms.GetHeadPosition();
    }

Exit:
    return hr;
}

HRESULT CAssemblyEnum::PopulateZapAssemblyList()
{
    HRESULT         hr = S_FALSE;


    return hr;
}

HRESULT CAssemblyEnum::PopulateAssemblyList()
{
    HRESULT         hr = S_OK;
    DWORD           dwCmpMask = 0;
    DWORD           dwQueryMask = 0;
    BOOL            fIsPartial = FALSE;
    CCache         *pCache = NULL;
    CTransCache    *pTransCache = NULL;
    CTransCache    *pTC = NULL;
    CEnumCache     *pEnumR = NULL;
    DWORD           dwCacheId = 0; // the transcache id
    IAssemblyName  *pName = NULL;

    // If no name is passed in, create a default (blank) copy.
    if (!_pName)
    { 
        if (FAILED(hr = CreateAssemblyNameObject(&_pName, NULL, NULL, NULL)))
            goto exit;            
    }

    if (FAILED(hr = CCache::Create(&pCache, _pAppCtx)))
        goto exit;
    
    PREFIX_ASSUME(pCache != NULL);

    // Create a transcache entry from the name.
    if (FAILED(hr = pCache->TransCacheEntryFromName(_pName, _dwFlags, &pTransCache)))
        goto exit;

    if (FAILED(hr = pCache->ResolveCacheIndex(_pName, _dwFlags, &dwCacheId))) 
        goto exit;

    // Get the name comparison mask.
    fIsPartial = CAssemblyName::IsPartial(_pName, &dwCmpMask);    

    // Convert to query mask.
    dwQueryMask = pTransCache->MapNameMaskToCacheMask(dwCmpMask);

    // Allocate an enumerator.
    pEnumR = NEW(CEnumCache(FALSE, pCache->GetCustomPath()));
    if(!pEnumR) {
         hr = E_OUTOFMEMORY;
         goto exit;
    }
        
    // Initialize the enumerator on the transcache entry.
    if (FAILED(hr = pEnumR->Init(pTransCache,  dwQueryMask)))
        goto exit;
   
    if (hr == S_FALSE) {
        // nothing in the cache
        goto exit;
    }
    
    hr = S_OK;

    if (FAILED(hr = pCache->CreateTransCacheEntry(dwCacheId, &pTC)))
        goto exit;
    
    while (hr == S_OK) {
        if (FAILED(hr = pEnumR->GetNextRecord(pTC)))
            goto exit;

        if (hr == S_FALSE) {
            hr = S_OK;
            break;
        }

        if (FAILED(hr = CCache::NameFromTransCacheEntry(pTC, &pName)))
            goto exit;

        if (!_listAsms.AddTail(pName)) {
            hr = E_OUTOFMEMORY;
            goto exit;
        }

        pName = NULL;
        // release stuff allocated by GetNextRecord
        pTC->CleanInfo(TRUE);
    }
    
exit:
    SAFERELEASE(pCache);
    SAFERELEASE(pTransCache);
    SAFEDELETE(pEnumR);
    SAFERELEASE(pTC);
    SAFERELEASE(pName);

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyEnum::GetNextAssembly
// ---------------------------------------------------------------------------
STDMETHODIMP 
CAssemblyEnum::GetNextAssembly(LPVOID pvReserved,
    IAssemblyName** ppName, DWORD dwFlags)
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;


    if(!ppName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (!_pos) {
        hr = S_FALSE;
        goto Exit;
    }

    *ppName = _listAsms.GetNext(_pos);
    _ASSERTE(*ppName);
    (*ppName)->AddRef();

Exit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

void CAssemblyEnum::ReleaseAssemblyList()
{
    LISTNODE pos = _listAsms.GetHeadPosition();
    IAssemblyName *pName = NULL;

    while(pos) {
        pName = _listAsms.GetNext(pos);
        _ASSERTE(pName);
        SAFERELEASE(pName);
    }

    _listAsms.RemoveAll();

    _pos = NULL;
}

// ---------------------------------------------------------------------------
// CAssemblyEnum::Reset
// ---------------------------------------------------------------------------
STDMETHODIMP 
CAssemblyEnum::Reset(void)
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return E_NOTIMPL;
}

// ---------------------------------------------------------------------------
// CAssemblyEnum::Clone
// ---------------------------------------------------------------------------
STDMETHODIMP 
CAssemblyEnum::Clone(IAssemblyEnum** ppEnum)
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return E_NOTIMPL;
}

// IUnknown Boilerplate

// ---------------------------------------------------------------------------
// CAssemblyEnum::QI
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyEnum::QueryInterface(REFIID riid, void** ppvObj)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    if (!ppvObj) 
        IfFailGo(E_POINTER);
    
    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IAssemblyEnum)
       )
    {
        *ppvObj = static_cast<IAssemblyEnum*> (this);
        AddRef();
    }
    else
    {
        *ppvObj = NULL;
        IfFailGo(E_NOINTERFACE);
    }

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
    
}

// ---------------------------------------------------------------------------
// CAssemblyEnum::AddRef
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyEnum::AddRef()
{
    ULONG refCount = 0;
    BEGIN_ENTRYPOINT_VOIDRET;
    refCount = InterlockedIncrement (&_cRef);
    END_ENTRYPOINT_VOIDRET;
    return refCount;
}

// ---------------------------------------------------------------------------
// CAssemblyEnum::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyEnum::Release()
{
    BEGIN_CLEANUP_ENTRYPOINT;
    
    ULONG lRet = 0;
    
    lRet = InterlockedDecrement (&_cRef);
    if (!lRet)
        delete this;

    END_CLEANUP_ENTRYPOINT;
    return lRet;
}

