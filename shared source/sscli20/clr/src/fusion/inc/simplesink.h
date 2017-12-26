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

#ifndef _SIMPLE_SINK_H_
#define _SIMPLE_SINK_H_

#include <fusionp.h>

class CSimpleFusionBindSink: public IAssemblyBindSink
{
public:
    CSimpleFusionBindSink();
    ~CSimpleFusionBindSink();

    HRESULT Init(void** ppInterface);

    static HRESULT Create(void **ppInterface, CSimpleFusionBindSink** ppSink);

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    STDMETHODIMP OnProgress(DWORD dwNotification,
                HRESULT hrNotification,
                LPCWSTR szNotification,
                DWORD dwProgress,
                DWORD dwProgressMax,
                LPVOID pvBindInfo,
                IUnknown *pUnk);

    HRESULT Reset();

    virtual HRESULT Wait();

    HRESULT LastResult() {return _hrResult;}
    HRESULT GetBindLog(IFusionBindLog **ppdbglog);

private:
    LONG        _cRef;
    Event      *_hEvent;
    HRESULT     _hrResult;

    void      **_ppInterface;
    IUnknown*   _pBind;
    IFusionBindLog *_pdbglog;
};

#endif
