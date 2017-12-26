/****************************** Module Header ******************************\
* Module Name:  ATLCOMService.cpp
* Project:      ATLCOMService
* Copyright (c) Microsoft Corporation.
* 
* Acitve Template Library (ATL) is designed to simplify the process of 
* creating efficient, flexible, lightweight COM components. ATLCOMService 
* provides the our-of-process server objects that run in a Windows Service. 
* 
* ATLCOMService.cpp implements WinMain and defines the service module.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include "ATLCOMService_i.h"
#pragma endregion


class CATLCOMServiceModule : public CAtlServiceModuleT< CATLCOMServiceModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_ATLCOMServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLCOMSERVICE, "{5CDE0403-41B3-45F9-8B6F-49E3193B5425}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate 
		// security settings for your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.

		return S_OK;
	}
	// Called when the service is started
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
};

CATLCOMServiceModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}


void CATLCOMServiceModule::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	// TODO: Add your specialized code here and/or call the base class
	CAtlServiceModuleT<CATLCOMServiceModule,100>::ServiceMain(dwArgc, lpszArgv);
}
