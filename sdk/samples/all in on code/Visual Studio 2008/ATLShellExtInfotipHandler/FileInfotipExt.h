/****************************** Module Header ******************************\
* Module Name:  FileInfotipExt.h
* Project:      ATLShellExtInfotipHandler
* Copyright (c) Microsoft Corporation.
* 
* The declaration of CFileInfotipExt for the FileInfotipExt infotip handler. 
* When mouse is placed over a .txt file item in Windows Explorer, shell 
* queries for the registry entry
* 
* 	HKEY_CLASSES_ROOT\.txt\ShellEx\{00021500-0000-0000-C000-000000000046}
* 
* It then checks the default value at this key. If it is a valid CLSID, shell 
* creates an object of that class. Once the object is created, shell queries 
* for IPersistFile and calls its Load method passing the file name of the 
* item over which mouse is placed. After that, IQueryInfo is queried for and 
* the method GetInfoTip is called. GetInfoTip has an out parameter *ppwszTip 
* which recieves the address of the tool tip buffer. Shell finally displays 
* the info tip using the value in the tool tip buffer.
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

#include "ATLShellExtInfotipHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CFileInfotipExt

class ATL_NO_VTABLE CFileInfotipExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileInfotipExt, &CLSID_FileInfotipExt>,
	public IPersistFile, 
	public IQueryInfo
{
public:
	CFileInfotipExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_FILEINFOTIPEXT)

DECLARE_NOT_AGGREGATABLE(CFileInfotipExt)

BEGIN_COM_MAP(CFileInfotipExt)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IQueryInfo)
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

	// IQueryInfo
	IFACEMETHODIMP GetInfoFlags(DWORD*)     { return E_NOTIMPL; }
	IFACEMETHODIMP GetInfoTip(DWORD, LPWSTR*);

public:

	OLECHAR m_szFileName[MAX_PATH];
};

OBJECT_ENTRY_AUTO(__uuidof(FileInfotipExt), CFileInfotipExt)
