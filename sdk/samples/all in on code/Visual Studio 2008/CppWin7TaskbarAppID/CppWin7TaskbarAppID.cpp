/****************************** Module Header ******************************\
* Module Name:  CppWin7TaskbarAppID.cpp
* Project:      CppWin7TaskbarAppID
* Copyright (c) Microsoft Corporation.
* 
* Application User Model IDs (AppUserModelIDs) are used extensively by the 
* taskbar in Windows 7 and later systems to associate processes, files, and 
* windows with a particular application. In some cases, it is sufficient to 
* rely on the internal AppUserModelID assigned to a process by the system. 
* However, an application that owns multiple processes or an application that 
* is running in a host process might need to explicitly identify itself so 
* that it can group its otherwise disparate windows under a single taskbar 
* button and control the contents of that application's Jump List.
* 
* CppWin7TaskbarAppID example demostrates how to set process level Application
* User Model IDs (AppUserModelIDs or AppIDs) and modify the AppIDs for a 
* specific window using native C++.
* 
* This Main dialog sets a process level AppID for the current process.  It 
* can create some Sub dialogs and set specific AppIDs for these Sub dialogs, 
* so the Sub dialogs will be in different group of the Main dialog's Taskbar 
* button.  It also can modify the Main dialog AppIDs. Each dialog's caption
* shows its corresponding AppID.
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
#include "CppWin7TaskbarAppID.h"
#pragma endregion


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// Current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ModalessDlgProc(HWND, UINT, WPARAM, LPARAM);
void				SetAppIDForSpecificWindow(HWND hWnd, int iAppID);

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CPPWIN7TASKBARAPPID, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPWIN7TASKBARAPPID));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


// PSWSTR array holding the AppIDs
PCWSTR c_rgszAppID[] = 
{
	L"All-In-One Code Framework.Win7.CppWin7TaskbarAppID.App1",
	L"All-In-One Code Framework.Win7.CppWin7TaskbarAppID.App2"
};


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG), 0, 0);

	if (!hWnd)
	{
		return FALSE;
	}

	// Set process-level AppID
	HRESULT hr = SetCurrentProcessExplicitAppUserModelID(c_rgszAppID[0]);

	if (!SUCCEEDED(hr))
	{
		return FALSE;
	}

	// Set Window caption
	if (!SetWindowText(hWnd, c_rgszAppID[0]))
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//   FUNCTION: OnCreate(HWND, LPCREATESTRUCT)
//
//   PURPOSE: Process the WM_CREATE message
//
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	return TRUE;
}


//
//   FUNCTION: OnCommand(HWND, int, HWND, UINT)
//
//   PURPOSE: Process the WM_COMMAND message
//
void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_SHOWDIALOG_BN:

		/////////////////////////////////////////////////////////////////////
		// Create a Sub Modeless Dialog
		// 

		// You create a modeless dialog box by using the CreateDialog 
		// function, specifying the identifier or name of a dialog box 
		// template resource and a pointer to the dialog box procedure. 
		// CreateDialog loads the template, creates the dialog box, and 
		// optionally displays it. Your application is responsible for 
		// retrieving and dispatching user input messages to the dialog box 
		// procedure.
		{
			HWND hDlg = CreateDialog(hInst, 
				MAKEINTRESOURCE(IDD_SUBDIALOG), 
				hWnd, ModalessDlgProc);
			if (hDlg)
			{
				// Set AppID (App2) for this Sub Dialog
				SetAppIDForSpecificWindow(hDlg, 1);

				// Set Sub Dialog caption
				SetWindowText(hDlg, c_rgszAppID[1]);

				ShowWindow(hDlg, SW_SHOW);
			}
		}
		break;

	case IDC_SETAPPID1_BN:

		/////////////////////////////////////////////////////////////////////
		// Set Main dialog AppID to 
		// All-In-One Code Framework.Win7.CppWin7TaskbarAppID.App1
		// 

		// Set the AppID (App1) for Main Dialog
		SetAppIDForSpecificWindow(hWnd, 0);

		// Set Main Dialog caption
		SetWindowText(hWnd, c_rgszAppID[0]);
		break;

	case IDC_SETAPPID2_BN:

		/////////////////////////////////////////////////////////////////////
		// Set Main dialog AppID to 
		// All-In-One Code Framework.Win7.CppWin7TaskbarAppID.App2
		// 

		// Set the AppID (App2) for Main Dialog
		SetAppIDForSpecificWindow(hWnd, 1);

		// Set Main Dialog caption 
		SetWindowText(hWnd, c_rgszAppID[1]);
		break;

	case IDOK:
		PostQuitMessage(0);
		break;
	}
}


//
//   FUNCTION: OnDestroy(HWND)
//
//   PURPOSE: Process the WM_DESTROY message
//
void OnDestroy(HWND hWnd)
{
	// Clear the specific window's AppID
	SetAppIDForSpecificWindow(hWnd, -1);
	PostQuitMessage(0);
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_CREATE	- a window is being created
//  WM_COMMAND	- process the application commands
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Handle the WM_CREATE message in OnCreate
		// Because it is a window based on a dialog resource but NOT a dialog 
		// box it receives a WM_CREATE message and NOT a WM_INITDIALOG message.
		HANDLE_MSG (hWnd, WM_CREATE, OnCreate);

		// Handle the WM_COMMAND message in OnCommand
		HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

		// Handle the WM_DESTROY message in OnDestroy
		HANDLE_MSG (hWnd, WM_DESTROY, OnDestroy);

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//
//  FUNCTION: ModalessDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the modaless dialog.
//
//
INT_PTR CALLBACK ModalessDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			return DestroyWindow(hWnd);
		}

	case WM_CLOSE:
		return DestroyWindow(hWnd);

	default:
		return FALSE;	// Let system deal with the message
	}
}


//
//  FUNCTION: SetAppIDForSpecificWindow(HWND hWnd, int iAppID)
//
//  PURPOSE:  Set AppID for a specific window.
//
//
void SetAppIDForSpecificWindow(HWND hWnd, int iAppID)
{
	IPropertyStore *pps;

	// This api retrieves an IPropertyStore that stores the window's properties
	HRESULT hr = SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&pps));
	if (SUCCEEDED(hr))
	{
		PROPVARIANT pv;
		if (iAppID >= 0)
		{
			// Creates a VT_LPWSTR propvariant
			hr = InitPropVariantFromString(c_rgszAppID[iAppID], &pv);
		}
		else
		{
			// Initializes a PROPVARIANT structure
			PropVariantInit(&pv);
		}
		if (SUCCEEDED(hr))
		{
			// Set the PROPVARIANT structure to PKEY_AppUserModel_ID property
			hr = pps->SetValue(PKEY_AppUserModel_ID, pv);

			// Clear the PROPVARIANT structure
			PropVariantClear(&pv);
		}
		// Release the IPropertyStore
		pps->Release();
	}
}
