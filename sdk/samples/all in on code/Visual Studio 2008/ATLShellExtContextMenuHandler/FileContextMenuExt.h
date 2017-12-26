/****************************** Module Header ******************************\
* Module Name:  FileContextMenuExt.h
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
#include "resource.h"       // Main symbols

#include "ATLShellExtContextMenuHandler_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CFileContextMenuExt

class ATL_NO_VTABLE CFileContextMenuExt :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFileContextMenuExt, &CLSID_FileContextMenuExt>,
    public IShellExtInit, 
    public IContextMenu
{
public:
    CFileContextMenuExt()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_FILECONTEXTMENUEXT)

    DECLARE_NOT_AGGREGATABLE(CFileContextMenuExt)

    BEGIN_COM_MAP(CFileContextMenuExt)
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

    /*!
    * Good Practice:
    * 
    * IFACEMETHODIMP is used instead of STDMETHODIMP for the COM interface 
    * method impelmetnations. IFACEMETHODIMP includes "__override" that lets 
    * SAL check that you are overriding a method, so this should be used for 
    * all COM interface method impelmetnations.
    */

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    IFACEMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
    IFACEMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

protected:

    // The name of the first selected file
    TCHAR m_szFileName[MAX_PATH];

    // The function that handles the "Sample" verb
    void OnSample(HWND hWnd);
};

OBJECT_ENTRY_AUTO(__uuidof(FileContextMenuExt), CFileContextMenuExt)