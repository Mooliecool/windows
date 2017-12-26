/****************************** Module Header ******************************\
* Module Name:  CppWin7MultiTouch.cpp
* Project:      CppWin7MultiTouch
* Copyright (c) Microsoft Corporation.
* 
* Defines the entry point for the application.
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
#include "StdAfx.h"
#include "CppWin7MultiTouch.h"

MainWindow *g_pMainWindow;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	// Ignore the return value because we want to run the program even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Initialize COM.
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"COM intialization failed! Cannot run the program!",
			L"Critical Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	g_pMainWindow = new MainWindow(hInstance);

	// Perform application initialization:
	if (FAILED(g_pMainWindow->Initialize()))
	{
		return FALSE;
	}
	g_pMainWindow->RunMessageLoop();

	CoUninitialize();
	delete g_pMainWindow;
	return 0;
}