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
#include "simplesink.h"

CSimpleFusionBindSink::CSimpleFusionBindSink()
: _cRef(1)
, _hEvent(NULL)
, _ppInterface(NULL)
, _pBind(NULL)
, _pdbglog(NULL)
{
}

CSimpleFusionBindSink::~CSimpleFusionBindSink()
{
    SAFERELEASE(_pBind);
    SAFERELEASE(_pdbglog);

    if (_hEvent) {
        SAFEDELETE(_hEvent);
    }
}

/* static */
HRESULT CSimpleFusionBindSink::Create(void** ppInterface, CSimpleFusionBindSink **ppSink)
{
    HRESULT hr = S_OK;
    CSimpleFusionBindSink *pSink = NULL;

    pSink = NEW(CSimpleFusionBindSink());
    if (!pSink) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pSink->Init(ppInterface);
    if (FAILED(hr)) {
        SAFERELEASE(pSink);
        goto Exit;
    }

    *ppSink = pSink;

Exit:
    return hr;
}

STDMETHODIMP CSimpleFusionBindSink::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;

    *ppv = NULL;

    if (riid == IID_IUnknown)
        *ppv = (IUnknown*) (IAssemblyBindSink*) this;
    else if (riid == IID_IAssemblyBindSink)   
        *ppv = (IAssemblyBindSink*)this;
    if (*ppv == NULL)
        return E_NOINTERFACE;
    AddRef();   
    return S_OK;
}

ULONG CSimpleFusionBindSink::AddRef()
{
    return (InterlockedIncrement(&_cRef));
}

ULONG CSimpleFusionBindSink::Release()
{
    ULONG   cRef = InterlockedDecrement(&_cRef);
    if (!cRef) {
        delete this;
    }
    return (cRef);
}

HRESULT CSimpleFusionBindSink::Init(void **ppInterface)
{
    HRESULT hr = S_OK;

    _ASSERTE(ppInterface);

    _ppInterface = ppInterface;

    if(_hEvent == NULL) {
        // Initialize the event to require manual reset
        // and to initially signaled.
        _hEvent = NEW(Event());
        if (!_hEvent) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        EX_TRY {
            _hEvent->CreateManualEvent(TRUE);
        }
        EX_CATCH
        {
            hr = GET_EXCEPTION()->GetHR();
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    
Exit:
    return hr;
}

HRESULT CSimpleFusionBindSink::Reset() 
{
    HRESULT hr = S_OK;

    SAFERELEASE(_pBind);

    _ASSERTE(_hEvent);

    if (!_hEvent->Reset()) {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}
    
HRESULT CSimpleFusionBindSink::Wait()
{
    HRESULT hr = S_OK;
    DWORD   dwReturn;

    dwReturn = _hEvent->Wait(INFINITE, FALSE);

    return hr;
}

STDMETHODIMP CSimpleFusionBindSink::OnProgress(DWORD dwNotification,
                                    HRESULT hrNotification,
                                    LPCWSTR szNotification,
                                    DWORD dwProgress,
                                    DWORD dwProgressMax,
                                    LPVOID pvBindInfo,
                                    IUnknown* punk)
{
    HRESULT hr = S_OK;
    switch(dwNotification) {
        case ASM_NOTIFICATION_START:
            if(punk)
                hr = punk->QueryInterface(IID_IAssemblyBinding, (void**)&_pBind);
            break;       

        case ASM_NOTIFICATION_DONE:
            _hrResult = hrNotification;
            SAFERELEASE(_pBind);

            if(punk && SUCCEEDED(hrNotification)){
                hr = punk->QueryInterface(IID_IAssembly, _ppInterface);
            }
            _hEvent->Set();
            break;

        case ASM_NOTIFICATION_ATTEMPT_NEXT_CODEBASE:
            break;

        case ASM_NOTIFICATION_BIND_INFO:
            FusionBindInfo      *pBindInfo;

            pBindInfo = (FusionBindInfo *)pvBindInfo;

            if (pBindInfo) {
                _pdbglog = pBindInfo->pdbglog;
                if (_pdbglog) {
                    _pdbglog->AddRef();
                }
            }

            break;
        default:
            break;
    }
    
    return hr;
}

HRESULT CSimpleFusionBindSink::GetBindLog(IFusionBindLog **ppdbglog)
{
    HRESULT                                      hr = S_OK;

    if (_pdbglog) {
        *ppdbglog = _pdbglog;
        _pdbglog->AddRef();
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return hr;
}
