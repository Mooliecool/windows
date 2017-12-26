/******************************** Module Header ********************************\
Module Name:  FilePropSheetExt.cpp
Project:      CppShellExtPropSheetHandler
Copyright (c) Microsoft Corporation.

The code sample demonstrates creating a Shell property sheet handler with C++. 

A property sheet extension is a COM object implemented as an in-proc server. The 
property sheet extension must implement the IShellExtInit and IShellPropSheetExt 
interfaces. A property sheet extension is instantiated when the user displays the
property sheet for an object of a class for which the property sheet extension 
has been registered in the display specifier of the class. It enables you to add 
or replace pages. You can register and implement a property sheet handler for a 
file class, a mounted drive, a control panel application, and starting from 
Windows 7, you can install a property sheet handler to devices in Devices and 
Printers dialog.

The example property sheet handler adds a property sheet page with the title 
"CppShellExtPropSheetHandler" to the Properties dialog of the .cpp file class 
when one .cpp file is selected in the Windows Explorer. The property sheet page 
displays the name of the selected file. It also has a button "Simulate Property 
Changing" to simulate the change of properties that activates the "Apply" button 
of the property sheet. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

#include "FilePropSheetExt.h"
#include "resource.h"
#pragma comment(lib, "comctl32.lib")


extern HINSTANCE g_hInst;


FilePropSheetExt::FilePropSheetExt() : m_cRef(1), 
    pszPageTitle(L"CppShellExtPropSheetHandler")
{
}


FilePropSheetExt::~FilePropSheetExt()
{
}


PCWSTR FilePropSheetExt::GetSelectedFile()
{
    return this->m_szSelectedFile;
}


#define EXT_POINTER_PROP            L"CppShellExtPropSheetHandler"


INT_PTR CALLBACK FilePropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT CALLBACK FilePropPageCallbackProc(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp);


#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP FilePropSheetExt::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hr = S_OK;

    if (IsEqualIID(IID_IUnknown, riid) || 
        IsEqualIID(IID_IShellPropSheetExt, riid))
    {
        *ppv = static_cast<IShellPropSheetExt *>(this);
    }
    else if (IsEqualIID(IID_IShellExtInit, riid))
    {
        *ppv = static_cast<IShellExtInit *>(this);
    }
    else
    {
        hr = E_NOINTERFACE;
        *ppv = NULL;
    }

    if (*ppv)
    {
        AddRef();
    }

    return hr;
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) FilePropSheetExt::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) FilePropSheetExt::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

#pragma endregion


#pragma region IShellExtInit

// Initialize the context menu extension.
IFACEMETHODIMP FilePropSheetExt::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
    if (NULL == pDataObj)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = E_FAIL;

    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;

    // The pDataObj pointer contains the objects being acted upon. In this 
    // example, we get an HDROP handle for enumerating the selected files and 
    // folders.
    if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
    {
        // Get an HDROP handle.
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != NULL)
        {
            // Determine how many files are involved in this operation. This 
            // code sample displays the custom context menu item when only 
            // one file is selected. 
            UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
            if (nFiles == 1)
            {
                // Get the path of the file.
                if (0 != DragQueryFile(hDrop, 0, m_szSelectedFile, 
                    ARRAYSIZE(m_szSelectedFile)))
                {
                    hr = S_OK;
                }
            }

            // [-or-]

            // Enumerates the selected files and folders.
            //if (nFiles > 0)
            //{
            //    std::list<std::wstring> selectedFiles;
            //    wchar_t szFileName[MAX_PATH];
            //    for (UINT i = 0; i < nFiles; i++)
            //    {
            //        // Get the next file name.
            //        if (0 != DragQueryFile(hDrop, i, szFileName, ARRAYSIZE(szFileName)))
            //        {
            //            // Add the file name to the list.
            //            selectedFiles.push_back(szFileName);
            //        }
            //    }

            //    // If we found any files we can work with, return S_OK.
            //    if (selectedFiles.size() > 0) 
            //    {
            //        hr = S_OK;
            //    }
            //}

            GlobalUnlock(stm.hGlobal);
        }

        ReleaseStgMedium(&stm);
    }

    // If any value other than S_OK is returned from the method, the property 
    // sheet is not displayed.
    return hr;
}

#pragma endregion


#pragma region IShellPropSheetExt

//
//   FUNCTION: FilePropSheetExt::AddPages
//
//   PURPOSE: Adds one or more pages to a property sheet that the Shell 
//            displays for a file object. The Shell calls this method for 
//            each property sheet handler registered to the file type. 
//
IFACEMETHODIMP FilePropSheetExt::AddPages(LPFNADDPROPSHEETPAGE pfnAddPage, 
    LPARAM lParam)
{
    // Create a property sheet page.

    PROPSHEETPAGE psp = { sizeof(psp) };
    psp.dwFlags = PSP_USETITLE | PSP_USECALLBACK;
    psp.hInstance = g_hInst;
    psp.pszTemplate = MAKEINTRESOURCE(IDD_FILE_PROPPAGE);
    psp.pszIcon = NULL;
    psp.pszTitle = this->pszPageTitle;
    psp.pfnDlgProc = FilePropPageDlgProc;
    psp.pcRefParent = NULL;
    psp.pfnCallback = FilePropPageCallbackProc;
    psp.lParam = reinterpret_cast<LPARAM>(this);

    HPROPSHEETPAGE hPage = CreatePropertySheetPage(&psp);
    if (hPage == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // The property sheet page is then added to the property sheet by calling 
    // the callback function (LPFNADDPROPSHEETPAGE pfnAddPage) passed to 
    // IShellPropSheetExt::AddPages.
    if (pfnAddPage(hPage, lParam))
    {
        // By default, after AddPages returns, the shell releases its 
        // IShellPropSheetExt interface and the property page cannot access the
        // extension object. However, it is sometimes desirable to be able to use 
        // the extension object, or some other object, from the property page. So 
        // we increase the reference count and maintain this object until the 
        // page is released in PropPageCallbackProc where we call Release upon 
        // the extension.
        this->AddRef();
    }
    else
    {
        DestroyPropertySheetPage(hPage);
        return E_FAIL;
    }

    // If any value other than S_OK is returned from the method, the property 
    // sheet is not displayed.
    return S_OK;
}


IFACEMETHODIMP FilePropSheetExt::ReplacePage(UINT uPageID, 
    LPFNADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam)
{
    // The method is not used.
    return E_NOTIMPL;
}

#pragma endregion


//
//   FUNCTION: FilePropPageDlgProc
//
//   PURPOSE: Processes messages for the property page.
//
INT_PTR CALLBACK FilePropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            // Get the pointer to the property sheet page object. This is 
            // contained in the LPARAM of the PROPSHEETPAGE structure.
            LPPROPSHEETPAGE pPage = reinterpret_cast<LPPROPSHEETPAGE>(lParam);
            if (pPage != NULL)
            {
                // Access the property sheet extension from property page.
                FilePropSheetExt *pExt = reinterpret_cast<FilePropSheetExt *>(pPage->lParam);
                if (pExt != NULL)
                {
                    // Display the name of the selected file.
                    HWND hFileLabel = GetDlgItem(hWnd, IDC_SELECTEDFILE_STATIC);
                    SetWindowText(hFileLabel, pExt->GetSelectedFile());

                    // Store the object pointer with this particular page dialog.
                    SetProp(hWnd, EXT_POINTER_PROP, static_cast<HANDLE>(pExt));
                }
            }
            return TRUE;
        }

    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDC_CHANGEPROP_BUTTON:
                // User clicks the "Simulate Property Changing" button...

                // Simulate property changing. Inform the property sheet to 
                // enable the Apply button.
                SendMessage(GetParent(hWnd), PSM_CHANGED, 
                    reinterpret_cast<WPARAM>(hWnd), 0);
                return TRUE;
            }
        }
        break;

    case WM_NOTIFY:
        {
            switch ((reinterpret_cast<LPNMHDR>(lParam))->code)
            {
            case PSN_APPLY:
                // The PSN_APPLY notification code is sent to every page in the 
                // property sheet to indicate that the user has clicked the OK, 
                // Close, or Apply button and wants all changes to take effect. 

                // Get the property sheet extension object pointer that was 
                // stored in the page dialog (See the handling of WM_INITDIALOG 
                // in FilePropPageDlgProc).
                FilePropSheetExt *pExt = static_cast<FilePropSheetExt *>(
                    GetProp(hWnd, EXT_POINTER_PROP));

                // Access the property sheet extension object.
                // ...

                // Return PSNRET_NOERROR to allow the property dialog to close if 
                // the user clicked OK.
                SetWindowLong(hWnd, DWLP_MSGRESULT, PSNRET_NOERROR);

                return TRUE;
            }
        }
        break;

    case WM_DESTROY:
        {
            // Remove the EXT_POINTER_PROP property from the page. 
            // The EXT_POINTER_PROP property stored the pointer to the 
            // FilePropSheetExt object.
            RemoveProp(hWnd, EXT_POINTER_PROP);
            return TRUE;
        }
    }

    return FALSE; // Let system deal with other messages
}


//
//   FUNCTION: FilePropPageCallbackProc
//
//   PURPOSE: Specifies an application-defined callback function that a property
//            sheet calls when a page is created and when it is about to be 
//            destroyed. An application can use this function to perform 
//            initialization and cleanup operations for the page.
//
UINT CALLBACK FilePropPageCallbackProc(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
    switch(uMsg)
    {
    case PSPCB_CREATE:
        {
            // Must return TRUE to enable the page to be created.
            return TRUE;
        }

    case PSPCB_RELEASE:
        {
            // When the callback function receives the PSPCB_RELEASE notification, 
            // the ppsp parameter of the PropSheetPageProc contains a pointer to 
            // the PROPSHEETPAGE structure. The lParam member of the PROPSHEETPAGE 
            // structure contains the extension pointer which can be used to 
            // release the object.

            // Release the property sheet extension object. This is called even 
            // if the property page was never actually displayed.
            FilePropSheetExt *pExt = reinterpret_cast<FilePropSheetExt *>(ppsp->lParam);
            if (pExt != NULL)
            {
                pExt->Release();
            }
        }
        break;
    }

    return FALSE;
}