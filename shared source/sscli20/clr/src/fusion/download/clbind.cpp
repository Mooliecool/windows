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
#include "adl.h"
#include "clbind.h"

CClientBinding::CClientBinding(CAssemblyDownload *pad,
                               IAssemblyBindSink *pbindsink)
: _cRef(1)
, _pbindsink(pbindsink)
, _padl(pad)
, _cLocks(0)
, _bPendingDelete(FALSE)
, _bAborted(FALSE)
{
    _dwSig = 0x444e4243; /* 'DNBC' */
    if (_padl) {
        _padl->AddRef();
    }

    if (_pbindsink) {
        _pbindsink->AddRef();
    }
}

CClientBinding::~CClientBinding()
{
    if (_padl) {
        _padl->Release();
    }

    if (_pbindsink) {
        _pbindsink->Release();
    }
}

STDMETHODIMP CClientBinding::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT            hr = S_OK;

    if (!ppv) 
        return E_POINTER;

    *ppv = NULL;

    if (riid==IID_IUnknown || riid==IID_IAssemblyBinding) {
        *ppv = static_cast<IAssemblyBinding *>(this);
    }
    else {
        hr = E_NOINTERFACE;
    }

    if (*ppv) {
        AddRef();
    }

    return hr;
}  

STDMETHODIMP_(ULONG) CClientBinding::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CClientBinding::Release()
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }
    
    return ulRef;
}

IAssemblyBindSink *CClientBinding::GetBindSink()
{
    return _pbindsink;
}

STDMETHODIMP CClientBinding::Control(HRESULT hrControl)
{
    HRESULT                           hr = S_OK;

    switch (hrControl) {
        case E_ABORT:
            _bAborted = TRUE;
            hr = _padl->ClientAbort(this);
            break;
        default:
            hr = E_INVALIDARG;
    }

    return hr;
}

STDMETHODIMP CClientBinding::DoDefaultUI(HWND hWnd, DWORD dwFlags)
{
    return E_NOTIMPL;
}

void CClientBinding::SetPendingDelete(BOOL bPending)
{
    _bPendingDelete = bPending;
}

int CClientBinding::LockCount()
{
    return _cLocks;
}

int CClientBinding::Lock()
{
    return InterlockedIncrement((LONG *)&_cLocks);
}

int CClientBinding::UnLock()
{
    return InterlockedDecrement((LONG *)&_cLocks);
}

BOOL CClientBinding::IsPendingDelete()
{
    return _bPendingDelete;
}

HRESULT CClientBinding::SwitchDownloader(CAssemblyDownload *padl)
{
    HRESULT                               hr = S_OK;

    if (!padl) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    SAFERELEASE(_padl);
    _padl = padl;
    _padl->AddRef();

Exit:
    return hr;
}

HRESULT CClientBinding::CallStartBinding()
{
    HRESULT                                    hr = S_OK;

    _ASSERTE(_pbindsink);

    _pbindsink->OnProgress(ASM_NOTIFICATION_START, S_OK, NULL, 0, 0, NULL, this);

    return hr;
}

BOOL CClientBinding::IsAborted()
{
    return _bAborted;
}
      
