/****************************** Module Header ******************************\
* Module Name:  MainWindow.xaml.cs
* Project:      CSWin7Direct2D
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to work with Direct2D using C#.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/11/2009 14:54 Yilun Luo Created
\***************************************************************************/

using System.Windows;
using System.Windows.Interop;
using Microsoft.WindowsAPICodePack.DirectX.Direct2D1;
using Microsoft.WindowsAPICodePack.DirectX;
using System;

namespace CSWin7Direct2D
{
	/// <summary>
	/// Interaction logic for Window1.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		private D2DFactory _d2DFactory;
		private RenderTarget _renderTarget;
		private SolidColorBrush _planetBackgroundBrush;
		private SolidColorBrush _starBrush;
		private SolidColorBrush _continentBrush;
		private SolidColorBrush _smallStarBrush;
		private RadialGradientBrush _starOutlineBrush;
		private PathGeometry _starOutline;
		private PathGeometry _planetUpPath;
		private PathGeometry _planetDownPath;
		private Random _random = new Random();
		private bool _animateToRight = true;
		private int _animateTranslateX;
		private bool _animate;
		private int _clickedPointX;
		private int _clickedPointY;

		public MainWindow()
		{
			InitializeComponent();
		}

		private void Host_Loaded(object sender, RoutedEventArgs e)
		{
			this.CreateDeviceIndependentResource();
			ComponentDispatcher.ThreadPreprocessMessage += new ThreadMessageEventHandler(ComponentDispatcher_ThreadPreprocessMessage);
			host.Render = this.Render;
		}

		/// <summary>
		/// Device independent resources are not specific to a particular device. For example, factory and font.
		/// </summary>
		private void CreateDeviceIndependentResource()
		{
			//Create Direct 2D factory.
			this._d2DFactory = D2DFactory.CreateFactory();
		}

		/// <summary>
		/// Device dependent resources are specific to a particular device. For example,brush may render differently on different devices (hwnd, DXGI (Direct3D surface), etc...).
		/// </summary>
		private void CreateDeviceResource()
		{
			if (this._renderTarget == null)
			{
				// Create an HwndRenderTarget to draw to the hwnd.
				this._renderTarget = this._d2DFactory.CreateHwndRenderTarget(new RenderTargetProperties(), new HwndRenderTargetProperties(this.host.Handle, new SizeU((uint)this.host.ActualWidth, (uint)this.host.ActualHeight), PresentOptions.RetainContents));
				// Create a SolidColorBrush (planet background).
				this._planetBackgroundBrush = this._renderTarget.CreateSolidColorBrush(new ColorF(0f, 0.49f, 0.84f, 1));
				// Create a SolidColorBrush (star).
				this._starBrush = this._renderTarget.CreateSolidColorBrush(new ColorF(Colors.Red));
				// Create a SolidColorBrush (continent).
				this._continentBrush = this._renderTarget.CreateSolidColorBrush(new ColorF(0.04f, 1, 0f, 1));
				// Create a SolidColorBrush (small stars).
				this._smallStarBrush = this._renderTarget.CreateSolidColorBrush(new ColorF(Colors.White));
				// Create a RadialGradientBrush (star outline).
				this.CreateStarOutlineBrush();
				// Prepare the geometry for the star's outline.
				this.CreateStarOutline();
				// Prepare the geometry for the planet's up path.
				this.CreatePlanetUpPath();
				// Prepare the geometry for the planet's down path.
				this.CreatePlanetDownPath();
			}
		}

		/// <summary>
		///  Create a RadialGradientBrush (star outline).
		/// </summary>
		private void CreateStarOutlineBrush()
		{
			GradientStopCollection gradientStopCollection = null;
			GradientStop[] gradientStops = new GradientStop[2];
			gradientStops[0].Color = new ColorF(1f, 0.48f, 0f, 1f);
			gradientStops[0].Position = 0.72093f;
			gradientStops[1].Color = new ColorF(0.92f, 1f, 0f, 0.5f);
			gradientStops[1].Position = 1.0f;
			gradientStopCollection = this._renderTarget.CreateGradientStopCollection(gradientStops, Gamma.Gamma_22, ExtendMode.Clamp);
			this._starOutlineBrush = this._renderTarget.CreateRadialGradientBrush(
				new RadialGradientBrushProperties(new Point2F(95f, 95f), new Point2F(0f, 0f), 95f, 95f),
				gradientStopCollection);
		}

