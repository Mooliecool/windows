/****************************** Module Header ******************************\
* Module Name:  Renderer.h
* Project:      CppWin7Direct2D
* Copyright (c) Microsoft Corporation.
* 
* The Renderer class is responsible for most of the rendering task.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/11/2009 14:54 Yilun Luo Created
\***************************************************************************/

#pragma once
class Renderer
{
private:
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pPlanetBackgroundBrush;
	ID2D1SolidColorBrush* m_pStarBrush;
	ID2D1SolidColorBrush* m_pContinentBrush;
	ID2D1SolidColorBrush* m_pSmallStarBrush;
	ID2D1RadialGradientBrush* m_pStartOutlineBrush;
	ID2D1PathGeometry* m_pStarOutline;
	ID2D1PathGeometry* m_pPlanetUpPath;
	ID2D1PathGeometry* m_pPlanetDownPath;
	int m_animateTranslateX;
	bool m_animateToRight;

public:
	Renderer(HWND hwnd, int width, int height);
	~Renderer(void);

	HRESULT Init();
	HRESULT Render();
	void DiscardResources();

	HWND m_hwnd;
	bool m_animate;
	int m_clickedPointX;
	int m_clickedPointY;

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT CreateStarOutline();
	HRESULT CreatePlanetUpPath();
	HRESULT CreatePlanetDownPath();
	HRESULT CreateStarOutlineBrush();
	void DrawPlanet(D2D1_ELLIPSE planet);
};

