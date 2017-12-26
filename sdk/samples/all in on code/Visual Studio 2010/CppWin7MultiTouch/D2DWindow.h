/****************************** Module Header ******************************\
* Module Name:  D2DWindow.h
* Project:      CppWin7MultiTouch
* Copyright (c) Microsoft Corporation.
* 
* The Direct2D window. All touching and rendering are handled in this class.
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

extern UINT g_RibbonHeight;

class D2DWindow
{
public:
	D2DWindow(HINSTANCE hInstance, HWND hParentWnd);
	~D2DWindow(void);

	HWND m_hWnd;

	ATOM MyRegisterClass(HINSTANCE hInstance);
	HRESULT Initialize();
	HRESULT OpenImage();
	LRESULT OnRender();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnResize();
	void Translate(float x, float y);
	void Scale(float scale);
	void Rotate(float angle);

private:
	HINSTANCE hInstance;
	HWND m_hParentWnd;
	int m_WindowOffsetY;							// The current y offset of this window.
	ID2D1Factory *m_pD2DFactory;					// The Direct2D factory.
	IDWriteFactory *m_pDWriteFactory;				// The DirectWrite factory.
	IWICImagingFactory *m_pWICFactory;				// The WIC factory.
	IUIFramework *m_pRibbonFramework;				// The ribbon framework.
	ID2D1HwndRenderTarget *m_pRenderTarger;			// The Direct2D render target.
	IWICFormatConverter *m_pFormatConverter;		// The WIC format converter.
	ID2D1Bitmap *m_pBitmap;							// The converted Direct2D bitmap.
	ID2D1SolidColorBrush *m_pTextBrush;				// The brush for the text.
	IDWriteTextFormat *m_pTextFormat;				// The DirectWrite text format.
	IUIApplication *m_pRibbonApplication;			// The ribbon application.
	D2D1_POINT_2F m_ImageCenter;					// Center point of the image. Used in mouse simulation, where transform origin is always the center point.
	D2D1_POINT_2F m_StartImageCenter;				// Center point of the image when the transform starts.


	// Global variables that used in transform.
	POINTS m_gestureStartPoint;
	ULONG m_startDistance;
	D2D1::Matrix3x2F m_startTransformMatrix;
	D2D1::Matrix3x2F m_currentTransformMatrix;

	wchar_t *m_pGestureInfoText;
	bool m_bSkew;

	static const UINT_PTR ID_TIMER = 1;

	HRESULT InitWindow();
	void Reset();
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	LRESULT OnGestureNotify(LPARAM lParam);
	LRESULT OnGesture(LPARAM lParam);
	void OnTranslate(GESTUREINFO gestureInfo);
	void OnScale(GESTUREINFO gestureInfo);
	void OnRotate(GESTUREINFO gestureInfo);
	D2D1_SIZE_U CalculateD2DWindowSize();
	D2D1_POINT_2F GetImageCenterPoint();
};

