/****************************** Module Header ******************************\
* Module Name:  MainWindow.h
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

#pragma once
#include "stdafx.h"
#include "Application.h"
#include "D2DWindow.h"

class MainWindow
{
public:
	MainWindow(HINSTANCE hInstance);
	~MainWindow(void);

	HWND m_hWnd;
	D2DWindow *m_pD2DWindow;

	HRESULT Initialize();
	void RunMessageLoop();
	UINT GetRibbonHeight();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE hInstance;
	ID2D1Factory *m_pD2DFactory;				// The Direct2D factory.
	IUIFramework *m_pRibbonFramework;			// The ribbon framework.
	CApplication *m_pRibbonApplication;			// The ribbon application.

	HRESULT InitializeWindow();
	HRESULT InitializeChildWindow();
	HRESULT InitializeRibbon();
};