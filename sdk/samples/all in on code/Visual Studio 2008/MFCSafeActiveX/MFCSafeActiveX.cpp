/****************************** Module Header ******************************\
* Module Name:  MFCSafeActiveX.cpp
* Project:      MFCSafeActiveX
* Copyright (c) Microsoft Corporation.
* 
* Implementation of CMFCSafeActiveXApp and DLL registration.
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
#include "MFCSafeActiveX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Include the file that implements CreateComponentCategory, RegisterCLSIDInCategory 
// and UnRegisterCLSIDInCategory

#include "CatHelp.h"
#pragma endregion


CMFCSafeActiveXApp theApp;

const GUID CDECL BASED_CODE _tlid =
{ 0x98DB52D, 0x2AAE, 0x499B, { 0xB9, 0x59, 0xA4, 0x30, 0xBA, 0xF, 0xF3, 0x57 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CLSID_SafeItem - Necessary for safe ActiveX control
// The value is taken from IMPLEMENT_OLECREATE_EX in MFCSafeActiveXCtrl.cpp.
// It is actually the CLSID of the ActiveX control.

const CATID CLSID_SafeItem =
{ 0x1ebae592, 0x7515, 0x43c2,{ 0xa6, 0xf1, 0xcd, 0xee, 0xdf, 0x3f, 0xd8, 0x2b}};


// CMFCSafeActiveXApp::InitInstance - DLL initialization
BOOL CMFCSafeActiveXApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


// CMFCSafeActiveXApp::ExitInstance - DLL termination
int CMFCSafeActiveXApp::ExitInstance()
{
	// TODO: Add your own module termination code here.
	return COleControlModule::ExitInstance();
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	HRESULT hr;    // HResult used by Safety Functions

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	// Mark the control as safe for initializing. (Added)

	hr = CreateComponentCategory(CATID_SafeForInitializing, 
		L"Controls safely initializable from persistent data!");
	if (FAILED(hr))
		return hr;

	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	// Mark the control as safe for scripting. (Added)

	hr = CreateComponentCategory(CATID_SafeForScripting,
		L"Controls safely  scriptable!");
	if (FAILED(hr))
		return hr;

	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

	return NOERROR;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr;    // HResult used by Safety Functions

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	// Remove entries from the registry.

	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

	return NOERROR;
}