/****************************** Module Header ******************************\
* Module Name:  FileIconExt.h
* Project:      ATLShellExtIconHandler
* Copyright (c) Microsoft Corporation.
* 
* An shell icon handler is a type of Shell extension handler that allows you 
* to dynamically assign icons to the members of a file class and replace the 
* file's default icon with a custom icon on a file-by-file basis. Every time 
* a file from the class is displayed, the Shell queries the handler for the 
* appropriate icon. For instance, an icon handler can assign different icons 
* to different members of the class, or vary the icon based on the current 
* state of the file. 
* 
* FileIconExt in ATLShellExtIconHandler demonstrates a shell icon handler for 
* .cfx files. After installing the icon handler, "C#.cfx" files show Sample 
* C# icon, "VB.cfx" files show Sample VB icon, "C++.cfx" files show Sample 
* C++ icon, and other .cfx files show a Sample icon in Microsoft Windows 
* Explorer. 
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

#include "ATLShellExtIconHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CFileIconExt

class ATL_NO_VTABLE CFileIconExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileIconExt, &CLSID_FileIconExt>,
	public IPersistFile,
	public IExtractIcon
{
public:
	CFileIconExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_FILEICONEXT)

DECLARE_NOT_AGGREGATABLE(CFileIconExt)

BEGIN_COM_MAP(CFileIconExt)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IExtractIcon)
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

	/*!
	* Good Practice:
	* 
	* IFACEMETHODIMP is used instead of STDMETHODIMP for the COM interface 
	* method impelmetnations. IFACEMETHODIMP includes "__override" that lets 
	* SAL check that you are overriding a method, so this should be used for 
	* all COM interface method impelmetnations.
	*/

	// IPersistFile
	IFACEMETHODIMP GetClassID(LPCLSID)      { return E_NOTIMPL; }
	IFACEMETHODIMP IsDirty()                { return E_NOTIMPL; }
	IFACEMETHODIMP Load(LPCOLESTR, DWORD);
	IFACEMETHODIMP Save(LPCOLESTR, BOOL)    { return E_NOTIMPL; }
	IFACEMETHODIMP SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
	IFACEMETHODIMP GetCurFile(LPOLESTR*)    { return E_NOTIMPL; }

	// IExtractIcon
	IFACEMETHODIMP GetIconLocation(UINT, LPTSTR, UINT, int*, UINT*);
	IFACEMETHODIMP Extract(LPCTSTR, UINT, HICON*, HICON*, UINT);

public:

	OLECHAR m_szFileName[MAX_PATH];
};

OBJECT_ENTRY_AUTO(__uuidof(FileIconExt), CFileIconExt)
