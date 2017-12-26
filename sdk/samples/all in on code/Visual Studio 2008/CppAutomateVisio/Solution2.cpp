/****************************** Module Header ******************************\
* Module Name:  Solution2.cpp
* Project:      CppAutomateVisio
* Copyright (c) Microsoft Corporation.
* 
* The code in Solution2.h/cpp demontrates the use of C/C++ and the COM APIs 
* to automate Visio. The raw automation is much more difficult, but it is 
* sometimes necessary to avoid the overhead with MFC, or problems with 
* #import. Basically, you work with such APIs as CoCreateInstance(), and COM 
* interfaces such as IDispatch and IUnknown.
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
#include "Solution2.h"
#pragma endregion


DWORD WINAPI AutomateVisioByCOMAPI(LPVOID lpParam)
{
	HRESULT hr;

	// Initializes the COM library on the current thread and identifies 
	// the concurrency model as single-thread apartment (STA). 
	// [-or-] ::CoInitialize(NULL);
	// [-or-] ::CoCreateInstance(NULL);
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);


	/////////////////////////////////////////////////////////////////////////
	// Create the Visio.Application COM object using C++ and COM APIs.
	// 

	// Get CLSID of the server

	CLSID clsid;

	// Option 1. Get CLSID from ProgID using CLSIDFromProgID.
	LPCOLESTR progID = L"Visio.Application";
	hr = ::CLSIDFromProgID(progID, &clsid);
	if (FAILED(hr))
	{
		wprintf(L"CLSIDFromProgID(\"%s\") failed w/err 0x%08lx\n", progID, hr);
		return hr;
	}
	// Option 2. Build the CLSID directly.
	/*const IID CLSID_Application = 
	{0x00021A20,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
	clsid = CLSID_Application;*/

	// Start the server and get the IDispatch interface

	IDispatch* pVisioApp = NULL;
	hr = CoCreateInstance(		// [-or-] CoCreateInstanceEx, CoGetObject
		clsid,					// CLSID of the server
		NULL,
		CLSCTX_LOCAL_SERVER,	// Visio.Application is a local server
		IID_IDispatch,			// Query the IDispatch interface
		(void **)&pVisioApp);	// Output

	if (FAILED(hr))
	{
		_tprintf(_T("Visio is not registered properly w/err 0x%08lx\n"), 
			hr);
		return hr;
	}

	_putts(_T("Visio.Application is started"));


	/////////////////////////////////////////////////////////////////////////
	// Make Visio invisible. (Application.Visible = 0)
	// 

	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, pVisioApp, L"Visible", 1, x);
	}


	/////////////////////////////////////////////////////////////////////////
	// Create a new Document based on no template. 
	// (i.e. Application.Documents.Add(""))
	// 

	// Get the Documents collection
	IDispatch* pDocs = NULL;
	{ 
		VARIANT result;
		VariantInit(&result);
		hr = AutoWrap(DISPATCH_PROPERTYGET, &result, pVisioApp, L"Documents", 0);
		pDocs = result.pdispVal;
	}

	// Call Documents.Add("") to get a new document based on no template
	IDispatch* pDoc = NULL;
	{
		VARIANT x;
		x.vt = VT_BSTR;
		x.bstrVal = ::SysAllocString(L"");

		VARIANT result;
		VariantInit(&result);
		hr = AutoWrap(DISPATCH_METHOD, &result, pDocs, L"Add", 1, x);
		pDoc = result.pdispVal;

		VariantClear(&x);
	}

	_putts(_T("A new document is created"));


	/////////////////////////////////////////////////////////////////////////
	// Draw a rectangle and a oval on the first page.
	// 

	_putts(_T("Draw a rectangle and a oval"));

	// Get the Pages collection
	IDispatch* pPages = NULL;
	{
		VARIANT result;
		VariantInit(&result);
		hr = AutoWrap(DISPATCH_PROPERTYGET, &result, pDoc, L"Pages", 0);
		pPages = result.pdispVal;
	}

	// Get the first page (pPages->Item[1])
	IDispatch* pPage = NULL;
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 1;

		VARIANT result;
		VariantInit(&result);
		hr = AutoWrap(DISPATCH_PROPERTYGET, &result, pPages, L"Item", 1, x);
		pPage = result.pdispVal;
	}

	// Draw a rectangle (pPage->DrawRectangle)
	IDispatch* pRectShape = NULL;
	{
		VARIANT x1;
		x1.vt = VT_R8;
		x1.dblVal = 0.5;
		VARIANT y1;
		y1.vt = VT_R8;
		y1.dblVal = 10.25;
		VARIANT x2;
		x2.vt = VT_R8;
		x2.dblVal = 6.25;
		VARIANT y2;
		y2.vt = VT_R8;
		y2.dblVal = 7.375;

		VARIANT result;
		VariantInit(&result);
		hr = AutoWrap(DISPATCH_METHOD, &result, pPage, L"DrawRectangle", 
			4, y2, x2, y1, x1);
		pRectShape = result.pdispVal;
	}

	// Draw a oval (pPage->DrawOval)
	IDispatch* pOvalShape = NULL;
	{
		VARIANT x1;
		x1.vt = VT_R8;
		x1.dblVal = 1.125;
		VARIANT y1;
		y1.vt = VT_R8;
		y1.dblVal = 6;
		VARIANT x2;
		x2.vt = VT_R8;
		x2.dblVal = 6.875;
		VARIANT y2;
		y2.vt = VT_R8;
		y2.dblVal = 2.125;
		
		VARIANT result;
		VariantInit(&result);
		hr = AutoWrap(DISPATCH_METHOD, &result, pPage, L"DrawOval", 
			4, y2, x2, y1, x1);
		pOvalShape = result.pdispVal;
	}


	/////////////////////////////////////////////////////////////////////
	// Save the document as a vsd file and close it.
	// 

	_putts(_T("Save and close the document"));

	// pDoc->SaveAs
	{
		// Make the file name

		// Get the directory of the current exe.
		WCHAR szFileName[MAX_PATH];
		if (FAILED(GetModuleDirectoryW(szFileName, MAX_PATH)))
		{
			_putts(_T("GetModuleDirectoryW failed"));
			return S_FALSE;
		}

		// Concat "Sample2.vsd" to the directory
		wcsncat_s(szFileName, MAX_PATH, L"Sample2.vsd", 11);

		// Convert the NULL-terminated string to BSTR
		VARIANT vtFileName;
		vtFileName.vt = VT_BSTR;
		vtFileName.bstrVal = ::SysAllocString(szFileName);

		hr = AutoWrap(DISPATCH_METHOD, NULL, pDoc, L"SaveAs", 1, vtFileName);

		VariantClear(&vtFileName);
	}

	// pDoc->Close()
	hr = AutoWrap(DISPATCH_METHOD, NULL, pDoc, L"Close", 0);


	/////////////////////////////////////////////////////////////////////////
	// Quit the Visio application. (i.e. Application.Quit())
	// 

	_putts(_T("Quit the Visio application"));
	hr = AutoWrap(DISPATCH_METHOD, NULL, pVisioApp, L"Quit", 0);


	/////////////////////////////////////////////////////////////////////////
	// Release the COM objects.
	// 

	if (pOvalShape != NULL)
	{
		pOvalShape->Release();
	}
	if (pRectShape != NULL)
	{
		pRectShape->Release();
	}
	if (pPage != NULL)
	{
		pPage->Release();
	}
	if (pPages != NULL)
	{
		pPages->Release();
	}
	if (pDoc != NULL)
	{
		pDoc->Release();
	}
	if (pDocs != NULL)
	{
		pDocs->Release();
	}
	if (pVisioApp != NULL)
	{
		pVisioApp->Release();
	}

	// Uninitialize COM for this thread
	::CoUninitialize();

	return hr;
}


