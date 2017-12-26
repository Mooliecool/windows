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
/*============================================================
**
** Header:  FusionSink.hpp
**
** Purpose: Implements FusionSink
**
** Date:  Oct 26, 1998
**
===========================================================*/
#ifndef _FUSIONSINK_H
#define _FUSIONSINK_H

#include <fusion.h>
#include <fusionpriv.h>
#include "corhlpr.h"

class FusionSink : public IAssemblyBindSink, public INativeImageEvaluate
{
public:
    
    FusionSink() :
        m_punk(NULL),
        m_pNIunk(NULL),
        m_pAbortUnk(NULL),
        m_pFusionLog(NULL),
        m_cRef(1),
        m_hEvent(NULL),
        m_LastResult(S_OK)
    {
        WRAPPER_CONTRACT;
    }

    virtual void Reset()
    {
        CONTRACTL
        {
            INSTANCE_CHECK;
            NOTHROW;
        }
        CONTRACTL_END;

        if(m_pAbortUnk) {
            m_pAbortUnk->Release();
            m_pAbortUnk = NULL;
        }

        if(m_punk) {
            m_punk->Release();
            m_punk = NULL;
        }

        if(m_pNIunk) {
            m_pNIunk->Release();
            m_pNIunk = NULL;
        }
        
        if(m_pFusionLog) {
            m_pFusionLog->Release();
            m_pFusionLog = NULL;
        }

        m_LastResult = S_OK;
    }

    ~FusionSink()
    {
        CONTRACTL
        {
            DESTRUCTOR_CHECK;
            NOTHROW;
        }
        CONTRACTL_END;
 
        if(m_hEvent) {
            delete m_hEvent;
            m_hEvent = NULL;
        }

        Reset();
    }

    HRESULT AssemblyResetEvent();
    HRESULT LastResult()
    {
        LEAF_CONTRACT;
        return m_LastResult;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppInterface);
    ULONG STDMETHODCALLTYPE AddRef(void); 
    ULONG STDMETHODCALLTYPE Release(void);
    
    STDMETHODIMP OnProgress(DWORD dwNotification,
                            HRESULT hrNotification,
                            LPCWSTR szNotification,
                            DWORD dwProgress,
                            DWORD dwProgressMax,
                            LPVOID pvBindInfo,
                            IUnknown* punk);

    // Wait on the event.
    virtual HRESULT Wait();

    STDMETHODIMP Evaluate(
        IAssembly *pILAssembly, 
        IAssembly *pNativeAssembly,
        BYTE * pbCachedData,
        DWORD dwDataSize);
    
    IUnknown*    m_punk;      // Getting an assembly
    IUnknown*    m_pNIunk;      // Getting an assembly    
    IUnknown*    m_pAbortUnk; // pUnk for aborting a bind
    IFusionBindLog *m_pFusionLog;

protected:
    HRESULT AssemblyCreateEvent();

    LONG        m_cRef;    // Ref count.
    Event      *m_hEvent;  // Event to block thread.
    HRESULT     m_LastResult; // Last notification result
};

#endif  // _FUSIONSINK_H
