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

#include "common.h"

#include "hostexecutioncontext.h"
#include "corhost.h"
#include "security.h"

IHostSecurityContext *HostExecutionContextManager::m_pRestrictedHostContext = NULL;

// initialize HostRestrictedContext
void HostExecutionContextManager::InitializeRestrictedContext()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
	
	_ASSERTE(m_pRestrictedHostContext == NULL);
	
	IHostSecurityManager *pSM = CorHost2::GetHostSecurityManager();
	if (pSM)
	{
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
		pSM->GetSecurityContext(eRestrictedContext, &m_pRestrictedHostContext);
        END_SO_TOLERANT_CODE_CALLING_HOST;
	}	
}
// notify the Host to SetRestrictedContext
void HostExecutionContextManager::SetHostRestrictedContext()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
	
	if(m_pRestrictedHostContext != NULL)
	{		
        	IHostSecurityManager *pSM = CorHost2::GetHostSecurityManager();
		if (pSM)
		{
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
			pSM->SetSecurityContext(eRestrictedContext, m_pRestrictedHostContext);
            END_SO_TOLERANT_CODE_CALLING_HOST;
		}
	}
}

FCIMPL0(FC_BOOL_RET, HostExecutionContextManager::HostPresent)
{
    STATIC_CONTRACT_SO_TOLERANT;
    WRAPPER_CONTRACT;
    
    FC_RETURN_BOOL(CorHost2::GetHostSecurityManager() != NULL);
}
FCIMPLEND

FCIMPL1(HRESULT, HostExecutionContextManager::ReleaseSecurityContext, LPVOID handle)
{
	CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
	 PRECONDITION(CheckPointer(handle));
    	} CONTRACTL_END;


	HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();
	
	IHostSecurityManager *pSM = CorHost2::GetHostSecurityManager();
	if (pSM)
	{		
		// get the IUnknown pointer from handle
		IHostSecurityContext* pSecurityContext = (IHostSecurityContext*)handle;
		// null out the IUnknown pointer in the handle
		//hTokenSAFE->SetHandle((void*)NULL);
		// release the IUnknown pointer if it is non null
		if (pSecurityContext != NULL)
		{
			pSecurityContext->Release();			
		}
	}
	
	HELPER_METHOD_FRAME_END();
	return S_OK;

}
FCIMPLEND

FCIMPL1(HRESULT, HostExecutionContextManager::CaptureSecurityContext, SafeHandle* hTokenUNSAFE)
{
	CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
	 PRECONDITION(CheckPointer(hTokenUNSAFE));
    	} CONTRACTL_END;
	
	IHostSecurityContext* pCurrentHostSecurityContext = NULL;
	IHostSecurityContext* pCapturedSecurityContext = NULL;

	HRESULT hr = S_OK;
	SAFEHANDLE hTokenSAFE = (SAFEHANDLE) hTokenUNSAFE;
	HELPER_METHOD_FRAME_BEGIN_RET_1(hTokenSAFE);
	
	IHostSecurityManager *pSM = CorHost2::GetHostSecurityManager();
	if (pSM)
	{
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
		hr = pSM->GetSecurityContext(eCurrentContext, &pCurrentHostSecurityContext);
        END_SO_TOLERANT_CODE_CALLING_HOST;
		if (hr == S_OK)
		{
			if(pCurrentHostSecurityContext != NULL)
			{				
                BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
				hr = pCurrentHostSecurityContext->Capture(&pCapturedSecurityContext);				
                END_SO_TOLERANT_CODE_CALLING_HOST;
				hTokenSAFE->SetHandle((void*)pCapturedSecurityContext);
				SafeRelease(pCurrentHostSecurityContext);
			}			
		}
	}
	
	if (FAILED(hr))
		COMPlusThrowHR(hr);	
	
	HELPER_METHOD_FRAME_END();
	return hr;

}
FCIMPLEND

FCIMPL2(HRESULT, HostExecutionContextManager::CloneSecurityContext, SafeHandle* hTokenUNSAFE, SafeHandle* hTokenClonedUNSAFE)
{
	CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
	PRECONDITION(CheckPointer(hTokenUNSAFE));
	PRECONDITION(CheckPointer(hTokenClonedUNSAFE));
    	} CONTRACTL_END;

	SAFEHANDLE hTokenClonedSAFE = (SAFEHANDLE) hTokenClonedUNSAFE;
	SAFEHANDLE hTokenSAFE = (SAFEHANDLE)hTokenUNSAFE;
	
	HELPER_METHOD_FRAME_BEGIN_RET_2(hTokenSAFE, hTokenClonedSAFE);

	IHostSecurityManager *pSM = CorHost2::GetHostSecurityManager();
	if (pSM)
	{		
		IHostSecurityContext* pSecurityContext = (IHostSecurityContext*)hTokenSAFE->GetHandle();
		if (pSecurityContext != NULL)
		{
			pSecurityContext->AddRef();
			hTokenClonedSAFE->SetHandle((void*)pSecurityContext);
		}
	}
	
	HELPER_METHOD_FRAME_END();
	return S_OK;
}
FCIMPLEND

FCIMPL3(HRESULT, HostExecutionContextManager::SetSecurityContext, SafeHandle* hTokenUNSAFE, CLR_BOOL fReturnPrevious, SafeHandle* hTokenPreviousUNSAFE)
{
	CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
	 PRECONDITION(CheckPointer(hTokenUNSAFE));
    	} CONTRACTL_END;

	HRESULT hr = S_OK;
	
	SAFEHANDLE hTokenPreviousSAFE = (SAFEHANDLE) hTokenPreviousUNSAFE;
	SAFEHANDLE hTokenSAFE = (SAFEHANDLE) hTokenUNSAFE;
	
	HELPER_METHOD_FRAME_BEGIN_RET_2(hTokenSAFE, hTokenPreviousSAFE);

	IHostSecurityManager *pSM = CorHost2::GetHostSecurityManager();
	if (pSM)
	{
		if (fReturnPrevious)
		{
			IHostSecurityContext* pPreviousHostSecurityContext = NULL;
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
			hr = pSM->GetSecurityContext(eCurrentContext, &pPreviousHostSecurityContext);
            END_SO_TOLERANT_CODE_CALLING_HOST;
			if (FAILED(hr))
				COMPlusThrowHR(hr);
			// store the previous host context in the safe handle
			hTokenPreviousSAFE->SetHandle((void*)pPreviousHostSecurityContext);
		}
		
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
		hr = pSM->SetSecurityContext(eCurrentContext, (IHostSecurityContext*)hTokenSAFE->GetHandle());
        END_SO_TOLERANT_CODE_CALLING_HOST;
		if (FAILED(hr))
			COMPlusThrowHR(hr);
	}

	HELPER_METHOD_FRAME_END();
	return hr;
}
FCIMPLEND

