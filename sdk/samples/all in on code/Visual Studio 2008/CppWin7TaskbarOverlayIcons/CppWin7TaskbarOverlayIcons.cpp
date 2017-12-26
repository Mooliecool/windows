/****************************** Module Header ******************************\
* Module Name:  CppWin7TaskbarOverlayIcons.cpp
* Project:      CppWin7TaskbarOverlayIcons
* Copyright (c) Microsoft Corporation.
* 
* Windows 7 Taskbar introduces Overlay Icons, which makes your application can 
* provide contextual status information to the user even if the application's 
* window is not shown.  The user doesn't even have to look at the thumbnail 
* or the live preview of your app - the taskbar button itself can reveal 
* whether you have any interesting status updates.
* 
* CppWin7TaskbarOverlayIcons example demostrates how to initialize Windows 7 
* Taskbar list instance, set and clear Taskbar Overlay Icons using 
* ITaskbarList3 related APIs.  
* 
* This Main dialog enables the user select whether to show the Taskbar Overlay
* Icon and which icon to be displayed according to the ComboBox selected status.
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
#include "CppWin7TaskbarOverlayIcons.h"
#pragma endregion

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;						// Current instance
TCHAR szTitle[MAX_LOADSTRING];			// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The main window class name
ITaskbarList3 *g_pTaskbarList = NULL;	// Windows 7 Taskbar list instance
HWND hCombo;							// Window handle of the ComboBox control
BOOL bShowOverlayIcons = FALSE;			// Whether to show Overlay Icons


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				SetOverlayIconsByComboSelectedValue(HWND hWnd);
void				SetOverlayIcons(HWND hWnd, int id);

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
	LoadString(hInstance, IDC_CPPWIN7TASKBAROVERLAYICONS, szWindowClass, MAX_LOADSTRING);
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPWIN7TASKBAROVERLAYICONS));
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

	// Find the ComboBox window handle
	hCombo = FindWindowEx(hWnd, NULL, L"ComboBox", NULL);

	if (hCombo)
	{
		// Add items into the ComboBox
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Available");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Away");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Offline");
	}
	else
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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_STATUS_COMBO:

			///////////////////////////////////////////////////////////////// 
			// Display Taskbar Overly Icons
			//

			// Check if the selection is made and whether it is allowed to 
			// display Overlay Icons
			if (wmEvent == CBN_SELENDOK && bShowOverlayIcons)
			{
				SetOverlayIconsByComboSelectedValue(hWnd);
			}
			break;
		case IDC_SHOWICON_CHECK:

			///////////////////////////////////////////////////////////////// 
			// Enable/Disable the Taskbar Overlay Icons
			//

			bShowOverlayIcons = !bShowOverlayIcons;
			if (!bShowOverlayIcons)
			{
				// Disable the Taskbar Overlay Icons
				g_pTaskbarList->SetOverlayIcon(hWnd, NULL, NULL);
			}
			else
			{
				SetOverlayIconsByComboSelectedValue(hWnd);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:

		// Release the Taskbar list instance
		g_pTaskbarList->Release();

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
//   FUNCTION: SetOverlayIconsByComboSelectedValue(HWND hWnd)
//
//   PURPOSE: Get the selected index of the ComboBox and set the Overlay Icons
//
//
void SetOverlayIconsByComboSelectedValue(HWND hWnd)
{
	// Retrieve the ComboBox selected index
	LRESULT index;
	index = SendMessage(hCombo, CB_GETCURSEL, 0, 0);

	// Display Taskbar Overlay Icons based on the selected index
	switch (index)
	{
	case 0:
		SetOverlayIcons(hWnd, IDI_AVAILABLE_ICON);
		break;
	case 1:
		SetOverlayIcons(hWnd, IDI_AWAY_ICON);
		break;
	case 2:
		SetOverlayIcons(hWnd, IDI_OFFLINE_ICON);
		break;
	default:
		g_pTaskbarList->SetOverlayIcon(hWnd, NULL, NULL);
	}
}


//
//   FUNCTION: SetOverlayIcons(HWND hWnd, int id)
//
//   PURPOSE: Load an icon and set the icon as Taskbar Overlay Icon
//
//
void SetOverlayIcons(HWND hWnd, int id)
{
	HICON hIcon = NULL; // For IDM_OVERLAY_CLEAR
	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(id));

	// Set the window's overlay icon, possibly NULL value
	g_pTaskbarList->SetOverlayIcon(hWnd, hIcon, NULL);

	if (hIcon) 
	{
		// Need to clean up the icon as we no longer need it
		DestroyIcon(hIcon);
	}
}
