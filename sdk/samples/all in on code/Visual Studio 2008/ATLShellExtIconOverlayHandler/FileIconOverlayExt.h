/****************************** Module Header ******************************\
* Module Name:  FileIconOverlayExt.h
* Project:      ATLShellExtIconOverlayHandler
* Copyright (c) Microsoft Corporation.
* 
* Icon overlays are small images placed at the lower-left corner of the icon 
* that represents a Shell object. They are normally added to an object's icon 
* to provide some extra information. For instance, a commonly used icon 
* overlay is the small arrow that indicates that the icon represents a link, 
* rather than the actual file or folder. In addition to the standard icon 
* overlays that are provided by the system, you can request custom icon 
* overlays for specified Shell objects by implementing and registering an 
* icon overlay handler.
* 
* FileIconOverlayExt in ATLShellExtIconOverlayHandler demonstrates a shell 
* icon overlay handler for files with "Sample" in the file paths. After 
* installing the icon overlay handler, any file with "Sample" in its file 
* path, e.g. "D:\Sample.txt", shows an icon overlay in Windows Explorer. 
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

#include "ATLShellExtIconOverlayHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CFileIconOverlayExt

class ATL_NO_VTABLE CFileIconOverlayExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileIconOverlayExt, &CLSID_FileIconOverlayExt>,
	public IShellIconOverlayIdentifier
{
public:
	CFileIconOverlayExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_FILEICONOVERLAYEXT)

DECLARE_NOT_AGGREGATABLE(CFileIconOverlayExt)

BEGIN_COM_MAP(CFileIconOverlayExt)
	COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
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

	// IShellIconOverlayIdentifier
	IFACEMETHODIMP GetOverlayInfo(LPWSTR, int, int*, DWORD*);
	IFACEMETHODIMP GetPriority(int*);
	IFACEMETHODIMP IsMemberOf(LPCWSTR, DWORD);
};

OBJECT_ENTRY_AUTO(__uuidof(FileIconOverlayExt), CFileIconOverlayExt)