/*!
* \brief
* The definition of AutoWrap in the header file
* 
* \see
* Separate Solution2.h | AutoWrap
*/
HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, 
				 LPOLESTR ptName, int cArgs...) 
{
	// Begin variable-argument list
	va_list marker;
	va_start(marker, cArgs);

	if (!pDisp) 
	{
		_putts(_T("NULL IDispatch passed to AutoWrap()"));
		return E_INVALIDARG;
	}

	// Variables used
	DISPPARAMS dp = { NULL, NULL, 0, 0 };
	DISPID dispidNamed = DISPID_PROPERTYPUT;
	DISPID dispID;
	HRESULT hr;

	// Get DISPID for name passed
	hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
	if (FAILED(hr))
	{
		wprintf(L"IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx\n", 
			ptName, hr);
		return hr;
	}

	// Allocate memory for arguments
	VARIANT *pArgs = new VARIANT[cArgs + 1];
	// Extract arguments...
	for(int i=0; i < cArgs; i++) 
	{
		pArgs[i] = va_arg(marker, VARIANT);
	}

	// Build DISPPARAMS
	dp.cArgs = cArgs;
	dp.rgvarg = pArgs;

	// Handle special-case for property-puts
	if (autoType & DISPATCH_PROPERTYPUT)
	{
		dp.cNamedArgs = 1;
		dp.rgdispidNamedArgs = &dispidNamed;
	}

	// Make the call
	hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT,
		autoType, &dp, pvResult, NULL, NULL);
	if (FAILED(hr)) 
	{
		wprintf(L"IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx\n", 
			ptName, dispID, hr);
		return hr;
	}

	// End variable-argument section
	va_end(marker);

	delete[] pArgs;

	return hr;
}