		/// <summary>
		/// The main rendering method.
		/// </summary>
		private void Render()
		{
			this.CreateDeviceResource();
			this._renderTarget.BeginDraw();

			// Do some clearing.
			this._renderTarget.Transform = Matrix3x2F.Identity;
			this._renderTarget.Clear(new ColorF(Colors.Black));
			SizeF size = this._renderTarget.Size;
			RectF rectBackground = new RectF(0f, 0f, size.Width, size.Height);

			// Get the size of the RenderTarget.
			float rtWidth = this._renderTarget.Size.Width;
			float rtHeight = this._renderTarget.Size.Height;

			// Draw some small stars
			for (int i = 0; i < 300; i++)
			{
				float x = (float)(this._random.NextDouble()) * rtWidth;
				float y = (float)(this._random.NextDouble()) * rtHeight;
				Ellipse smallStar = new Ellipse(new Point2F(x, y), 1f, 1f);
				this._renderTarget.FillEllipse(smallStar, this._smallStarBrush);
			}

			Ellipse planet = new Ellipse(new Point2F(100f, 100f), 100f, 100f);
			// When animating from right to left, draw the planet afte the star so it has a smaller z-index, and will be covered by the star.
			if (!this._animateToRight)
			{
				this.DrawPlanet(planet);
			}

			// Draw the star.
			Ellipse star = new Ellipse(new Point2F(95f, 95f), 75f, 75f);
			// Scale the star, and translate it to the center of the screen. Note if translation is performed before scaling, you'll get different result.
			Matrix3x2F scaleMatrix = Matrix3x2F.Scale(2f, 2f, new Point2F(95f, 95f));
			Matrix3x2F translationMatrix = Matrix3x2F.Translation(rtWidth / 2 - 95, rtHeight / 2 - 95);
			// Since the managed counter part of Matrix3x2F does not expose the multiply operaion, let's convert them to WPF matrixes to do the multiplication.
			System.Windows.Media.Matrix wpfScaleMatrix = new System.Windows.Media.Matrix(scaleMatrix.M11, scaleMatrix.M12, scaleMatrix.M21, scaleMatrix.M22, scaleMatrix.M31, scaleMatrix.M32);
			System.Windows.Media.Matrix wpfTranslateMatrix = new System.Windows.Media.Matrix(translationMatrix.M11, translationMatrix.M12, translationMatrix.M21, translationMatrix.M22, translationMatrix.M31, translationMatrix.M32);
			System.Windows.Media.Matrix wpfResultMatrix = wpfScaleMatrix * wpfTranslateMatrix;
			this._renderTarget.Transform = new Matrix3x2F((float)wpfResultMatrix.M11, (float)wpfResultMatrix.M12, (float)wpfResultMatrix.M21, (float)wpfResultMatrix.M22, (float)wpfResultMatrix.OffsetX, (float)wpfResultMatrix.OffsetY);
			this._renderTarget.FillGeometry(this._starOutline, this._starOutlineBrush);
			// The transform matrix will be apllied to all rendered elements, until it is reset. So we don't need to set the matrix for the ellipse again.
			this._renderTarget.FillEllipse(star, this._starBrush);

			// By default, or when animating from left to right, draw the planet afte the star so it has a larger z-index.
			if (this._animateToRight)
			{
				this.DrawPlanet(planet);
			}

			if (this._animate)
			{
				// Perform a hit test. If the user clicked the planet, let's animate it to make it move around the star.
				EllipseGeometry hitTestEllipse = this._d2DFactory.CreateEllipseGeometry(planet);
				Point2F point = new Point2F(this._clickedPointX, this._clickedPointY);
				Matrix3x2F matrix = Matrix3x2F.Translation(10f, rtHeight / 2 - 100);
				bool hit = hitTestEllipse.FillContainsPoint(point, 0f, matrix);
				if (!hit)
				{
					this._animate = false;
				}
				else
				{
					// When moving from left to right, translate transform becomes larger and lager.
					if (this._animateToRight)
					{
						this._animateTranslateX++;
						if (this._animateTranslateX > rtWidth - 220)
						{
							this._animateToRight = false;
						}
					}
					else
					{
						// When moving from right to left, translate transform becomes smaller and smaller.
						this._animateTranslateX--;
						if (this._animateTranslateX <= 0)
						{
							this._animateToRight = true;
							this._animateTranslateX = 0;
							this._animate = false;
						}
					}
				}
			}

			// Finish drawing.
			this._renderTarget.EndDraw();
		}

