/****************************** Module Header ******************************\
* Module Name:  CppRegFreeCOMClient.cpp
* Project:      CppRegFreeCOMClient
* Copyright (c) Microsoft Corporation.
* 
* 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Imports
#include <stdio.h>
#include <tchar.h>

#import "CSRegFreeCOMServer.tlb" no_namespace named_guids
#pragma endregion


int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr;

	// Initializes the COM library on the current thread and identifies the
	// concurrency model as single-thread apartment (STA). 
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);


	/////////////////////////////////////////////////////////////////////////
	// Create the CSRegFreeCOMServer.SimpleObject COM object using 
	// the #import directive and smart pointers.
	// 

	// Create an instance of the component.
	// NOTE: Make sure that registration-free components are created with 
	// their CLSID and not the ProgId.
	ISimpleObjectPtr spSimpleObj;
	hr = spSimpleObj.CreateInstance(__uuidof(SimpleObject));
	if (FAILED(hr))
	{
		_tprintf(_T(
			"ISimpleObjectPtr::CreateInstance failed w/err 0x%08lx\n"
			), hr);
		return hr;
	}


	/////////////////////////////////////////////////////////////////////////
	// Consume the properties and the methods of the COM object.
	// 

	try
	{
		// Set the property: FloatProperty.
		{
			_tprintf(_T("Set FloatProperty = %.2f\n"), 1.2f);
			spSimpleObj->FloatProperty = 1.2f;
		}

		// Get the property: FloatProperty.
		{
			_tprintf(_T("Get FloatProperty = %.2f\n"),
				spSimpleObj->FloatProperty);
		}

		// Call the method: HelloWorld, that returns a BSTR.
		{
			// the _bstr_t object and the underlying BSTR will be cleared 
			// automatically in the destructor when the object is out of 
			// the scope.
			_bstr_t bstrResult = spSimpleObj->HelloWorld();
			_tprintf(_T("Call HelloWorld => %s\n"), (LPCTSTR)bstrResult);
		}

		// Call the method: GetProcessThreadID, that outputs two DWORDs.
		{
			_tprintf(_T("The client process and thread: %ld, %ld\n"),
				GetCurrentProcessId(), GetCurrentThreadId());
			
			DWORD dwProcessId, dwThreadId;
			spSimpleObj->GetProcessThreadID(&dwProcessId, &dwThreadId);
			_tprintf(_T("Call GetProcessThreadID => %ld, %ld\n"), 
				dwProcessId, dwThreadId);
		}

		_putts(_T(""));
	}
	catch (_com_error &err)
	{
		_tprintf(_T("The server throws the error: %s\n"), err.ErrorMessage());
		_tprintf(_T("Description: %s\n"), (LPCTSTR) err.Description());
	}


	/////////////////////////////////////////////////////////////////////////
	// Release the COM object.
	// 

	// Releasing the references is not necessary for the smart pointers
	// spSimpleObj.Release();

	// Uninitialize COM for this thread
	CoUninitialize();
	
	return 0;
}