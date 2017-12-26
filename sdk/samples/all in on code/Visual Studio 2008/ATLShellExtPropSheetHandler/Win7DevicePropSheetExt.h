/****************************** Module Header ******************************\
* Module Name:  Win7DevicePropSheetExt.h
* Project:      ATLShellExtPropSheetHandler
* Copyright (c) Microsoft Corporation.
* 
* The declaration of CWin7DevicePropSheetExt for the Win7DevicePropSheet 
* property sheet handler. Win7DevicePropSheetExt adds a property page with 
* the subject "All-In-One Code Framework" to the property sheet of mouse in 
* Windows 7 Devices and Printers dialog. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once
#include "resource.h"       // Main symbols

#include "ATLShellExtPropSheetHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CWin7DevicePropSheetExt

class ATL_NO_VTABLE CWin7DevicePropSheetExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWin7DevicePropSheetExt, &CLSID_Win7DevicePropSheetExt>,
	public IShellExtInit,
	public IShellPropSheetExt
{
public:
	CWin7DevicePropSheetExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WIN7DEVICEPROPSHEETEXT)

DECLARE_NOT_AGGREGATABLE(CWin7DevicePropSheetExt)

BEGIN_COM_MAP(CWin7DevicePropSheetExt)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IShellPropSheetExt)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IShellPropSheetExt
    IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
    IFACEMETHODIMP ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM) 
	{
		// The IShellPropSheetExt::ReplacePage method is not used.
		return E_NOTIMPL;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Win7DevicePropSheetExt), CWin7DevicePropSheetExt)
