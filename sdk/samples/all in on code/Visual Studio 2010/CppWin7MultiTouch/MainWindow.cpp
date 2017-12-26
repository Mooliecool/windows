/****************************** Module Header ******************************\
* Module Name:  MainWindow.cpp
* Project:      CppWin7MultiTouch
* Copyright (c) Microsoft Corporation.
* 
* The main window. The container of the ribbon and the Direct2D window.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "StdAfx.h"
#include "MainWindow.h"
#include "winerror.h"


MainWindow::MainWindow(HINSTANCE hInstance)
	: hInstance(hInstance),
	m_pRibbonFramework(NULL),
	m_pRibbonApplication(NULL),
	m_hWnd(NULL),
	m_pD2DWindow(NULL),
	m_pD2DFactory(NULL)
{
}

MainWindow::~MainWindow(void)
{
	if (this->m_pRibbonFramework != NULL)
	{
		this->m_pRibbonFramework->Destroy();
	}
	SafeRelease(&this->m_pRibbonFramework);
	SafeRelease(&this->m_pRibbonApplication);
	if (this->m_pD2DWindow != NULL)
	{
		delete this->m_pD2DWindow;
	}
}

// Performs the intialization tasks such as create devices and windows.
HRESULT MainWindow::Initialize()
{
	// We use the Direct2D factory to get desktop DPI.
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->m_pD2DFactory);
	if (SUCCEEDED(hr))
	{
		hr = this->InitializeWindow();
	}
	if (SUCCEEDED(hr))
	{
		hr = this->InitializeRibbon();
	}
	if (SUCCEEDED(hr))
	{
		hr = this->InitializeChildWindow();
	}
	if (SUCCEEDED(hr))
	{
		// Resize the window to get the ribbon's height.
		RECT rc;
		GetClientRect(this->m_hWnd, &rc);
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		PostMessage(this->m_hWnd, WM_SIZE, 0, 0);
	}
	return hr;
}

// Creates main window.
HRESULT MainWindow::InitializeWindow()
{	
	FLOAT dpiX, dpiY;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= MainWindow::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPWIN7MULTITOUCH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= L"MainWindow";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&wcex);

	this->m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);
	// Pass this to lpParam, so we can get the instance of MainWindow in the static window proc function.
	this->m_hWnd = CreateWindow(
		L"MainWindow",
		L"CppWin7MultiTouch",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT * dpiX / 96,
		CW_USEDEFAULT * dpiY / 96,
		NULL,
		NULL,
		hInstance,
		this);
	HRESULT hr = this->m_hWnd ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		ShowWindow(this->m_hWnd, SW_SHOWMAXIMIZED);
		UpdateWindow(this->m_hWnd);
	}
	return hr;
}

// Createss the Direct2D window.
HRESULT MainWindow::InitializeChildWindow()
{	
	this->m_pD2DWindow = new D2DWindow(this->hInstance, this->m_hWnd);
	return this->m_pD2DWindow->Initialize();
}

// Intialize ribbon.
HRESULT MainWindow::InitializeRibbon()
{	
	HRESULT hr = CoCreateInstance(
		CLSID_UIRibbonFramework,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&this->m_pRibbonFramework));
	if (SUCCEEDED(hr))
	{
		SafeRelease(&this->m_pRibbonApplication);
		this->m_pRibbonApplication = new CApplication();
		hr = this->m_pRibbonFramework->Initialize(this->m_hWnd, this->m_pRibbonApplication);
	}
	if (SUCCEEDED(hr))
	{
		hr = this->m_pRibbonFramework->LoadUI(GetModuleHandle(NULL), L"APPLICATION_RIBBON");
	}
	return hr;
}

// Main window's message loop.
void MainWindow::RunMessageLoop()
{
	MSG msg;
	HACCEL hAccelTable;	
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPWIN7MULTITOUCH));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

UINT MainWindow::GetRibbonHeight()
{
	if (this->m_pRibbonApplication == NULL)
	{
		return 0;
	}
	return this->m_pRibbonApplication->m_RibbonHeight;
}

// The main window proc function.
LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	LPCREATESTRUCT createStruct;
	MainWindow *pMainWindow = NULL;

	// Get the instance of MainWindow to use in the static window proc function.
	if (message == WM_CREATE)
	{
		createStruct = (LPCREATESTRUCT)lParam;
		pMainWindow = (MainWindow *)createStruct->lpCreateParams;
		SetWindowLongW(hWnd, GWLP_USERDATA, PtrToUlong(pMainWindow));
		return 1;
	}
	else
	{
		pMainWindow = reinterpret_cast<MainWindow *>(GetWindowLongW(hWnd, GWLP_USERDATA));
		if (pMainWindow != NULL)
		{
			switch (message)
			{
			case WM_SIZE:
				if (pMainWindow->m_pD2DWindow != NULL)
				{
					pMainWindow->m_pD2DWindow->OnResize();
				}
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
