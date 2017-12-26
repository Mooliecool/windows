/****************************** Module Header ******************************\
* Module Name:  FilePropSheetExt.cpp
* Project:      ATLShellExtPropSheetHandler
* Copyright (c) Microsoft Corporation.
* 
* The implementation of CFilePropSheetExt for the FilePropSheet property 
* sheet handler. FilePropSheetExt adds a property page with the subject 
* "All-In-One Code Framework" to the property dialogs of all file classes in 
* Windows Explorer. The property page displays the name of the first selected 
* file. It also has a button "Simulate Property Changing" to simulate the 
* change of properties that activates the "Apply" button of the property 
* sheet.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include "stdafx.h"
#include "FilePropSheetExt.h"
#pragma endregion


INT_PTR CALLBACK FilePropPageDlgProc(HWND, UINT, WPARAM, LPARAM);
UINT CALLBACK FilePropPageCallbackProc(HWND, UINT, LPPROPSHEETPAGE);


/////////////////////////////////////////////////////////////////////////////
// CFilePropSheetExt IShellExtInit methods.
// 

//
//   FUNCTION: CFilePropSheetExt::Initialize(LPCITEMIDLIST, LPDATAOBJECT, 
//             HKEY)
//
//   PURPOSE: Initializes a property sheet extension, shortcut menu extension, 
//            or drag-and-drop handler.
//
IFACEMETHODIMP CFilePropSheetExt::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
    if (NULL == pDataObj)
    {
        return E_INVALIDARG;
    }

    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;

    // pDataObj contains the objects being acted upon. In this example, 
    // we get an HDROP handle for enumerating the selected files.
    if (FAILED(pDataObj->GetData(&fe, &stm)))
        return E_INVALIDARG;

    // Get an HDROP handle.
    HDROP hDrop = (HDROP)GlobalLock(stm.hGlobal);
    if (NULL == hDrop)
    {
        ReleaseStgMedium(&stm);
        return E_INVALIDARG;
    }

    // Determine how many files are involved in this operation
    UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    if (nFiles == 0)
    {
        GlobalUnlock(stm.hGlobal);
        ReleaseStgMedium(&stm);
        return E_INVALIDARG;
    }

    // Get the name of the first file and store it in m_szFileName
    if (0 == DragQueryFile(hDrop, 0, m_szFileName, MAX_PATH))
    {
        GlobalUnlock(stm.hGlobal);
        ReleaseStgMedium(&stm);
        return E_INVALIDARG;
    }

    // [-or-]

    // Enumerates through the selected files and directories
    //TCHAR szFileName[MAX_PATH];
    //for (UINT i = 0; i < nFiles; i++)
    //{
    //	// Get the next filename
    //	if (0 == DragQueryFile(hDrop, i, szFileName, MAX_PATH))
    //		continue;

    //	//...
    //}

    // Release resources
    GlobalUnlock(stm.hGlobal);
    ReleaseStgMedium(&stm);

    // If any value other than S_OK is returned from 
    // IShellExtInit::Initialize, the context menu is not displayed.
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropSheetExt IShellPropSheetExt methods.
// 

//
//   FUNCTION: CFilePropSheetExt::AddPages(LPFNADDPROPSHEETPAGE, LPARAM)
//
//   PURPOSE: Adds one or more pages to a property sheet that the Shell 
//            displays for a file object. The Shell calls this method for 
//            each property sheet handler registered to the file type. 
//
IFACEMETHODIMP CFilePropSheetExt::AddPages(
    LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam )
{
    PROPSHEETPAGE  psp;
    HPROPSHEETPAGE hPage;

    // Set up the PROPSHEETPAGE struct
    ZeroMemory(&psp, sizeof(PROPSHEETPAGE));
    psp.dwSize		= sizeof(PROPSHEETPAGE);
    psp.dwFlags		= PSP_USETITLE | PSP_USECALLBACK;
    psp.hInstance	= _AtlBaseModule.GetResourceInstance();
    psp.pszTemplate	= MAKEINTRESOURCE(IDD_FILE_PROPPAGE);
    psp.pszIcon		= NULL;
    psp.pszTitle	= _T("All-In-One Code Framework");
    psp.pfnDlgProc	= (DLGPROC)FilePropPageDlgProc;
    psp.pcRefParent	= NULL;
    psp.pfnCallback	= FilePropPageCallbackProc;
    // Pass the object pointer to the property page
    psp.lParam		= (LPARAM)this;

    // Create a property sheet page and get the handle.
    hPage = CreatePropertySheetPage(&psp);

    if (hPage)
    {
        // The property page is then added to the property sheet by calling 
        // the callback function passed to IShellPropSheetExt::AddPages in 
        // the lpfnAddPage parameter.
        if (lpfnAddPageProc(hPage, lParam))
        {
            // By default, after AddPages returns, the shell releases its 
            // IShellPropSheetExt interface and the property page cannot  
            // access the extension object. However, it is sometimes desirable
            // to be able to use the extension object, or some other object, 
            // from the property page. So we increase the reference count and 
            // maintain this object until the page is released in 
            // PropPageCallbackProc where we call Release upon the extension.
            this->AddRef();
        }
        else
        {
            DestroyPropertySheetPage(hPage);
            return E_FAIL;
        }
    }
    else
    {
        return E_OUTOFMEMORY;
    }

    // If any value other than S_OK is returned from 
    // IShellPropSheetExt::AddPages, the property sheet is not displayed.
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFilePropSheetExt dialog/callback procs
// 

#define THIS_POINTER_PROP		_T("ThisPointerProperty")


//
//   FUNCTION: OnInitFilePropPageDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message of the property page.
//
BOOL OnInitFilePropPageDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    // Get the pointer to the object. This is contained in the LPARAM of 
    // the PROPSHEETPAGE structure.
    LPPROPSHEETPAGE pPage = (LPPROPSHEETPAGE)lParam;
    if (pPage)
    {
		// Access the property sheet extension from property page
        CFilePropSheetExt* pPropSheetExt = (CFilePropSheetExt*)pPage->lParam;
        if (pPropSheetExt)
        {
            HWND hFileName = GetDlgItem(hWnd, IDC_FILENAME_STATIC);
            SetWindowText(hFileName, pPropSheetExt->m_szFileName);

            // Store the object pointer with this particular page dialog.
            SetProp(hWnd, THIS_POINTER_PROP, (HANDLE)pPropSheetExt);
        }
    }

    return TRUE;
}


//
//   FUNCTION: OnFilePropPageApply(HWND, PSHNOTIFY*)
//
//   PURPOSE: OnFilePropPageApply is called for the property page when user 
//            clicks on the Apply or OK button on the property sheet.
//
BOOL OnFilePropPageApply(HWND hWnd, PSHNOTIFY* phdr)
{
    // Get the property sheet extension object pointer that was stored 
    // in the page dialog. (see OnInitPropPageDialog)
    CFilePropSheetExt *pPropSheetExt = (CFilePropSheetExt*)GetProp(hWnd, 
        THIS_POINTER_PROP);

    // Access the property sheet extension object
    // ...

    // Return PSNRET_NOERROR to allow the property dialog to close if the 
    // user clicked OK
    SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);

    return TRUE;
}


//
//   FUNCTION: OnFilePropPageDestroy(HWND)
//
//   PURPOSE: Process the WM_DESTROY message of the property page.
//
void OnFilePropPageDestroy(HWND hWnd)
{
    // Remove the property from the page.
    RemoveProp(hWnd, THIS_POINTER_PROP);
}


//
//   FUNCTION: FilePropPageDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//   PURPOSE: Processes messages for the property page.
//
INT_PTR CALLBACK FilePropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, 
                                     LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return OnInitFilePropPageDialog(hWnd, (HWND)wParam, lParam);

    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDC_CHANGEFILEPROP_BN:
                // Simulate property changing.
                // Inform the property sheet to enable the Apply button
                SendMessage(GetParent(hWnd), PSM_CHANGED, (WPARAM)hWnd, 0);
                return TRUE;
            }
			break;
        }

    case WM_NOTIFY:
        {
            switch (((LPNMHDR)lParam)->code)
            {
            case PSN_APPLY:
                return OnFilePropPageApply(hWnd, (PSHNOTIFY*)lParam);
            }
			break;
        }

    case WM_DESTROY:
        OnFilePropPageDestroy(hWnd);
        return TRUE;
    }

	return FALSE; // Let system deal with other messages
}


//
//   FUNCTION: FilePropPageCallbackProc(HWND, UINT, LPPROPSHEETPAGE)
//
//   PURPOSE: Specifies an application-defined callback function that a 
//            property sheet calls when a page is created and when it is 
//            about to be destroyed. An application can use this function to 
//            perform initialization and cleanup operations for the page.
//
UINT CALLBACK FilePropPageCallbackProc(HWND hWnd, UINT uMsg, 
                                       LPPROPSHEETPAGE ppsp)
{
    switch(uMsg)
    {
    case PSPCB_CREATE:
        // Must return TRUE to enable the page to be created.
        return TRUE;

    case PSPCB_RELEASE:
        {
            // Release the property sheet extension object. This is called 
            // even if the property page was never actually displayed.
            CFilePropSheetExt *pPropSheetExt = (CFilePropSheetExt*)ppsp->lParam;

            if (pPropSheetExt != NULL)
            {
                pPropSheetExt->Release();
            }
        }
        break;
    }

    return FALSE;
}