		/// <summary>
		/// Draw the planet.
		/// </summary>
		/// <param name="planet"></param>
		private void DrawPlanet(Ellipse planet)
		{
			// Get the size of the RenderTarget.
			float rtWidth = this._renderTarget.Size.Width;
			float rtHeight = this._renderTarget.Size.Height;

			this._renderTarget.Transform = Matrix3x2F.Translation(10 + this._animateTranslateX, rtHeight / 2 - 100);
			this._renderTarget.FillEllipse(planet, this._planetBackgroundBrush);
			this._renderTarget.Transform = Matrix3x2F.Translation(23 + this._animateTranslateX, rtHeight / 2 - 121);
			this._renderTarget.FillGeometry(this._planetUpPath, this._continentBrush);
			this._renderTarget.Transform = Matrix3x2F.Translation(15 + this._animateTranslateX, rtHeight / 2 + 1);
			this._renderTarget.FillGeometry(this._planetDownPath, this._continentBrush);
		}

		/// <summary>
		/// Because RenderHost and DirectHost uses Win32 interop, we cannot handle input directly using WPF's event model. We have to handle the message loop in ThreadPreprocessMessage.
		/// </summary>
		/// <param name="msg"></param>
		/// <param name="handled"></param>
		private void ComponentDispatcher_ThreadPreprocessMessage(ref MSG msg, ref bool handled)
		{
			switch (msg.message)
			{
				// WM_LBUTTONDOWN
				case 0x0201:
					if (!this._animate)
					{
						this._animate = true;
						this._clickedPointX = msg.lParam.ToInt32() & 0xffff;
						this._clickedPointY = (msg.lParam.ToInt32() >> 16) & 0xffff;
					}
					break;
			}
		}

