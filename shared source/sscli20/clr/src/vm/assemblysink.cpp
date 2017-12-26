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
** Header:  AssemblySink.cpp
**
** Purpose: Implements AssemblySink, event objects that block
**          the current thread waiting for an asynchronous load
**          of an assembly to succeed.
**
** Date:  June 16, 1999
**
===========================================================*/

#include "common.h"

#include <stdlib.h>
#include "assemblysink.h"
#include "assemblyspec.hpp"
#include "corpriv.h"
#include "appdomain.inl"

AssemblySink::AssemblySink(AppDomain* pDomain) 
{
    WRAPPER_CONTRACT;
    m_Domain=pDomain->GetId();
    m_pSpec=NULL;
    m_CheckCodebase = FALSE;
}

void AssemblySink::Reset()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    m_CheckCodebase = FALSE;
    FusionSink::Reset();
}

ULONG AssemblySink::Release()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_TRIGGERS);}
        MODE_ANY;
        PRECONDITION(CheckPointer(this));
    } CONTRACTL_END;
    
    
    ULONG   cRef = InterlockedDecrement(&m_cRef);
    if (!cRef) {
        Reset();
        AssemblySink* ret = this;
        
        


        HRESULT hr = S_OK;
        SetupThreadNoThrow(&hr);
        {
            GCX_COOP();
        
            if(m_Domain.m_dwId) {
                AppDomainFromIDHolder AD(m_Domain, TRUE);
                if (!AD.IsUnloaded())
                     ret = (AssemblySink*) FastInterlockCompareExchangePointer((void**) &(AD->m_pAsyncPool),
                                                                          this,
                                                                          NULL);

            }
        }

        if(ret != NULL) 
            delete this;
    }
    return (cRef);
}



STDMETHODIMP AssemblySink::OnProgress(DWORD dwNotification,
                                      HRESULT hrNotification,
                                      LPCWSTR szNotification,
                                      DWORD dwProgress,
                                      DWORD dwProgressMax,
                                      LPVOID pvBindInfo,
                                      IUnknown* punk)
{
    STATIC_CONTRACT_NOTHROW;

    HRESULT hr = S_OK;

    switch(dwNotification) {

    case ASM_NOTIFICATION_BIND_INFO:
        FusionBindInfo          *pBindInfo;

        pBindInfo = (FusionBindInfo *)pvBindInfo;

        if (pBindInfo && pBindInfo->pNamePolicy && m_pSpec) {
            pBindInfo->pNamePolicy->AddRef();
            m_pSpec->SetNameAfterPolicy(pBindInfo->pNamePolicy);
        }
        break;

    default:
        break;
    }

    if (SUCCEEDED(hr))
        hr = FusionSink::OnProgress(dwNotification, hrNotification, szNotification, 
                                    dwProgress, dwProgressMax, pvBindInfo, punk);

    return hr;
}


HRESULT AssemblySink::Wait()
{
    STATIC_CONTRACT_NOTHROW;

    HRESULT hr = FusionSink::Wait();

    if (FAILED(hr)) {
        // If we get an exception then we will just release this sink. It may be the
        // case that the appdomain was terminated. Other exceptions will cause the
        // sink to be scavenged but this is ok. A new one will be generated for the
        // next bind.
        m_Domain.m_dwId = 0;
        // The AssemblySpec passed is stack allocated in some cases.
        // Remove reference to it to prevent AV in delayed fusion bind notifications.
        m_pSpec = NULL;
    }

    return hr;
}
