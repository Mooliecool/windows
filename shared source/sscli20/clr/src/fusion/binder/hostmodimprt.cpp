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

#include "hostmodimprt.h"
#include "naming.h"
#include "util.h"
#include "hostasm.h"

STDAPI CreateHostAssemblyModuleImport(
    ModuleBindInfo    *pBindInfo,
    IStream           *pStream,
    IStream           *pDebugStream,
    DWORD              dwModuleId,
    IHostAssemblyModuleImport **ppImport)
{
    HRESULT hr = S_OK;
    CHostAssemblyModuleImport *pHostModImport = NULL;

    _ASSERTE(ppImport);
    *ppImport = NULL;

    pHostModImport = NEW(CHostAssemblyModuleImport);
    if (!pHostModImport) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pHostModImport->Init(pBindInfo, pStream, pDebugStream, dwModuleId); 
    if (FAILED(hr)) {
        SAFERELEASE(pHostModImport);
        goto Exit;
    }

    *ppImport = pHostModImport;

Exit:
    return hr;
}

CHostAssemblyModuleImport::CHostAssemblyModuleImport()
:_cRef(1)
,_pNameDef(NULL)
,_pStream(NULL)
,_pDebugStream(NULL)
,_dwModuleId(0)
,_bAskedHost(FALSE)
{
   _dwSig = 0x4D414843; // 'MAHC'
   memset(&_bindInfo, 0, sizeof(ModuleBindInfo));
}

CHostAssemblyModuleImport::~CHostAssemblyModuleImport()
{
    SAFERELEASE(_pNameDef);
    SAFERELEASE(_pStream);
    SAFERELEASE(_pDebugStream);
    SAFEDELETEARRAY(_bindInfo.lpAssemblyIdentity);
    SAFEDELETEARRAY(_bindInfo.lpModuleName);
}

HRESULT CHostAssemblyModuleImport::Init( 
                ModuleBindInfo   *pBindInfo,
                IStream           *pStream,
                IStream           *pDebugStream,
                DWORD              dwModuleId)
{
    HRESULT hr = S_OK;

    if (!pBindInfo) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    _bindInfo.dwAppDomainId = pBindInfo->dwAppDomainId;

    _bindInfo.lpAssemblyIdentity = WSTRDupDynamic(pBindInfo->lpAssemblyIdentity);
    if (!_bindInfo.lpAssemblyIdentity) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    _bindInfo.lpModuleName = WSTRDupDynamic(pBindInfo->lpModuleName);
    if (!_bindInfo.lpModuleName) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    hr = CreateAssemblyNameObject(&_pNameDef, _bindInfo.lpAssemblyIdentity, CANOF_PARSE_DISPLAY_NAME, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    _pStream = pStream;
    if (_pStream) {
        _pStream->AddRef();
    }

    _pDebugStream = pDebugStream;
    if (_pDebugStream) {
        _pDebugStream->AddRef();
    }

    _dwModuleId = dwModuleId;

Exit:
    return hr;
}

STDMETHODIMP
CHostAssemblyModuleImport::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) {
        return E_POINTER;
    }

    *ppv = NULL;

    if (riid==IID_IHostAssemblyModuleImport || riid == IID_IUnknown) {
        *ppv = (IHostAssemblyModuleImport *)this;
        AddRef();
    }
    else {
        return E_NOINTERFACE;
    }

    return S_OK;
} 

STDMETHODIMP_(ULONG)
CHostAssemblyModuleImport::AddRef()
{
    return InterlockedIncrement((LONG *)&_cRef);
}

STDMETHODIMP_(ULONG)
CHostAssemblyModuleImport::Release()
{
    ULONG lRef = InterlockedDecrement((LONG *)&_cRef);
    if (!lRef) {
        delete this;
    }

    return lRef;
}

STDMETHODIMP
CHostAssemblyModuleImport::GetModuleName(
        __out_ecount_opt(*pccModuleName) LPOLESTR szModuleName, 
        __inout LPDWORD pccModuleName)
{
    HRESULT hr = S_OK;
    DWORD ccModuleName = lstrlenW(_bindInfo.lpModuleName) + 1;

    if (!pccModuleName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (*pccModuleName < ccModuleName) {
        *pccModuleName = ccModuleName;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    if (!szModuleName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = StringCchCopyN(szModuleName, *pccModuleName, _bindInfo.lpModuleName, ccModuleName-1);
    if (FAILED(hr)) {
        goto Exit;
    }

    *pccModuleName = ccModuleName;

Exit:
    return hr;
}

STDMETHODIMP
CHostAssemblyModuleImport::GetModuleStream(
        IStream **ppStreamModule)
{
    HRESULT hr = S_OK;
    DWORD   dwModuleId;
    IStream *pDebugStream = NULL;

    if (!ppStreamModule) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = GetModuleId(&dwModuleId);
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppStreamModule = NULL;

    *ppStreamModule = (IStream*)InterlockedExchangePointer((LPVOID *)&_pStream, 0);
    if (!*ppStreamModule) {
        if (! g_pHostAssemblyStore ) {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            goto Exit;
        }

        REMOVE_STACK_GUARD;
        hr = g_pHostAssemblyStore->ProvideModule(
                            &_bindInfo,
                            &dwModuleId,
                            ppStreamModule,
                            &pDebugStream);

        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (!*ppStreamModule) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

Exit:
    SAFERELEASE(pDebugStream);
    return hr;

}

STDMETHODIMP
CHostAssemblyModuleImport::GetModuleDebugStream(
        IStream **ppDebugStream)
{
    HRESULT hr = S_OK;
    DWORD   dwModuleId;
    IStream *pModuleStream = NULL;

    if (!ppDebugStream) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = GetModuleId(&dwModuleId);
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppDebugStream = NULL;

    *ppDebugStream= (IStream*)InterlockedExchangePointer((LPVOID *)&_pDebugStream, 0);
    if (!*ppDebugStream) {
        // we must have been told to release the stream. 
        // Have to ask the assembly store for it again.
        if (! g_pHostAssemblyStore ) {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            goto Exit;
        }

        REMOVE_STACK_GUARD;
        hr = g_pHostAssemblyStore->ProvideModule(
                            &_bindInfo,
                            &dwModuleId,
                            &pModuleStream,
                            ppDebugStream);

        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (!*ppDebugStream) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

Exit:
    SAFERELEASE(pModuleStream);
    return hr;
}

STDMETHODIMP 
CHostAssemblyModuleImport::GetModuleId(
        /* out */  DWORD *pdwModuleId)
{
    HRESULT hr = S_OK;
    DWORD   dwModuleId;
    IStream *pStream = NULL;
    IStream *pDebugStream = NULL;

    if (!pdwModuleId) {
        return E_INVALIDARG;
    }
    if (!_bAskedHost) { 
        REMOVE_STACK_GUARD;
        hr = g_pHostAssemblyStore->ProvideModule(
                            &_bindInfo,
                            &dwModuleId,
                            &pStream,
                            &pDebugStream);
        if (FAILED(hr)) {
            goto Exit;
        }
        if (pDebugStream) { 
            if (InterlockedCompareExchangePointer((void **)&_pDebugStream, pDebugStream, NULL)) {
                SAFERELEASE(pDebugStream);
            }
        }
        if (InterlockedCompareExchangePointer((void **)&_pStream, pStream, NULL)) {
            SAFERELEASE(pStream);
        }
        _dwModuleId = dwModuleId;
        _bAskedHost = TRUE;
    }

    *pdwModuleId = _dwModuleId;

Exit:
    return hr;
}