		/// <summary>
		/// This method creates the path geometry that represents the star's outline.
		/// The data of the path is created in Expression Blend first,
		/// and then use a PowerShell script to translate the XAML to C# code.
		/// </summary>
		private void CreateStarOutline()
		{
			this._starOutline = this._d2DFactory.CreatePathGeometry();
			GeometrySink sink = this._starOutline.Open();
			sink.BeginFigure(new Point2F(55.7775869362862f, 8.51374572801038f), FigureBegin.Filled);

			// The following AddBezier code are generated by the PowerShell script.
			sink.AddBezier(new BezierSegment(
							new Point2F(56.9435916193468f, 9.34704511572823f),
							new Point2F(53.1105762246886f, 6.5137471975463f),
							new Point2F(52.4435735457851f, 16.0137402172507f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(51.7775708708979f, 25.513733236955f),
							new Point2F(44.4435414150087f, 26.513732502187f),
							new Point2F(42.9435353904881f, 26.8470322572889f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(41.4435293659675f, 27.1803320123907f),
							new Point2F(28.4438771550624f, 22.0137358086429f),
							new Point2F(22.2772523878566f, 30.5137295631152f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(16.1105276202493f, 39.0137233175875f),
							new Point2F(21.7772503796831f, 48.0137167046759f),
							new Point2F(21.1105477019846f, 54.5137119286841f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(20.4439450246876f, 61.0137071526923f),
							new Point2F(6.61048946495231f, 64.3467047037107f),
							new Point2F(3.77717808543619f, 67.0137027440846f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(0.943866705920101f, 69.6807007844584f),
							new Point2F(-0.889540657650585f, 73.5136979680928f),
							new Point2F(0.443864697746569f, 77.6806949063147f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(1.77717005274209f, 81.8466918452714f),
							new Point2F(8.94379883629487f, 88.8466867018956f),
							new Point2F(8.9438988366965f, 88.8466867018956f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(11.2772082080391f, 91.3466848649757f),
							new Point2F(10.7772061998656f, 100.680678006652f),
							new Point2F(8.44389682852298f, 102.1806769045f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(6.11048745677877f, 103.680675802348f),
							new Point2F(-2.38954668217117f, 114.180668087284f),
							new Point2F(1.61046938321705f, 120.680663311292f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(5.61048544860525f, 127.180658535301f),
							new Point2F(11.6105095466876f, 126.846658780713f),
							new Point2F(15.9439269511259f, 128.513657555855f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(20.2772443551626f, 130.180656330997f),
							new Point2F(21.7772503796831f, 136.346651800417f),
							new Point2F(21.4439490410347f, 137.846650698265f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(21.1105477019846f, 139.346649596113f),
							new Point2F(18.4439369919935f, 151.013641023576f),
							new Point2F(19.9439430165141f, 154.513638451888f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(21.4439490410347f, 158.0136358802f),
							new Point2F(23.7772584123772f, 159.84663453337f),
							new Point2F(23.7772584123772f, 159.84663453337f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(24.9439630982493f, 162.180632818422f),
							new Point2F(30.110583849108f, 162.513632573744f),
							new Point2F(34.2773005840213f, 161.346633431218f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(38.4435173169264f, 160.180634287958f),
							new Point2F(44.1105400775651f, 161.846633063834f),
							new Point2F(44.1105400775651f, 161.846633063834f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(53.9435795703057f, 169.013627797752f),
							new Point2F(50.2775648463774f, 173.346624614003f),
							new Point2F(54.4435815784792f, 180.013619715305f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(58.6105983145974f, 186.680614816607f),
							new Point2F(63.6106183963326f, 185.346615796787f),
							new Point2F(66.7776311161037f, 184.846616164171f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(69.9436438318585f, 184.346616531555f),
							new Point2F(77.4436739544614f, 178.846620572779f),
							new Point2F(79.6106826578854f, 178.180621062134f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(81.7776913613095f, 177.513621552225f),
							new Point2F(86.6107107723148f, 178.846620572779f),
							new Point2F(86.6107107723148f, 178.846620572779f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(96.7777516065153f, 189.930612428611f),
							new Point2F(102.943776371311f, 190.013612367625f),
							new Point2F(105.943788420352f, 189.680612612303f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(108.943800469394f, 189.346612857715f),
							new Point2F(115.277825908936f, 185.013616041465f),
							new Point2F(116.610831262726f, 180.513619347921f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(117.943836616517f, 176.013622654377f),
							new Point2F(122.110853352635f, 173.346624614003f),
							new Point2F(122.943856698252f, 173.180624735974f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(123.777860047886f, 173.01362485868f),
							new Point2F(124.777864064233f, 170.846626450923f),
							new Point2F(131.277890170489f, 173.180624735974f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(137.777916276744f, 175.513623021761f),
							new Point2F(144.943945057887f, 175.680622899054f),
							new Point2F(149.777964472909f, 173.346624614003f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(154.610983883914f, 171.013626328216f),
							new Point2F(153.277978530124f, 163.180632083654f),
							new Point2F(153.277978530124f, 161.18063355319f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(153.277978530124f, 159.180635022726f),
							new Point2F(153.110977859394f, 155.51363771712f),
							new Point2F(156.277990579165f, 151.180640900869f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(159.44400329492f, 146.846644085354f),
							new Point2F(171.611052161814f, 145.013645432183f),
							new Point2F(171.611052161814f, 145.013645432183f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(180.444087638208f, 142.680647146397f),
							new Point2F(179.111082284417f, 136.180651922389f),
							new Point2F(179.944085630034f, 133.346654004721f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(180.778088979668f, 130.513656086319f),
							new Point2F(174.611064210855f, 119.01366453615f),
							new Point2F(174.611064210855f, 119.01366453615f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(174.611064210855f, 119.01366453615f),
							new Point2F(175.778068897932f, 112.846669067465f),
							new Point2F(176.611072243549f, 111.846669802232f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(177.444075589167f, 110.846670537f),
							new Point2F(189.778125126791f, 103.680675802348f),
							new Point2F(190.444127801678f, 96.6806809457234f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(191.111130480582f, 89.6806860890991f),
							new Point2F(181.111090317111f, 83.1806908650909f),
							new Point2F(179.111082284417f, 81.6806919672428f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(177.111074251723f, 80.1806930693948f),
							new Point2F(172.944057515605f, 76.8466955191112f),
							new Point2F(175.611068227202f, 70.6807000496904f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(178.2780789388f, 64.5137045810045f),
							new Point2F(183.944101695422f, 52.013713765604f),
							new Point2F(176.444071572819f, 47.1807173167376f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(168.944041450217f, 42.3470208683855f),
							new Point2F(162.111014006517f, 43.846719766454f),
							new Point2F(157.110993924782f, 40.8470219705374f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(152.110973843047f, 37.8470241748413f),
							new Point2F(154.110981875741f, 28.1804312775493f),
							new Point2F(152.61097585122f, 22.8470351963607f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(151.1109698267f, 17.5137391150987f),
							new Point2F(149.610963802179f, 17.0137394824827f),
							new Point2F(145.943949074234f, 14.5137413194026f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(142.277934350306f, 12.0137431563225f),
							new Point2F(131.943892845376f, 16.1804400947648f),
							new Point2F(127.777876113274f, 17.1804393599969f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(123.610859377156f, 18.1803386253024f),
							new Point2F(121.277850007018f, 15.5137405846346f),
							new Point2F(121.277850007018f, 15.5137405846346f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(107.443794444873f, -2.65294606705621f),
							new Point2F(110.2778058272f, 2.34715025903049f),
							new Point2F(106.110789091082f, 0.347051728639904f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(101.943772354964f, -1.65294680182417f),
							new Point2F(93.5027384529787f, 5.64394783664748f),
							new Point2F(93.5027384529787f, 5.64394783664748f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(91.9237321111667f, 7.53884644433565f),
							new Point2F(90.3437257653383f, 9.43364505209733f),
							new Point2F(88.5837186965675f, 10.6619441495818f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(86.8237116277967f, 11.8902432470663f),
							new Point2F(84.8837038360835f, 12.4519428343472f),
							new Point2F(82.7476952571661f, 12.2606429749083f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(80.6106866742325f, 12.0692431155429f),
							new Point2F(78.2776773040948f, 11.1248438094578f),
							new Point2F(76.082668488213f, 9.90264470749116f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(73.8886596763476f, 8.68044560552456f),
							new Point2F(71.8326514187381f, 7.18044670767652f),
							new Point2F(69.1666407111568f, 6.23594740166484f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(66.4996299995592f, 5.29154809557971f),
							new Point2F(63.2216168339736f, 4.90264838133097f),
							new Point2F(60.8886074638359f, 5.37484803437354f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(58.5555980936983f, 5.84704768741611f),
							new Point2F(57.1665925149922f, 7.18044670767652f),
							new Point2F(55.7775869362862f, 8.51374572801038f)));

			sink.EndFigure(FigureEnd.Closed);
			sink.Close();
		}

		/// <summary>
		/// This method creates the path geometry that represents the up continent in the planet.
		/// It demonstrates how to substitute clipping by intersecting.
		/// </summary>
		private void CreatePlanetUpPath()
		{
			// Since clip is required, we must draw to a temporary path, and then intersect it with the ellipse, and save the result in m_pPlanetUpPath.
			PathGeometry tempPath = this._d2DFactory.CreatePathGeometry();
			this._planetUpPath = this._d2DFactory.CreatePathGeometry();
			GeometrySink sink = tempPath.Open();
			sink.BeginFigure(new Point2F(0.0288963486137668f, 71.2923486227374f), FigureBegin.Filled);

			// The following AddBezier code are generated by the PowerShell script.
			sink.AddBezier(new BezierSegment(
				new Point2F(-0.581304006712504f, 71.029349563583f),
				new Point2F(8.52890129825862f, 88.2922878076214f),
				new Point2F(22.0289091594593f, 91.7922752868622f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(35.5289170206599f, 95.292262766103f),
							new Point2F(42.528921096838f, 64.2923736642557f),
							new Point2F(42.528921096838f, 64.2923736642557f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(49.0289248818605f, 71.2923486227374f),
							new Point2F(59.5289309961277f, 76.2923307359385f),
							new Point2F(61.52893216075f, 94.7922645547829f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(63.5289333253723f, 113.292198373627f),
							new Point2F(96.5289525416406f, 85.7922967510208f),
							new Point2F(99.5289542885741f, 86.2922949623409f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(102.528956035508f, 86.792293173661f),
							new Point2F(127.528970593286f, 111.792203739667f),
							new Point2F(146.028981366043f, 106.792221626466f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(164.528992138799f, 101.792239513265f),
							new Point2F(178.029f, 80.2923164264995f),
							new Point2F(178.029f, 80.2923164264995f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(178.029f, 80.2923164264995f),
							new Point2F(105.028957491285f, -94.2070593242214f),
							new Point2F(0.0288963486137668f, 71.2923486227374f)));

			sink.EndFigure(FigureEnd.Closed);
			sink.Close();

			GeometrySink sink2 = this._planetUpPath.Open();
			// Create a clip ellipse.
			Ellipse clip = new Ellipse(new Point2F(87f, 121f), 100f, 100f);
			EllipseGeometry clipEllipse = this._d2DFactory.CreateEllipseGeometry(clip);
			// There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
			tempPath.CombineWithGeometry(clipEllipse, CombineMode.Intersect, 0f, sink2);
			sink2.Close();
		}

		/// <summary>
		/// This method creates the path geometry that represents the down continent in the planet.
		/// It demonstrates how to substitute clipping by intersecting.
		/// </summary>
		private void CreatePlanetDownPath()
		{
			// Since clip is required, we must draw to a temporary path, and then intersect it with the ellipse, and save the result in m_pPlanetUpPath.
			PathGeometry tempPath = this._d2DFactory.CreatePathGeometry();
			this._planetDownPath = this._d2DFactory.CreatePathGeometry();
			GeometrySink sink = tempPath.Open();
			sink.BeginFigure(new Point2F(0.0288963486137668f, 71.2923486227374f), FigureBegin.Filled);

			// The following AddBezier code are generated by the PowerShell script.
			sink.AddBezier(new BezierSegment(
							new Point2F(0.715499175522696f, -0.486801532710843f),
							new Point2F(26.1927417195169f, 14.0124923433061f),
							new Point2F(48.6926998092458f, 17.5124667261123f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(71.1926578989747f, 21.0124411089184f),
							new Point2F(94.1926150573642f, 39.512305703751f),
							new Point2F(93.192616920043f, 53.512703231316f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(92.1926187827217f, 67.5126007625406f),
							new Point2F(123.192561039681f, 82.5124909745669f),
							new Point2F(123.192561039681f, 82.5124909745669f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(123.192561039681f, 82.5124909745669f),
							new Point2F(134.192540550216f, 62.0126410181309f),
							new Point2F(121.6925638337f, 45.0122654481606f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(109.192587117184f, 28.0123898745307f),
							new Point2F(99.6926048126313f, 2.01258017368507f),
							new Point2F(122.692561971021f, 21.5124374493193f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(145.69251912941f, 41.0122947249536f),
							new Point2F(151.692507953338f, 62.5126373585318f),
							new Point2F(149.192512610035f, 67.0126044221397f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(146.692517266732f, 71.5125714857476f),
							new Point2F(159.192493983248f, 73.5125568473511f),
							new Point2F(147.192516335392f, 84.5124763361704f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(135.192538687537f, 95.5123958249898f),
							new Point2F(108.692588048523f, 102.012348250201f),
							new Point2F(108.692588048523f, 102.012348250201f)));

			sink.AddBezier(new BezierSegment(
							new Point2F(108.692588048523f, 102.012348250201f),
							new Point2F(-5.30719960610358f, 132.012128674254f),
							new Point2F(0.19280014914486f, 0.0125948120815593f)));

			sink.EndFigure(FigureEnd.Closed);
			sink.Close();

			GeometrySink sink2 = this._planetDownPath.Open();
			// Create a clip ellipse.
			Ellipse clip = new Ellipse(new Point2F(95f, 1f), 100f, 100f);
			EllipseGeometry clipEllipse = this._d2DFactory.CreateEllipseGeometry(clip);
			// There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
			tempPath.CombineWithGeometry(clipEllipse, CombineMode.Intersect, 0f, sink2);
			sink2.Close();
		}
	}
}
