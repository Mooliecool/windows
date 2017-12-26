/****************************** Module Header ******************************\
* Module Name:  Win7DeviceContextMenuExt.h
* Project:      ATLShellExtContextMenuHandler
* Copyright (c) Microsoft Corporation.
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

#pragma once
#include "resource.h"       // main symbols

#include "ATLShellExtContextMenuHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CWin7DeviceContextMenuEx

class ATL_NO_VTABLE CWin7DeviceContextMenuEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWin7DeviceContextMenuEx, &CLSID_Win7DeviceContextMenuEx>,
	public IShellExtInit, 
	public IContextMenu
{
public:
	CWin7DeviceContextMenuEx()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WIN7DEVICECONTEXTMENUEX)

DECLARE_NOT_AGGREGATABLE(CWin7DeviceContextMenuEx)

BEGIN_COM_MAP(CWin7DeviceContextMenuEx)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
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

	// IContextMenu
	IFACEMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
	IFACEMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

protected:

	// The function that handles the "Sample" verb
	void OnSample(HWND hWnd);
};

OBJECT_ENTRY_AUTO(__uuidof(Win7DeviceContextMenuEx), CWin7DeviceContextMenuEx)