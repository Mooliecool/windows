/****************************** Module Header ******************************\
* Module Name:  Solution1.cpp
* Project:      CppAutomateVisio
* Copyright (c) Microsoft Corporation.
* 
* The code in Solution1.h/cpp demonstrates the use of #import to automate 
* Visio. #import (http://msdn.microsoft.com/en-us/library/8etzzkb6.aspx), 
* a new directive that became available with Visual C++ 5.0, creates VC++ 
* "smart pointers" from a specified type library. It is very powerful, but 
* often not recommended because of reference-counting problems that typically 
* occur when used with the Microsoft Office applications. Unlike the direct 
* API approach in Solution2.h/cpp, smart pointers enable us to benefit from 
* the type info to early/late bind the object. #import takes care of adding 
* the messy guids to the project and the COM APIs are encapsulated in custom 
* classes that the #import directive generates.
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
#include "Solution1.h"
#pragma endregion


#pragma region Import the type libraries

#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
	rename("RGB", "MSORGB") \
	rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")

using namespace Office;

//#import "libid:00021A98-0000-0000-C000-000000000046"
// [-or-]
#import "C:\\Program Files\\Microsoft Office\\Office12\\VISLIB.DLL"

#pragma endregion


DWORD WINAPI AutomateVisioByImport(LPVOID lpParam)
{
	HRESULT hr;

	// Initializes the COM library on the current thread and identifies the
	// concurrency model as single-thread apartment (STA). 
	// [-or-] ::CoInitialize(NULL);
	// [-or-] ::CoCreateInstance(NULL);
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	try
	{

		/////////////////////////////////////////////////////////////////////
		// Create the Visio.Application COM object using the #import 
		// directive and smart pointers.
		// 

		// Option 1) Create the object using the smart pointer's constructor
		 
		// IVApplicationPtr is the original interface name, IVApplication, 
		// with a "Ptr" suffix.
		//Visio::IVApplicationPtr spVisioApp(
		//	__uuidof(Visio::Application)	// CLSID of the component
		//	);

		// Option 2) Create the object using the smart pointer's function,
		// CreateInstance

		Visio::IVApplicationPtr spVisioApp;
		hr = spVisioApp.CreateInstance(__uuidof(Visio::Application));
		if (FAILED(hr))
		{
			_tprintf(_T(
				"Visio::IVApplicationPtr.CreateInstance failed w/err 0x%08lx\n"
				), hr);
			return hr;
		}

		_putts(_T("Visio.Application is started"));


		/////////////////////////////////////////////////////////////////////
		// Make Visio invisible. (i.e. Application.Visible = 0)
		// 
		
		spVisioApp->Visible = VARIANT_FALSE;


		/////////////////////////////////////////////////////////////////////
		// Create a new Document based on no template. 
		// (i.e. Application.Documents.Add(""))
		// 

		Visio::IVDocumentsPtr spDocs = spVisioApp->Documents;
		Visio::IVDocumentPtr spDoc = spDocs->Add(_bstr_t(_T("")));

		_putts(_T("A new document is created"));


		/////////////////////////////////////////////////////////////////////
		// Draw a rectangle and a oval on the first page.
		// 

		_putts(_T("Draw a rectangle and a oval"));

		Visio::IVPagesPtr spPages = spDoc->Pages;
		Visio::IVPagePtr spPage = spPages->Item[1];
		Visio::IVShapePtr spRectShape = spPage->DrawRectangle(0.5, 10.25, 6.25, 7.375);
		Visio::IVShapePtr spOvalShape = spPage->DrawOval(1.125, 6, 6.875, 2.125);


		/////////////////////////////////////////////////////////////////////
		// Save the document as a vsd file and close it.
		// 

		_putts(_T("Save and close the document"));

		// Make the file name
		
		// Get the directory of the current exe.
		WCHAR szFileName[MAX_PATH];
		if (FAILED(GetModuleDirectoryW(szFileName, MAX_PATH)))
		{
			_putts(_T("GetModuleDirectoryW failed"));
			return S_FALSE;
		}

		// Concat "Sample1.vsd" to the directory
		wcsncat_s(szFileName, MAX_PATH, L"Sample1.vsd", 11);
		
		// Convert the NULL-terminated string to BSTR
		_bstr_t bstrFileName(szFileName);

		spDoc->SaveAs(bstrFileName);

		hr = spDoc->Close();


		/////////////////////////////////////////////////////////////////////
		// Quit the Visio application. (i.e. Application.Quit)
		// 

		_putts(_T("Quit the Visio application"));

		hr = spVisioApp->Quit();


		/////////////////////////////////////////////////////////////////////
		// Release the COM objects.
		// 

		// Releasing the references is not necessary for the smart pointers
		// spVisioApp.Release();

	}
	catch (_com_error &err)
	{
		_tprintf(_T("Visio throws the error: %s\n"), err.ErrorMessage());
		_tprintf(_T("Description: %s\n"), (LPCTSTR) err.Description());
	}

	// Uninitialize COM for this thread
	CoUninitialize();

	return 1;
}