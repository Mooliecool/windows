/****************************** Module Header ******************************\
* Module Name:  FilePropSheetExt.h
* Project:      ATLShellExtPropSheetHandler
* Copyright (c) Microsoft Corporation.
* 
* The declaration of CFilePropSheetExt for the FilePropSheet property sheet 
* handler. FilePropSheetExt adds a property page with the subject "All-In-One 
* Code Framework" to the property dialogs of all file classes in Windows 
* Explorer. The property page displays the name of the first selected file. 
* It also has a button "Simulate Property Changing" to simulate the change of 
* properties that activates the "Apply" button of the property sheet.
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



// CFilePropSheetExt

class ATL_NO_VTABLE CFilePropSheetExt :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFilePropSheetExt, &CLSID_FilePropSheetExt>,
    public IShellExtInit,
    public IShellPropSheetExt
{
public:
    CFilePropSheetExt()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_FILEPROPSHEETEXT)

    DECLARE_NOT_AGGREGATABLE(CFilePropSheetExt)

    BEGIN_COM_MAP(CFilePropSheetExt)
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

    // IShellPropSheetExt
    IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
    IFACEMETHODIMP ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM) 
    {
        // The IShellPropSheetExt::ReplacePage method is not used.
        return E_NOTIMPL;
    }

public:

    // The name of the first selected file
    TCHAR m_szFileName[MAX_PATH];
};

OBJECT_ENTRY_AUTO(__uuidof(FilePropSheetExt), CFilePropSheetExt)