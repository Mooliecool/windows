/****************************** Module Header ******************************\
* Module Name:  Win7DevicePropSheetExt.cpp
* Project:      ATLShellExtPropSheetHandler
* Copyright (c) Microsoft Corporation.
* 
* The implementation of CWin7DevicePropSheetExt for the Win7DevicePropSheet 
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

#pragma region Includes
#include "stdafx.h"
#include "Win7DevicePropSheetExt.h"
#pragma endregion


INT_PTR CALLBACK Win7DevicePropPageDlgProc(HWND, UINT, WPARAM, LPARAM);
UINT CALLBACK Win7DevicePropPageCallbackProc(HWND, UINT, LPPROPSHEETPAGE);


/////////////////////////////////////////////////////////////////////////////
// CWin7DevicePropSheetExt IShellExtInit methods.
// 

//
//   FUNCTION: CWin7DevicePropSheetExt::Initialize(LPCITEMIDLIST, 
//             LPDATAOBJECT, HKEY)
//
//   PURPOSE: Initializes a property sheet extension, shortcut menu extension, 
//            or drag-and-drop handler.
//
IFACEMETHODIMP CWin7DevicePropSheetExt::Initialize(
    LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	// If any value other than S_OK is returned from 
	// IShellExtInit::Initialize, the context menu is not displayed.
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWin7DevicePropSheetExt IShellPropSheetExt methods.
// 

//
//   FUNCTION: CWin7DevicePropSheetExt::AddPages(LPFNADDPROPSHEETPAGE, LPARAM)
//
//   PURPOSE: Adds one or more pages to a property sheet that the Shell 
//            displays for a hardware object. The Shell calls this method for 
//            each property sheet handler registered to the hardware. 
//
IFACEMETHODIMP CWin7DevicePropSheetExt::AddPages(
    LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam )
{
	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;

	// Set up the PROPSHEETPAGE struct
	ZeroMemory(&psp, sizeof(PROPSHEETPAGE));
	psp.dwSize		= sizeof(PROPSHEETPAGE);
	psp.dwFlags		= PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= _AtlBaseModule.GetResourceInstance();
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_WIN7DEVICE_PROPPAGE);
	psp.pszIcon		= NULL;
	psp.pszTitle	= _T("All-In-One Code Framework");
	psp.pfnDlgProc	= (DLGPROC)Win7DevicePropPageDlgProc;
	psp.pcRefParent	= NULL;
	psp.pfnCallback	= Win7DevicePropPageCallbackProc;
	// Pass the object pointer to the property page
	psp.lParam		= (LPARAM)this;

	// Create the page and get a handle
	hPage = CreatePropertySheetPage (&psp);

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
// CWin7DevicePropSheetExt dialog/callback procs
// 

#define THIS_POINTER_PROP		_T("ThisPointerProperty")

//
//   FUNCTION: OnInitWin7DevicePropPageDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message of the property page.
//
BOOL OnInitWin7DevicePropPageDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
	// Get the pointer to the object. This is contained in the LPARAM of 
	// the PROPSHEETPAGE structure.
	LPPROPSHEETPAGE pPage = (LPPROPSHEETPAGE)lParam;
	if (pPage)
	{
		CWin7DevicePropSheetExt* pPropSheetExt = (CWin7DevicePropSheetExt*)
			pPage->lParam;
		if (pPropSheetExt)
		{
			// Access the property sheet extension from property page
			//...

			// Store the object pointer with this particular page dialog.
			SetProp(hWnd, THIS_POINTER_PROP, (HANDLE)pPropSheetExt);
		}
	}

	return TRUE;
}

//
//   FUNCTION: OnWin7DevicePropPageApply(HWND, PSHNOTIFY*)
//
//   PURPOSE: OnWin7DevicePropPageApply is called for the property page when 
//            user clicks on the Apply or OK button on the property sheet.
//
BOOL OnWin7DevicePropPageApply(HWND hWnd, PSHNOTIFY* phdr)
{
	// Get the property sheet extension object pointer that was stored 
	// in the page dialog. (see OnInitPropPageDialog)
    CWin7DevicePropSheetExt *pPropSheetExt = (CWin7DevicePropSheetExt*)
		GetProp(hWnd, THIS_POINTER_PROP);

	// Access the property sheet extension object
	// ...

	// Return PSNRET_NOERROR to allow the property dialog to close if the 
	// user clicked OK
    SetWindowLong(hWnd, DWL_MSGRESULT, PSNRET_NOERROR);

	return TRUE;
}


//
//   FUNCTION: OnWin7DevicePropPageDestroy(HWND)
//
//   PURPOSE: Process the WM_DESTROY message of the property page.
//
void OnWin7DevicePropPageDestroy(HWND hWnd)
{
	// Remove the property from the page.
	RemoveProp(hWnd, THIS_POINTER_PROP);
}


//
//   FUNCTION: Win7DevicePropPageDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//   PURPOSE: Processes messages for the property page.
//
INT_PTR CALLBACK Win7DevicePropPageDlgProc(HWND hWnd, UINT uMsg, 
										   WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return OnInitWin7DevicePropPageDialog(hWnd, (HWND)wParam, lParam);

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_CHANGEWIN7DEVICEPROP_BN:
				// Simulate property changing.
				// Inform the property sheet to enable the Apply button
				SendMessage(GetParent(hWnd), PSM_CHANGED, (WPARAM)hWnd, 0);
				break;
			}
		}

	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case PSN_APPLY:
				return OnWin7DevicePropPageApply(hWnd, (PSHNOTIFY*)lParam);
			}
		}

	case WM_DESTROY:
		OnWin7DevicePropPageDestroy(hWnd);
		return FALSE;

	default:
		return FALSE;	// Let system deal with the message
	}
}


//
//   FUNCTION: Win7DevicePropPageCallbackProc(HWND, UINT, LPPROPSHEETPAGE)
//
//   PURPOSE: Specifies an application-defined callback function that a 
//            property sheet calls when a page is created and when it is 
//            about to be destroyed. An application can use this function to 
//            perform initialization and cleanup operations for the page.
//
UINT CALLBACK Win7DevicePropPageCallbackProc(HWND hWnd, UINT uMsg, 
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
			CWin7DevicePropSheetExt *pPropSheetExt = (CWin7DevicePropSheetExt*)
				ppsp->lParam;

			if(pPropSheetExt)
				pPropSheetExt->Release();
		}
		break;
    }

	return FALSE;
}