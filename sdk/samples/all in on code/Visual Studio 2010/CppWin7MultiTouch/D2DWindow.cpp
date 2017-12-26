/****************************** Module Header ******************************\
* Module Name:  D2DWindow.cpp
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

#include "StdAfx.h"
#include "D2DWindow.h"


D2DWindow::D2DWindow(HINSTANCE hInstance, HWND hParentWnd)
	: hInstance(hInstance),
	m_hParentWnd(hParentWnd),
	m_pD2DFactory(NULL),
	m_pDWriteFactory(NULL),
	m_pWICFactory(NULL),
	m_pRibbonFramework(NULL),
	m_pRenderTarger(NULL),
	m_pFormatConverter(NULL),
	m_pBitmap(NULL),
	m_pTextBrush(NULL),
	m_pTextFormat(NULL),
	m_pRibbonApplication(NULL),
	m_gestureStartPoint(POINTS()),
	m_startDistance(0),
	m_startTransformMatrix(D2D1::Matrix3x2F(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f)),
	m_currentTransformMatrix(D2D1::Matrix3x2F(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f)),
	m_pGestureInfoText(NULL),
	m_bSkew(false),
	m_WindowOffsetY(0)
{
}

D2DWindow::~D2DWindow(void)
{
	this->DiscardDeviceResources();
	SafeRelease(&this->m_pD2DFactory);
	SafeRelease(&this->m_pDWriteFactory);
	SafeRelease(&this->m_pWICFactory);
	SafeRelease(&this->m_pRibbonFramework);
	SafeRelease(&this->m_pBitmap);
	SafeRelease(&this->m_pTextFormat);
	SafeRelease(&this->m_pRibbonApplication);
}

void D2DWindow::Reset()
{
	this->m_gestureStartPoint = POINTS();
	this->m_startDistance = 0;
	this->m_startTransformMatrix = D2D1::Matrix3x2F(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	this->m_currentTransformMatrix = D2D1::Matrix3x2F(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	this->m_bSkew = false;
}

// Performs the intialization tasks such as creating devices.
HRESULT D2DWindow::Initialize()
{
	HRESULT hr = this->CreateDeviceIndependentResources();
	
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Device independent resources intialization failed! Cannot run the program!",
			L"Critical Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	hr = this->InitWindow();

	if (SUCCEEDED(hr))
	{
		// The application can continue to run even if the image fails to open.
		this->OpenImage();
	}
	return hr;
}

// Creates the Direct2D window.
HRESULT D2DWindow::InitWindow()
{
	D2D_SIZE_U size = this->CalculateD2DWindowSize();

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= D2DWindow::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(LONG_PTR);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= L"D2DWindow";
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);

	this->m_hWnd = CreateWindow(
		L"D2DWindow",
		L"",
		WS_CHILDWINDOW | WS_VISIBLE,
		0,
		g_RibbonHeight,
		size.width,
		size.height,
		this->m_hParentWnd,
		NULL,
		hInstance,
		this);
	return this->m_hWnd ? S_OK : E_FAIL;
}

// Create the Direct2D/DirectWrite/WIC/Ribbon factories and the DirectWrite text format.
HRESULT D2DWindow::CreateDeviceIndependentResources()
{
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&this->m_pD2DFactory);
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&this->m_pWICFactory));
	}
	if (SUCCEEDED(hr))
	{
		hr = this->m_pDWriteFactory->CreateTextFormat(
			L"Gabriola",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			24,
			L"en-us",
			&this->m_pTextFormat);
	}
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_UIRibbonFramework,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&this->m_pRibbonFramework));
	}
	return hr;
}

// Creates the Direct2D render target, a solid color brush, the WIC format converter, and the bitmap.
HRESULT D2DWindow::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	if (this->m_pRenderTarger == NULL)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);		
		hr = this->m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(this->m_hWnd, size),
			&this->m_pRenderTarger);
		if (SUCCEEDED(hr))
		{
			hr = this->m_pRenderTarger->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&this->m_pTextBrush);
		}
	}
	if (SUCCEEDED(hr) && (this->m_pFormatConverter != NULL) && (this->m_pBitmap == NULL))
	{
		hr = this->m_pRenderTarger->CreateBitmapFromWicBitmap(
			this->m_pFormatConverter,
			NULL,
			&this->m_pBitmap);
		if (SUCCEEDED(hr))
		{
			D2D1_SIZE_F imageSize = this->m_pBitmap->GetSize();
			this->m_ImageCenter.x = imageSize.width / 2;
			this->m_ImageCenter.y = imageSize.height / 2;
		}
	}
	return hr;
}

void D2DWindow::DiscardDeviceResources()
{
	SafeRelease(&this->m_pRenderTarger);
	SafeRelease(&this->m_pFormatConverter);
	SafeRelease(&this->m_pTextBrush);
}

// Display an open file dialog and open the selected image.
HRESULT D2DWindow::OpenImage()
{
	IFileOpenDialog *pFileOpenDialog = NULL;
	IShellItem *pPicturesFolder = NULL;
	IShellItem *pSelectedFile = NULL;
	LPWSTR filePath = NULL;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pFrame = NULL;

	// Display the open file dialog.
	HRESULT hr = CoCreateInstance(
		__uuidof(FileOpenDialog),
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pFileOpenDialog));
	if (SUCCEEDED(hr))
	{
		// For simplicity. This sample only supports jpg files.
		COMDLG_FILTERSPEC filter[] =
		{ 
			{ L"jpg", L"*.jpg;*.jpeg" },
		};
		hr = pFileOpenDialog->SetFileTypes(1, filter);
	}
	if (SUCCEEDED(hr))
	{
		// Defaults to the Pictures library.
		hr = SHCreateItemInKnownFolder(
			FOLDERID_PicturesLibrary,
			KF_FLAG_DEFAULT,
			NULL,
			IID_PPV_ARGS(&pPicturesFolder));
	}
	if (SUCCEEDED(hr))
	{
		hr = pFileOpenDialog->SetDefaultFolder(pPicturesFolder);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFileOpenDialog->Show(NULL);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFileOpenDialog->GetResult(&pSelectedFile);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSelectedFile->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	}

	// Create the WIC decoder. Use on load cache so the file will be closed immediately.
	if (SUCCEEDED(hr))
	{
		hr = this->m_pWICFactory->CreateDecoderFromFilename(
			filePath,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder);
		CoTaskMemFree(filePath);
	}
	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pFrame);
	}
	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			L"Cannot decode the image. Please make sure it is a valid image file.",
			L"Canno Decode Image",
			MB_OK | MB_ICONERROR);
	}
	if (SUCCEEDED(hr))
	{
		// We must recreate the render target in order to display the new image.
		SafeRelease(&this->m_pBitmap);
		this->DiscardDeviceResources();
		this->CreateDeviceResources();
		this->Reset();
		hr = this->m_pWICFactory->CreateFormatConverter(&this->m_pFormatConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = this->m_pFormatConverter->Initialize(
			pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeCustom);
	}
	if (SUCCEEDED(hr))
	{
		InvalidateRect(this->m_hWnd, NULL, true);
	}

	// Clean up.
	SafeRelease(&pFileOpenDialog);
	SafeRelease(&pPicturesFolder);
	SafeRelease(&pSelectedFile);
	SafeRelease(&pDecoder);
	SafeRelease(&pFrame);
	return hr;
}

// The main rendering method.
LRESULT D2DWindow::OnRender()
{
	HRESULT hr = this->CreateDeviceResources();
	if (SUCCEEDED(hr) && !(this->m_pRenderTarger->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		this->m_pRenderTarger->BeginDraw();
		// Clear the background to white.
		this->m_pRenderTarger->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// Set the transform.
		D2D1::Matrix3x2F transformMatrix = this->m_currentTransformMatrix;
		if (this->m_bSkew)
		{
			transformMatrix = transformMatrix * D2D1::Matrix3x2F::Skew(
				30.0f,
				30.0f,
				D2D1::Point2F(transformMatrix._31, transformMatrix._32));
		}
		this->m_pRenderTarger->SetTransform(transformMatrix);
		D2D1_RECT_F rc = {};
		if (this->m_pBitmap != NULL)
		{
			D2D1_SIZE_F size = this->m_pBitmap->GetSize();
			rc = D2D1::RectF(0.0f, 0.0f, size.width, size.height);
			// Render the bitmap.
			this->m_pRenderTarger->DrawBitmap(this->m_pBitmap, rc);
		}
		// Display the current gesture information.
		if (this->m_pGestureInfoText != NULL)
		{
			this->m_pRenderTarger->SetTransform(D2D1::Matrix3x2F::Identity());
			rc.left = 50.0f;
			rc.top = 20.0f;
			rc.right = 300.0f;
			rc.bottom = 100.0f;
			this->m_pRenderTarger->DrawTextW(
				this->m_pGestureInfoText,
				(UINT32)wcslen(this->m_pGestureInfoText),
				this->m_pTextFormat,
				rc,
				this->m_pTextBrush);
		}

		hr = this->m_pRenderTarger->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
			this->DiscardDeviceResources();
			InvalidateRect(this->m_hWnd, NULL, true);
		}
	}
	return 0;
}

// By default, other than single finger pan and rotate, all other gestures are enabled.
// Handle WM_GESTURENOTIFY to override the behavior.
// This sample disables GC_PAN_WITH_GUTTER, while enables all other gestures.
// If GC_PAN_WITH_GUTTER is enabled, when you pan the image, touch engine tries its best to move the image along with a horizontal or vertical line.
LRESULT D2DWindow::OnGestureNotify(LPARAM lParam)
{
	DWORD panWant = GC_PAN
		| GC_PAN_WITH_SINGLE_FINGER_VERTICALLY
		| GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY
		| GC_PAN_WITH_INERTIA;
	GESTURECONFIG gestureConfig[] =
	{
		{ GID_PAN, panWant, GC_PAN_WITH_GUTTER },
		{ GID_ZOOM, GC_ZOOM, 0 },
		{ GID_ROTATE, GC_ROTATE, 0},
		{ GID_TWOFINGERTAP, GC_TWOFINGERTAP, 0 },
		{ GID_PRESSANDTAP, GC_PRESSANDTAP, 0 }
	};
	SetGestureConfig(this->m_hWnd, 0, 5, gestureConfig, sizeof(GESTURECONFIG));
	return 0;
}

// Handles gesture messages.
LRESULT D2DWindow::OnGesture(LPARAM lParam)
{
	GESTUREINFO gestureInfo;
	ZeroMemory(&gestureInfo, sizeof(gestureInfo));
	gestureInfo.cbSize = sizeof(gestureInfo);
	GetGestureInfo((HGESTUREINFO)lParam, &gestureInfo);

	switch (gestureInfo.dwID)
	{
	// Translate:
	case GID_PAN:
		this->m_pGestureInfoText = L"Detected gesture: Pan.";
		this->OnTranslate(gestureInfo);
		break;

	// Scale:
	case GID_ZOOM:
		this->m_pGestureInfoText = L"Detected gesture: Zoom.";
		this->OnScale(gestureInfo);
		break;

	// Rotate:
	case GID_ROTATE:
		this->m_pGestureInfoText = L"Detected gesture: Rotate.";
		this->OnRotate(gestureInfo);
		break;

	// Two finger tap:
	case GID_TWOFINGERTAP:
	case GID_PRESSANDTAP:
		if (gestureInfo.dwID == GID_TWOFINGERTAP)
		{
			this->m_pGestureInfoText = L"Detected gesture: TwoFingerTap.";
		}
		else
		{
			this->m_pGestureInfoText = L"Detected gesture: PressAndTap.";
		}
		// Two finger tap can usually be translated to mouse double click. Press and 
		// tap can usually be translated to mouse right click.
		// There's no pre-defined behavior assoicated with those two gestures. You can do whatever you like.
		// In this sample, we toggle the skew of the image with those two gestures.
		this->m_bSkew = !this->m_bSkew;
		InvalidateRect(this->m_hWnd, NULL, true);
		break;

	default:
		break;
	}
	CloseGestureInfoHandle((HGESTUREINFO)lParam);
	return 0;
}

// Translate the image.
void D2DWindow::OnTranslate(GESTUREINFO gestureInfo)
{
	switch (gestureInfo.dwFlags)
	{
	case GF_BEGIN:
		this->m_gestureStartPoint = gestureInfo.ptsLocation;
		this->m_startTransformMatrix = this->m_currentTransformMatrix;
		this->m_StartImageCenter = this->m_ImageCenter;
		break;
	default:
		POINTS gestureCurrentPoint = gestureInfo.ptsLocation;

		// GESTUREINFO.ptsLocation represents the current touched point.
		// Minuse the start point of the gesture to get the translation.
		float translateX = gestureCurrentPoint.x - this->m_gestureStartPoint.x;
		float translateY = gestureCurrentPoint.y - this->m_gestureStartPoint.y;

		// Multiplies the new translate matrix.
		this->m_currentTransformMatrix = this->m_startTransformMatrix
			* D2D1::Matrix3x2F::Translation(translateX, translateY);
		this->m_ImageCenter = this->m_StartImageCenter
			* D2D1::Matrix3x2F::Translation(translateX, translateY);

		InvalidateRect(this->m_hWnd, NULL, true);
		break;
	}
}

// Scales the image.
void D2DWindow::OnScale(GESTUREINFO gestureInfo)
{
	switch (gestureInfo.dwFlags)
	{
	case GF_BEGIN:
		this->m_startDistance = gestureInfo.ullArguments;
		this->m_startTransformMatrix = this->m_currentTransformMatrix;
		this->m_StartImageCenter = this->m_ImageCenter;
		break;
	default:
		if (this->m_startDistance != 0)
		{
			// GESTUREINFO.ullArguments represents the distance between two fingers.
			// Devides it with the start distance to get the scale.
			// When scaling using finger, we usually do a uniform scaling, where ScaleX == ScaleY.
			float scale = (float)LODWORD(gestureInfo.ullArguments) / (float)this->m_startDistance;
			// GESTUREINFO.ptsLocation represents the current touched point,
			// which serves as the transform origin.
			// However, it returns the transform origin in the screen coordinate,
			// which must be transformed to the window coordinate.
			POINT p;
			p.x = gestureInfo.ptsLocation.x;
			p.y = gestureInfo.ptsLocation.y;
			ScreenToClient(this->m_hWnd, &p);

			// Multiplies the new scale matrix.
			this->m_currentTransformMatrix = this->m_startTransformMatrix
				* D2D1::Matrix3x2F::Scale(scale, scale, D2D1::Point2F(p.x, p.y));
			this->m_ImageCenter = this->m_StartImageCenter
				* D2D1::Matrix3x2F::Scale(scale, scale, D2D1::Point2F(p.x, p.y));

			InvalidateRect(this->m_hWnd, NULL, true);
		}
		break;
	}
}

// Rotates the image.
void D2DWindow::OnRotate(GESTUREINFO gestureInfo)
{
	switch (gestureInfo.dwFlags)
	{
	case GF_BEGIN:
		this->m_startTransformMatrix = this->m_currentTransformMatrix;
		this->m_StartImageCenter = this->m_ImageCenter;
		break;
	default:
		// Use the GID_ROTATE_ANGLE_FROM_ARGUMENT macro to calculate the rotate angle.
		// In Direct2D, a positive angle results in a clockwise rotation,
		// which is consistent with WPF and Silverlight.
		// But the rotate gesture assumes a positive angle results in a counter-clockwise rotation,
		// which is consistent with GDI.
		// Since we're using Direct2D, we must get the opposite number.
		// In addition, Direct2D's rotate transform uses degrees instead of radians.
		// So we must convert the radians angle to a degree angle.
		float angle = -GID_ROTATE_ANGLE_FROM_ARGUMENT((double)LODWORD(gestureInfo.ullArguments))
			* 180 / 3.14159265;

		// GESTUREINFO.ptsLocation represents the current touched point,
		// which serves as the transform origin.
		// However, it returns the transform origin in the screen coordinate,
		// which must be transformed to the window coordinate.
		POINT p;
		p.x = gestureInfo.ptsLocation.x;
		p.y = gestureInfo.ptsLocation.y;
		ScreenToClient(this->m_hWnd, &p);

		// Multiplies the new rotate matrix.
		this->m_currentTransformMatrix = this->m_startTransformMatrix
			* D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(p.x, p.y));
		this->m_ImageCenter = this->m_StartImageCenter
			* D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(p.x, p.y));

		InvalidateRect(this->m_hWnd, NULL, true);
		break;
	}
}

// Calculates the Direct2D window's size.
D2D1_SIZE_U D2DWindow::CalculateD2DWindowSize()
{
	RECT rc;
	GetClientRect(this->m_hParentWnd, &rc);
	D2D1_SIZE_U size;
	size.width = rc.right;
	size.height = rc.bottom - g_RibbonHeight;
	return size;
}

void D2DWindow::OnResize()
{
	if (this->m_pRenderTarger != NULL)
	{
		D2D_SIZE_U size = this->CalculateD2DWindowSize();
		MoveWindow(this->m_hWnd, 0, g_RibbonHeight, size.width, size.height, true);
		this->m_pRenderTarger->Resize(size);
	}
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
LRESULT CALLBACK D2DWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	LPCREATESTRUCT createStruct;
	D2DWindow *pD2DWindow = NULL;

	// Get the instance of MainWindow to use in the static window proc function.
	if (message == WM_CREATE)
	{
		createStruct = (LPCREATESTRUCT)lParam;
		pD2DWindow = (D2DWindow *)createStruct->lpCreateParams;
		SetWindowLongW(hWnd, GWLP_USERDATA, PtrToUlong(pD2DWindow));
		return 1;
	}
	else
	{
		pD2DWindow = reinterpret_cast<D2DWindow *>(GetWindowLongW(hWnd, GWLP_USERDATA));
		if (pD2DWindow != NULL)
		{
			switch (message)
			{
			case WM_PAINT:
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				pD2DWindow->OnRender();
				EndPaint(hWnd, &ps);
				break;
			case WM_GESTURE:
				pD2DWindow->OnGesture(lParam);
				break;
			case WM_GESTURENOTIFY:
				pD2DWindow->OnGestureNotify(lParam);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//The following methods are for mouse simulation.

// Translate the image using ribbon.
void D2DWindow::Translate(float x, float y)
{
	this->m_currentTransformMatrix = this->m_currentTransformMatrix
		* D2D1::Matrix3x2F::Translation(x, y);
	this->m_ImageCenter = this->m_ImageCenter
		* D2D1::Matrix3x2F::Translation(x, y);
	InvalidateRect(this->m_hWnd, NULL, true);
}

// Scale the image using ribbon.
// The transform origin is always the center of the image,
// so m_ImageCenter does not need to be updated.
void D2DWindow::Scale(float scale)
{
	D2D1_POINT_2F transformOrigin = this->GetImageCenterPoint();
	this->m_currentTransformMatrix = this->m_currentTransformMatrix
		* D2D1::Matrix3x2F::Scale(scale, scale, transformOrigin);
	InvalidateRect(this->m_hWnd, NULL, true);
}

// Rotate the image using ribbon.
// The transform origin is always the center of the image,
// so m_ImageCenter does not need to be updated.
void D2DWindow::Rotate(float angle)
{
	D2D1_POINT_2F transformOrigin = this->GetImageCenterPoint();
	this->m_currentTransformMatrix = this->m_currentTransformMatrix
		* D2D1::Matrix3x2F::Rotation(angle, transformOrigin);
	InvalidateRect(this->m_hWnd, NULL, true);
}

// Returns the center point of the image.
// The ribbon commands always transfrom the image on the center.
D2D1_POINT_2F D2DWindow::GetImageCenterPoint()
{
	return this->m_ImageCenter;
}
