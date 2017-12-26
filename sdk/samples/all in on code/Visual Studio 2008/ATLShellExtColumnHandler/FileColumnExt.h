/****************************** Module Header ******************************\
* Module Name:  FileColumnExt.h
* Project:      ATLShellExtColumnHandler
* Copyright (c) Microsoft Corporation.
* 
* The Details view in the Microsoft Windows Windows Explorer normally 
* displays several standard columns. Each column lists information, such as 
* the file size or type, for each file in the current folder. By implementing
* and registering a column handler, you can make custom columns available for 
* display on Windows 2000, Windows XP and Windows 2003. 
* 
* Note: Support for column handler (IColumnProvider) has been removed from 
* Windows Vista. So the sample does not work on Windows Vista, and later 
* operating systems. The new property system should be used in its place. See 
* Property System (http://msdn.microsoft.com/en-us/library/bb776859.aspx) for 
* conceptual materials that explain the use of the new system.
* 
* FileColumnExt in ATLShellExtColumnHandler demonstrates a column handler 
* that provides three columns:
* 
* 	1. Sample C1 - display the file path when enabled
* 	2. Sample C2 - display the file size when enabled
* 	3. Title (Predefined column) - display the file name when enabled
* 
* The three columns process only .cfx files.
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

#include "ATLShellExtColumnHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CFileColumnExt

class ATL_NO_VTABLE CFileColumnExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileColumnExt, &CLSID_FileColumnExt>,
	public IColumnProvider
{
public:
	CFileColumnExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_FILECOLUMNEXT)

DECLARE_NOT_AGGREGATABLE(CFileColumnExt)

BEGIN_COM_MAP(CFileColumnExt)
	COM_INTERFACE_ENTRY_IID(IID_IColumnProvider, IColumnProvider)
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

	// IColumnProvider
	IFACEMETHODIMP Initialize(LPCSHCOLUMNINIT psci);
	IFACEMETHODIMP GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci);
	IFACEMETHODIMP GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, 
		VARIANT* pvarData);
};

OBJECT_ENTRY_AUTO(__uuidof(FileColumnExt), CFileColumnExt)
