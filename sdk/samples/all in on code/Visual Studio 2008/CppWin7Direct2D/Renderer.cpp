/****************************** Module Header ******************************\
* Module Name:  Renderer.cpp
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

#include "Stdafx.h"
#include "Renderer.h"

Renderer::Renderer(HWND hwnd, int width, int height)
{
	this->m_hwnd = hwnd;
	this->m_pDirect2dFactory = NULL;
	this->m_pRenderTarget = NULL;
	this->m_animate = false;
	this->m_animateTranslateX = 0;
	this->m_animateToRight = true;
	this->CreateDeviceIndependentResources();
}

Renderer::~Renderer(void)
{
	this->DiscardResources();
}

// Device independent resources are not specific to a particular device. For example, factory and font.
HRESULT Renderer::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;
	// Create Direct 2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	return hr;
}

// Device dependent resources are specific to a particular device. For example,brush may render differently on different devices (hwnd, DXGI (Direct3D surface), etc...).
HRESULT Renderer::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	if(!this->m_pRenderTarget)
	{
		// Create an HwndRenderTarget to draw to the hwnd.
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = this->m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(this->m_hwnd, size),
			&m_pRenderTarget
			);
		if (SUCCEEDED(hr))
		{
			// Create a SolidColorBrush (planet background).
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0x007DD5)),
				&m_pPlanetBackgroundBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			// Create a SolidColorBrush (star).
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Red),
				&m_pStarBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			// Create a SolidColorBrush (continent).
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0x0BFF00)),
				&m_pContinentBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			// Create a SolidColorBrush (small stars).
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White),
				&m_pSmallStarBrush
				);
		}
		// Create a RadialGradientBrush (star outline).
		if (SUCCEEDED(hr))
		{
			hr = CreateStarOutlineBrush();
		}
		// Prepare the geometry for the star's outline.
		if (SUCCEEDED(hr))
		{
			hr = this->CreateStarOutline();
		}
		// Prepare the geometry for the planet's up path.
		if (SUCCEEDED(hr))
		{
			hr = this->CreatePlanetUpPath();
		}
		// Prepare the geometry for the planet's down path.
		if (SUCCEEDED(hr))
		{
			hr = this->CreatePlanetDownPath();
		}
	}
	return hr;
}

// Create a RadialGradientBrush (star outline).
HRESULT Renderer::CreateStarOutlineBrush()
{
	HRESULT hr = S_OK;
	ID2D1GradientStopCollection* gradientStopCollection;
	D2D1_GRADIENT_STOP gradientStops[2];
	gradientStops[0].color = D2D1::ColorF(0xFF7A00);
	gradientStops[0].position = 0.72093f;
	gradientStops[1].color = D2D1::ColorF(0xEBFF00, 0.5f);
	gradientStops[1].position = 1.0f;
	hr = this->m_pRenderTarget->CreateGradientStopCollection(gradientStops, 2, &gradientStopCollection);
	hr = m_pRenderTarget->CreateRadialGradientBrush(
		D2D1::RadialGradientBrushProperties(D2D1::Point2F(95, 95), D2D1::Point2F(0, 0), 95, 95),
		gradientStopCollection,
		&this->m_pStartOutlineBrush
		);
	return hr;
}

//The main rendering function.
HRESULT Renderer::Render()
{
	HRESULT hr = S_OK;
	hr = this->CreateDeviceResources();
	if (SUCCEEDED(hr))
	{
		this->m_pRenderTarget->BeginDraw();

		// Do some clearing.
		this->m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		this->m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	}
	if (SUCCEEDED(hr))
	{
		D2D1_SIZE_F size = m_pRenderTarget->GetSize();
		D2D1_RECT_F rectBackground = D2D1::RectF(0, 0, size.width, size.height);

		//Get the size of the RenderTarget.
		float rtWidth = this->m_pRenderTarget->GetSize().width;
		float rtHeight = this->m_pRenderTarget->GetSize().height;

		//Draw some small stars
		for (int i = 0; i < 300; i++)
		{
			int x = (float)(rand()) / RAND_MAX * rtWidth;
			int y = (float)(rand()) / RAND_MAX * rtHeight;
			D2D1_ELLIPSE smallStar = D2D1_ELLIPSE();
			smallStar.point = D2D1::Point2F(x, y);
			smallStar.radiusX = 1;
			smallStar.radiusY = 1;
			this->m_pRenderTarget->FillEllipse(&smallStar, this->m_pSmallStarBrush);
		}

		D2D1_ELLIPSE planet = D2D1_ELLIPSE();
		planet.point = D2D1::Point2F(100, 100);
		planet.radiusX = 100;
		planet.radiusY = 100;

		// When animating from right to left, draw the planet afte the star so it has a smaller z-index, and will be covered by the star.
		if(!this->m_animateToRight)
		{
			this->DrawPlanet(planet);
		}

		// Draw the star.
		D2D1_ELLIPSE star = D2D1_ELLIPSE();
		star.point = D2D1::Point2F(95, 95);
		star.radiusX = 75;
		star.radiusY = 75;
		// Scale the star, and translate it to the center of the screen. Note if translation is performed before scaling, you'll get different result.
		D2D1::Matrix3x2F scaleMatrix = D2D1::Matrix3x2F::Scale(2, 2, D2D1::Point2F(95, 95));
		D2D1::Matrix3x2F translationMatrix = D2D1::Matrix3x2F::Translation(rtWidth / 2 - 95, rtHeight / 2 - 95);
		this->m_pRenderTarget->SetTransform(scaleMatrix * translationMatrix);
		this->m_pRenderTarget->FillGeometry(this->m_pStarOutline, this->m_pStartOutlineBrush);
		//The transform matrix will be apllied to all rendered elements, until it is reset. So we don't need to set the matrix for the ellipse again.
		this->m_pRenderTarget->FillEllipse(&star, this->m_pStarBrush);

		// By default, or when animating from left to right, draw the planet afte the star so it has a larger z-index.
		if(this->m_animateToRight)
		{
			this->DrawPlanet(planet);
		}

		if(this->m_animate)
		{
			// Perform a hit test. If the user clicked the planet, let's animate it to make it move around the star.
			ID2D1EllipseGeometry* hitTestEllipse;
			this->m_pDirect2dFactory->CreateEllipseGeometry(&planet, &hitTestEllipse);
			D2D1_POINT_2F point;
			point.x = this->m_clickedPointX;
			point.y = this->m_clickedPointY;
			BOOL hit = false;
			D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Translation(10, rtHeight / 2 - 100);
			hitTestEllipse->FillContainsPoint(point, &matrix, &hit);
			if(!hit)
			{
				this->m_animate = false;
			}
			else
			{
				// When moving from left to right, translate transform becomes larger and lager.
				if(this->m_animateToRight)
				{
					this->m_animateTranslateX++;
					if(this->m_animateTranslateX > rtWidth - 220)
					{
						this->m_animateToRight = false;
					}
				}
				else
				{
					// When moving from right to left, translate transform becomes smaller and smaller.
					this->m_animateTranslateX--;
					if(this->m_animateTranslateX <= 0)
					{
						this->m_animateToRight = true;
						this->m_animateTranslateX = 0;
						this->m_animate = false;
					}
				}
				SafeRelease(&hitTestEllipse);
				InvalidateRect(this->m_hwnd, NULL, FALSE);
			}
		}

		// Finish drawing.
		hr = this->m_pRenderTarget->EndDraw();
	}
	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardResources();
	}
	return hr;
}

// Draw the planet.
void Renderer::DrawPlanet(D2D1_ELLIPSE planet)
{
	//Get the size of the RenderTarget.
	float rtWidth = this->m_pRenderTarget->GetSize().width;
	float rtHeight = this->m_pRenderTarget->GetSize().height;

	this->m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(10 + this->m_animateTranslateX, rtHeight / 2 - 100));
	this->m_pRenderTarget->FillEllipse(&planet, this->m_pPlanetBackgroundBrush);
	this->m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(23 + this->m_animateTranslateX, rtHeight / 2 - 121));
	this->m_pRenderTarget->FillGeometry(this->m_pPlanetUpPath, this->m_pContinentBrush);
	this->m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(15 + this->m_animateTranslateX, rtHeight / 2 + 1));
	this->m_pRenderTarget->FillGeometry(this->m_pPlanetDownPath, this->m_pContinentBrush);
}

void Renderer::DiscardResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pPlanetBackgroundBrush);
	SafeRelease(&m_pStarBrush);
	SafeRelease(&m_pContinentBrush);
	SafeRelease(&m_pStartOutlineBrush);
	SafeRelease(&m_pSmallStarBrush);
	SafeRelease(&m_pStarOutline);
	SafeRelease(&m_pPlanetUpPath);
	SafeRelease(&m_pPlanetDownPath);
	SafeRelease(&m_pDirect2dFactory);
}

// This function creates the path geometry that represents the star's outline.
// The data of the path is created in Expression Blend first,
// and then use a PowerShell script to translate the XAML to C++ code.
HRESULT Renderer::CreateStarOutline()
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = this->m_pDirect2dFactory->CreatePathGeometry(&this->m_pStarOutline);
	if (SUCCEEDED(hr))
	{

		hr = this->m_pStarOutline->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F(55.7775869362862,8.51374572801038),
			D2D1_FIGURE_BEGIN_FILLED
			);

		// The following AddBezier code are generated by the PowerShell script.
		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(56.9435916193468,9.34704511572823),
			D2D1::Point2F(53.1105762246886,6.5137471975463),
			D2D1::Point2F(52.4435735457851,16.0137402172507)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(51.7775708708979,25.513733236955),
			D2D1::Point2F(44.4435414150087,26.513732502187),
			D2D1::Point2F(42.9435353904881,26.8470322572889)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(41.4435293659675,27.1803320123907),
			D2D1::Point2F(28.4438771550624,22.0137358086429),
			D2D1::Point2F(22.2772523878566,30.5137295631152)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(16.1105276202493,39.0137233175875),
			D2D1::Point2F(21.7772503796831,48.0137167046759),
			D2D1::Point2F(21.1105477019846,54.5137119286841)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(20.4439450246876,61.0137071526923),
			D2D1::Point2F(6.61048946495231,64.3467047037107),
			D2D1::Point2F(3.77717808543619,67.0137027440846)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(0.943866705920101,69.6807007844584),
			D2D1::Point2F(-0.889540657650585,73.5136979680928),
			D2D1::Point2F(0.443864697746569,77.6806949063147)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(1.77717005274209,81.8466918452714),
			D2D1::Point2F(8.94379883629487,88.8466867018956),
			D2D1::Point2F(8.9438988366965,88.8466867018956)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(11.2772082080391,91.3466848649757),
			D2D1::Point2F(10.7772061998656,100.680678006652),
			D2D1::Point2F(8.44389682852298,102.1806769045)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(6.11048745677877,103.680675802348),
			D2D1::Point2F(-2.38954668217117,114.180668087284),
			D2D1::Point2F(1.61046938321705,120.680663311292)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(5.61048544860525,127.180658535301),
			D2D1::Point2F(11.6105095466876,126.846658780713),
			D2D1::Point2F(15.9439269511259,128.513657555855)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(20.2772443551626,130.180656330997),
			D2D1::Point2F(21.7772503796831,136.346651800417),
			D2D1::Point2F(21.4439490410347,137.846650698265)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(21.1105477019846,139.346649596113),
			D2D1::Point2F(18.4439369919935,151.013641023576),
			D2D1::Point2F(19.9439430165141,154.513638451888)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(21.4439490410347,158.0136358802),
			D2D1::Point2F(23.7772584123772,159.84663453337),
			D2D1::Point2F(23.7772584123772,159.84663453337)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(24.9439630982493,162.180632818422),
			D2D1::Point2F(30.110583849108,162.513632573744),
			D2D1::Point2F(34.2773005840213,161.346633431218)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(38.4435173169264,160.180634287958),
			D2D1::Point2F(44.1105400775651,161.846633063834),
			D2D1::Point2F(44.1105400775651,161.846633063834)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(53.9435795703057,169.013627797752),
			D2D1::Point2F(50.2775648463774,173.346624614003),
			D2D1::Point2F(54.4435815784792,180.013619715305)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(58.6105983145974,186.680614816607),
			D2D1::Point2F(63.6106183963326,185.346615796787),
			D2D1::Point2F(66.7776311161037,184.846616164171)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(69.9436438318585,184.346616531555),
			D2D1::Point2F(77.4436739544614,178.846620572779),
			D2D1::Point2F(79.6106826578854,178.180621062134)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(81.7776913613095,177.513621552225),
			D2D1::Point2F(86.6107107723148,178.846620572779),
			D2D1::Point2F(86.6107107723148,178.846620572779)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(96.7777516065153,189.930612428611),
			D2D1::Point2F(102.943776371311,190.013612367625),
			D2D1::Point2F(105.943788420352,189.680612612303)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(108.943800469394,189.346612857715),
			D2D1::Point2F(115.277825908936,185.013616041465),
			D2D1::Point2F(116.610831262726,180.513619347921)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(117.943836616517,176.013622654377),
			D2D1::Point2F(122.110853352635,173.346624614003),
			D2D1::Point2F(122.943856698252,173.180624735974)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(123.777860047886,173.01362485868),
			D2D1::Point2F(124.777864064233,170.846626450923),
			D2D1::Point2F(131.277890170489,173.180624735974)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(137.777916276744,175.513623021761),
			D2D1::Point2F(144.943945057887,175.680622899054),
			D2D1::Point2F(149.777964472909,173.346624614003)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(154.610983883914,171.013626328216),
			D2D1::Point2F(153.277978530124,163.180632083654),
			D2D1::Point2F(153.277978530124,161.18063355319)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(153.277978530124,159.180635022726),
			D2D1::Point2F(153.110977859394,155.51363771712),
			D2D1::Point2F(156.277990579165,151.180640900869)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(159.44400329492,146.846644085354),
			D2D1::Point2F(171.611052161814,145.013645432183),
			D2D1::Point2F(171.611052161814,145.013645432183)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(180.444087638208,142.680647146397),
			D2D1::Point2F(179.111082284417,136.180651922389),
			D2D1::Point2F(179.944085630034,133.346654004721)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(180.778088979668,130.513656086319),
			D2D1::Point2F(174.611064210855,119.01366453615),
			D2D1::Point2F(174.611064210855,119.01366453615)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(174.611064210855,119.01366453615),
			D2D1::Point2F(175.778068897932,112.846669067465),
			D2D1::Point2F(176.611072243549,111.846669802232)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(177.444075589167,110.846670537),
			D2D1::Point2F(189.778125126791,103.680675802348),
			D2D1::Point2F(190.444127801678,96.6806809457234)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(191.111130480582,89.6806860890991),
			D2D1::Point2F(181.111090317111,83.1806908650909),
			D2D1::Point2F(179.111082284417,81.6806919672428)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(177.111074251723,80.1806930693948),
			D2D1::Point2F(172.944057515605,76.8466955191112),
			D2D1::Point2F(175.611068227202,70.6807000496904)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(178.2780789388,64.5137045810045),
			D2D1::Point2F(183.944101695422,52.013713765604),
			D2D1::Point2F(176.444071572819,47.1807173167376)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(168.944041450217,42.3470208683855),
			D2D1::Point2F(162.111014006517,43.846719766454),
			D2D1::Point2F(157.110993924782,40.8470219705374)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(152.110973843047,37.8470241748413),
			D2D1::Point2F(154.110981875741,28.1804312775493),
			D2D1::Point2F(152.61097585122,22.8470351963607)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(151.1109698267,17.5137391150987),
			D2D1::Point2F(149.610963802179,17.0137394824827),
			D2D1::Point2F(145.943949074234,14.5137413194026)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(142.277934350306,12.0137431563225),
			D2D1::Point2F(131.943892845376,16.1804400947648),
			D2D1::Point2F(127.777876113274,17.1804393599969)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(123.610859377156,18.1803386253024),
			D2D1::Point2F(121.277850007018,15.5137405846346),
			D2D1::Point2F(121.277850007018,15.5137405846346)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(107.443794444873,-2.65294606705621),
			D2D1::Point2F(110.2778058272,2.34715025903049),
			D2D1::Point2F(106.110789091082,0.347051728639904)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(101.943772354964,-1.65294680182417),
			D2D1::Point2F(93.5027384529787,5.64394783664748),
			D2D1::Point2F(93.5027384529787,5.64394783664748)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(91.9237321111667,7.53884644433565),
			D2D1::Point2F(90.3437257653383,9.43364505209733),
			D2D1::Point2F(88.5837186965675,10.6619441495818)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(86.8237116277967,11.8902432470663),
			D2D1::Point2F(84.8837038360835,12.4519428343472),
			D2D1::Point2F(82.7476952571661,12.2606429749083)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(80.6106866742325,12.0692431155429),
			D2D1::Point2F(78.2776773040948,11.1248438094578),
			D2D1::Point2F(76.082668488213,9.90264470749116)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(73.8886596763476,8.68044560552456),
			D2D1::Point2F(71.8326514187381,7.18044670767652),
			D2D1::Point2F(69.1666407111568,6.23594740166484)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(66.4996299995592,5.29154809557971),
			D2D1::Point2F(63.2216168339736,4.90264838133097),
			D2D1::Point2F(60.8886074638359,5.37484803437354)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(58.5555980936983,5.84704768741611),
			D2D1::Point2F(57.1665925149922,7.18044670767652),
			D2D1::Point2F(55.7775869362862,8.51374572801038)
			));


		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	return hr;
}

// This function creates the path geometry that represents the up continent in the planet. It demonstrates how to substitute clipping by intersecting.
HRESULT Renderer::CreatePlanetUpPath()
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink* pSink = NULL;
	// Since clip is required, we must draw to a temporary path, and then intersect it with the ellipse, and save the result in m_pPlanetUpPath.
	ID2D1PathGeometry* tempPath;
	hr = this->m_pDirect2dFactory->CreatePathGeometry(&this->m_pPlanetUpPath);
	if (SUCCEEDED(hr))
	{
		hr = this->m_pDirect2dFactory->CreatePathGeometry(&tempPath);
	}
	if (SUCCEEDED(hr))
	{
		hr = tempPath->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F(0.0288963486137668,71.2923486227374),
			D2D1_FIGURE_BEGIN_FILLED
			);

		// The following AddBezier code are generated by the PowerShell script.
		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(-0.581304006712504,71.029349563583),
			D2D1::Point2F(8.52890129825862,88.2922878076214),
			D2D1::Point2F(22.0289091594593,91.7922752868622)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(35.5289170206599,95.292262766103),
			D2D1::Point2F(42.528921096838,64.2923736642557),
			D2D1::Point2F(42.528921096838,64.2923736642557)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(49.0289248818605,71.2923486227374),
			D2D1::Point2F(59.5289309961277,76.2923307359385),
			D2D1::Point2F(61.52893216075,94.7922645547829)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(63.5289333253723,113.292198373627),
			D2D1::Point2F(96.5289525416406,85.7922967510208),
			D2D1::Point2F(99.5289542885741,86.2922949623409)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(102.528956035508,86.792293173661),
			D2D1::Point2F(127.528970593286,111.792203739667),
			D2D1::Point2F(146.028981366043,106.792221626466)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(164.528992138799,101.792239513265),
			D2D1::Point2F(178.029,80.2923164264995),
			D2D1::Point2F(178.029,80.2923164264995)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(178.029,80.2923164264995),
			D2D1::Point2F(105.028957491285,-94.2070593242214),
			D2D1::Point2F(0.0288963486137668,71.2923486227374)
			));



		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	ID2D1GeometrySink* pSink2;
	if (SUCCEEDED(hr))
	{
		hr = this->m_pPlanetUpPath->Open(&pSink2);
	}
	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		// Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(87.0f, 121.0f),
			100.0f,
			100.0f
			);
		hr = this->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &clipEllipse);
	}
	if (SUCCEEDED(hr))
	{
		// There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
		hr = tempPath->CombineWithGeometry(clipEllipse, D2D1_COMBINE_MODE_INTERSECT, NULL, 0, pSink2);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSink2->Close();
		SafeRelease(&pSink2);
		SafeRelease(&tempPath);
		SafeRelease(&clipEllipse);
	}
	return hr;
}

// This function creates the path geometry that represents the down continent in the planet. It demonstrates how to substitute clipping by intersecting.
HRESULT Renderer::CreatePlanetDownPath()
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink* pSink = NULL;
	//Since clip is required, we must draw to a temporary path, and then intersect it with the ellipse, and save the result in m_pPlanetDownPath.
	ID2D1PathGeometry* tempPath;
	hr = this->m_pDirect2dFactory->CreatePathGeometry(&this->m_pPlanetDownPath);
	if (SUCCEEDED(hr))
	{
		hr = this->m_pDirect2dFactory->CreatePathGeometry(&tempPath);
	}
	if (SUCCEEDED(hr))
	{
		hr = tempPath->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F(0.0288963486137668,71.2923486227374),
			D2D1_FIGURE_BEGIN_FILLED
			);

		// The following AddBezier code are generated by the PowerShell script.
		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(0.715499175522696,-0.486801532710843),
			D2D1::Point2F(26.1927417195169,14.0124923433061),
			D2D1::Point2F(48.6926998092458,17.5124667261123)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(71.1926578989747,21.0124411089184),
			D2D1::Point2F(94.1926150573642,39.512305703751),
			D2D1::Point2F(93.192616920043,53.512703231316)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(92.1926187827217,67.5126007625406),
			D2D1::Point2F(123.192561039681,82.5124909745669),
			D2D1::Point2F(123.192561039681,82.5124909745669)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(123.192561039681,82.5124909745669),
			D2D1::Point2F(134.192540550216,62.0126410181309),
			D2D1::Point2F(121.6925638337,45.0122654481606)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(109.192587117184,28.0123898745307),
			D2D1::Point2F(99.6926048126313,2.01258017368507),
			D2D1::Point2F(122.692561971021,21.5124374493193)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(145.69251912941,41.0122947249536),
			D2D1::Point2F(151.692507953338,62.5126373585318),
			D2D1::Point2F(149.192512610035,67.0126044221397)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(146.692517266732,71.5125714857476),
			D2D1::Point2F(159.192493983248,73.5125568473511),
			D2D1::Point2F(147.192516335392,84.5124763361704)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(135.192538687537,95.5123958249898),
			D2D1::Point2F(108.692588048523,102.012348250201),
			D2D1::Point2F(108.692588048523,102.012348250201)
			));

		pSink->AddBezier(
			D2D1::BezierSegment(
			D2D1::Point2F(108.692588048523,102.012348250201),
			D2D1::Point2F(-5.30719960610358,132.012128674254),
			D2D1::Point2F(0.19280014914486,0.0125948120815593)
			));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	ID2D1GeometrySink* pSink2;
	if (SUCCEEDED(hr))
	{
		hr = this->m_pPlanetDownPath->Open(&pSink2);
	}
	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(95.0f, 1.0f),
			100.0f,
			100.0f
			);
		hr = this->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &clipEllipse);
	}
	if (SUCCEEDED(hr))
	{
		//There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
		hr = tempPath->CombineWithGeometry(clipEllipse, D2D1_COMBINE_MODE_INTERSECT, NULL, 0, pSink2);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSink2->Close();
		SafeRelease(&pSink2);
		SafeRelease(&tempPath);
		SafeRelease(&clipEllipse);
	}
	return hr;
}
