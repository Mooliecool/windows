/****************************** Module Header ******************************\
* Module Name:  CppWin7TaskbarProgressBar.cpp
* Project:      CppWin7TaskbarProgressBar
* Copyright (c) Microsoft Corporation.
* 
* Windows 7 Taskbar introduces Taskbar Progress Bar, which makes your 
* application can provide contextual status information to the user even if 
* the application's window is not shown.  The user doesn't even have to 
* look at the thumbnail or the live preview of your app ¨C the taskbar button 
* itself can reveal whether you have any interesting status updates.
* 
* CppWin7TaskbarProgressBar example demostrates how to initialize Windows 7 
* Taskbar list instance, set Taskbar ProgressBar state and value using 
* ITaskbarList3 related APIs.
* 
* This Main dialog enables the user to set Taskbar ProgressBar state (normal, 
* pause, indeterminate, error), progress value, and flash window.
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
#include "CppWin7TaskbarProgressBar.h"
#pragma endregion

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;							// Current instance
TCHAR szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];		// The main window class name
ITaskbarList3 *g_pTaskbarList = NULL;		// Windows 7 Taskbar list instance
HWND hProgressBar;							// Window handle of the ProgressBar control

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

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
	LoadString(hInstance, IDC_CPPWIN7TASKBARPROGRESSBAR, szWindowClass, MAX_LOADSTRING);
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPWIN7TASKBARPROGRESSBAR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

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

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Find the ProgressBar window handle
	hProgressBar = FindWindowEx(hWnd, NULL, L"msctls_progress32", NULL);
	if (!hProgressBar)
	{
		return FALSE;
	}

	HRESULT hr;

	// Initialize the Windows 7 Taskbar list instance
	hr = CoCreateInstance(CLSID_TaskbarList, 
		NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pTaskbarList));
	if (!SUCCEEDED(hr))
	{
		return FALSE;
	}

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
	case IDC_START_BN:

		/////////////////////////////////////////////////////////////////////
		// Set ProgressBar control and Taskbar ProgressBar to NORMAL state
		//

		// Set the ProgressBar control to the NORMAL state and half of the 
		// progress value
		SendMessage(hProgressBar, PBM_SETSTATE, (WPARAM)PBST_NORMAL, 0);
		SendMessage(hProgressBar, PBM_SETPOS, (WPARAM)50, 0);

		// Set the Taskbar ProgressBar to the NORMAL state and half of the
		// progress value
		g_pTaskbarList->SetProgressState(hWnd, TBPF_NORMAL);
		g_pTaskbarList->SetProgressValue(hWnd, (ULONG)50, (ULONG)100);

		break;

	case IDC_PAUSE_BN:

		/////////////////////////////////////////////////////////////////////
		// Set ProgressBar control and Taskbar ProgressBar to PAUSED state
		//

		// Set the ProgressBar control to the PAUSED state and half of the 
		// progress value
		SendMessage(hProgressBar, PBM_SETPOS, (WPARAM)50, 0);
		SendMessage(hProgressBar, PBM_SETSTATE, (WPARAM)PBST_PAUSED, 0);

		// Set the Taskbar ProgressBar to the PAUSED state and half of the
		// progress value
		g_pTaskbarList->SetProgressState(hWnd, TBPF_PAUSED);
		g_pTaskbarList->SetProgressValue(hWnd, (ULONG)50, (ULONG)100);

		break;

	case IDC_INDETERMINAT_BN:

		/////////////////////////////////////////////////////////////////////
		// Set Taskbar ProgressBar to INDETERMINATE state
		//

		g_pTaskbarList->SetProgressState(hWnd, TBPF_INDETERMINATE);

		break;

	case IDC_ERROR_BN:

		/////////////////////////////////////////////////////////////////////
		// Set ProgressBar control and Taskbar ProgressBar to ERROR state
		//

		// Set the ProgressBar control to the ERROR state and half of the 
		// progress value
		SendMessage(hProgressBar, PBM_SETPOS, (WPARAM)50, 0);
		SendMessage(hProgressBar, PBM_SETSTATE, (WPARAM)PBST_ERROR, 0);

		// Set the Taskbar ProgressBar to the ERROR state and half of the
		// progress value
		g_pTaskbarList->SetProgressState(hWnd, TBPF_ERROR);
		g_pTaskbarList->SetProgressValue(hWnd, (ULONG)50, (ULONG)100);

		break;

	case IDC_FLASH_BN:

		/////////////////////////////////////////////////////////////////////
		// Flash the window for one time
		//

		FlashWindow(hWnd, true);

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
	// Release the Taskbar list instance
	g_pTaskbarList->Release();

	PostQuitMessage(0);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
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