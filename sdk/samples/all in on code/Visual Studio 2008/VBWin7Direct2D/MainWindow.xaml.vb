'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWin7Direct2D
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to work with Direct2D using VB.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 10/11/2009 14:54 Yilun Luo Created
'***************************************************************************'

Imports Microsoft.WindowsAPICodePack.DirectX.Direct2D1
Imports Microsoft.WindowsAPICodePack.DirectX.Controls
Imports System.Windows.Interop

Class MainWindow
	Private _animate As Boolean
	Private _animateToRight As Boolean = True
	Private _animateTranslateX As Integer
	Private _clickedPointX As Integer
	Private _clickedPointY As Integer
	Private _continentBrush As SolidColorBrush
	Private _d2DFactory As D2DFactory
	Private _planetBackgroundBrush As SolidColorBrush
	Private _planetDownPath As PathGeometry
	Private _planetUpPath As PathGeometry
	Private _random As Random = New Random
	Private _renderTarget As RenderTarget
	Private _smallStarBrush As SolidColorBrush
	Private _starBrush As SolidColorBrush
	Private _starOutline As PathGeometry
	Private _starOutlineBrush As RadialGradientBrush

	''' <summary>
	''' Because RenderHost and DirectHost uses Win32 interop, we cannot handle input directly using WPF's event model. We have to handle the message loop in ThreadPreprocessMessage.
	''' </summary>
	''' <param name="msg"></param>
	''' <param name="handled"></param>
	''' <remarks></remarks>
	Private Sub ComponentDispatcher_ThreadPreprocessMessage(ByRef msg As Interop.MSG, ByRef handled As Boolean)
		' WM_LBUTTONDOWN
		If ((msg.message = &H201) AndAlso Not Me._animate) Then
			Me._animate = True
			Me._clickedPointX = (msg.lParam.ToInt32 And &HFFFF)
			Me._clickedPointY = ((msg.lParam.ToInt32 >> &H10) And &HFFFF)
		End If
	End Sub

	''' <summary>
	''' Device independent resources are not specific to a particular device. For example, factory and font.
	''' </summary>
	''' <remarks></remarks>
	Private Sub CreateDeviceIndependentResource()
		Me._d2DFactory = D2DFactory.CreateFactory
	End Sub

	''' <summary>
	''' Device dependent resources are specific to a particular device. For example,brush may render differently on different devices (hwnd, DXGI (Direct3D surface), etc...).
	''' </summary>
	''' <remarks></remarks>
	Private Sub CreateDeviceResource()
		If (Me._renderTarget Is Nothing) Then
			' Create an HwndRenderTarget to draw to the hwnd.
			Me._renderTarget = Me._d2DFactory.CreateHwndRenderTarget(New RenderTargetProperties, New HwndRenderTargetProperties(Me.host.Handle, New SizeU(CUInt(Me.host.ActualWidth), CUInt(Me.host.ActualHeight)), PresentOptions.RetainContents))
			' Create a SolidColorBrush (planet background).
			Me._planetBackgroundBrush = Me._renderTarget.CreateSolidColorBrush(New ColorF(0.0!, 0.49!, 0.84!, 1.0!))
			' Create a SolidColorBrush (star).
			Me._starBrush = Me._renderTarget.CreateSolidColorBrush(New ColorF(Microsoft.WindowsAPICodePack.DirectX.Colors.Red))
			' Create a SolidColorBrush (continent).
			Me._continentBrush = Me._renderTarget.CreateSolidColorBrush(New ColorF(0.04!, 1.0!, 0.0!, 1.0!))
			' Create a SolidColorBrush (small stars).
			Me._smallStarBrush = Me._renderTarget.CreateSolidColorBrush(New ColorF(Microsoft.WindowsAPICodePack.DirectX.Colors.White))
			' Create a RadialGradientBrush (star outline).
			Me.CreateStarOutlineBrush()
			' Prepare the geometry for the star's outline.
			Me.CreateStarOutline()
			' Prepare the geometry for the planet's up path.
			Me.CreatePlanetUpPath()
			' Prepare the geometry for the planet's down path.
			Me.CreatePlanetDownPath()
		End If
	End Sub

	''' <summary>
	''' This method creates the path geometry that represents the down continent in the planet.
	''' It demonstrates how to substitute clipping by intersecting.
	''' </summary>
	''' <remarks></remarks>
	Private Sub CreatePlanetDownPath()
		Dim tempPath As PathGeometry = Me._d2DFactory.CreatePathGeometry
		Me._planetDownPath = Me._d2DFactory.CreatePathGeometry
		Dim sink As GeometrySink = tempPath.Open
		sink.BeginFigure(New Point2F(0.02889635!, 71.29235!), FigureBegin.Filled)
		sink.AddBezier(New BezierSegment(New Point2F(0.7154992!, -0.4868015!), New Point2F(26.19274!, 14.01249!), New Point2F(48.6927!, 17.51247!)))
		sink.AddBezier(New BezierSegment(New Point2F(71.19266!, 21.01244!), New Point2F(94.19261!, 39.51231!), New Point2F(93.19262!, 53.5127!)))
		sink.AddBezier(New BezierSegment(New Point2F(92.19262!, 67.5126!), New Point2F(123.1926!, 82.51249!), New Point2F(123.1926!, 82.51249!)))
		sink.AddBezier(New BezierSegment(New Point2F(123.1926!, 82.51249!), New Point2F(134.1925!, 62.01264!), New Point2F(121.6926!, 45.01226!)))
		sink.AddBezier(New BezierSegment(New Point2F(109.1926!, 28.01239!), New Point2F(99.6926!, 2.01258!), New Point2F(122.6926!, 21.51244!)))
		sink.AddBezier(New BezierSegment(New Point2F(145.6925!, 41.01229!), New Point2F(151.6925!, 62.51264!), New Point2F(149.1925!, 67.0126!)))
		sink.AddBezier(New BezierSegment(New Point2F(146.6925!, 71.51257!), New Point2F(159.1925!, 73.51256!), New Point2F(147.1925!, 84.51247!)))
		sink.AddBezier(New BezierSegment(New Point2F(135.1925!, 95.5124!), New Point2F(108.6926!, 102.0124!), New Point2F(108.6926!, 102.0124!)))
		sink.AddBezier(New BezierSegment(New Point2F(108.6926!, 102.0124!), New Point2F(-5.307199!, 132.0121!), New Point2F(0.1928001!, 0.01259481!)))
		sink.EndFigure(FigureEnd.Closed)
		sink.Close()
		Dim sink2 As GeometrySink = Me._planetDownPath.Open
		Dim clip As New Ellipse(New Point2F(95.0!, 1.0!), 100.0!, 100.0!)
		Dim clipEllipse As EllipseGeometry = Me._d2DFactory.CreateEllipseGeometry(clip)
		tempPath.CombineWithGeometry(clipEllipse, CombineMode.Intersect, 0.0!, sink2)
		sink2.Close()
	End Sub

	''' <summary>
	''' This method creates the path geometry that represents the up continent in the planet.
	''' It demonstrates how to substitute clipping by intersecting.
	''' </summary>
	''' <remarks></remarks>
	Private Sub CreatePlanetUpPath()
		' Since clip is required, we must draw to a temporary path, and then intersect it with the ellipse, and save the result in m_pPlanetUpPath.
		Dim tempPath As PathGeometry = Me._d2DFactory.CreatePathGeometry
		Me._planetUpPath = Me._d2DFactory.CreatePathGeometry
		Dim sink As GeometrySink = tempPath.Open
		sink.BeginFigure(New Point2F(0.02889635!, 71.29235!), FigureBegin.Filled)

		sink.AddBezier(New BezierSegment(New Point2F(-0.581304!, 71.02935!), New Point2F(8.528901!, 88.29229!), New Point2F(22.02891!, 91.79227!)))
		sink.AddBezier(New BezierSegment(New Point2F(35.52892!, 95.29226!), New Point2F(42.52892!, 64.29237!), New Point2F(42.52892!, 64.29237!)))
		sink.AddBezier(New BezierSegment(New Point2F(49.02892!, 71.29235!), New Point2F(59.52893!, 76.29233!), New Point2F(61.52893!, 94.79227!)))
		sink.AddBezier(New BezierSegment(New Point2F(63.52893!, 113.2922!), New Point2F(96.52895!, 85.7923!), New Point2F(99.52895!, 86.2923!)))
		sink.AddBezier(New BezierSegment(New Point2F(102.529!, 86.79229!), New Point2F(127.529!, 111.7922!), New Point2F(146.029!, 106.7922!)))
		sink.AddBezier(New BezierSegment(New Point2F(164.529!, 101.7922!), New Point2F(178.029!, 80.29231!), New Point2F(178.029!, 80.29231!)))
		sink.AddBezier(New BezierSegment(New Point2F(178.029!, 80.29231!), New Point2F(105.029!, -94.20706!), New Point2F(0.02889635!, 71.29235!)))
		sink.EndFigure(FigureEnd.Closed)
		sink.Close()

		Dim sink2 As GeometrySink = Me._planetUpPath.Open
		' Create a clip ellipse.
		Dim clip As New Ellipse(New Point2F(87.0!, 121.0!), 100.0!, 100.0!)
		Dim clipEllipse As EllipseGeometry = Me._d2DFactory.CreateEllipseGeometry(clip)
		' There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
		tempPath.CombineWithGeometry(clipEllipse, CombineMode.Intersect, 0.0!, sink2)
		sink2.Close()
	End Sub

	''' <summary>
	''' This method creates the path geometry that represents the star's outline.
	''' The data of the path is created in Expression Blend first,
	''' and then use a PowerShell script to translate the XAML to C# code.
	''' The VB code is translated from C# using Reflector.
	''' </summary>
	''' <remarks></remarks>
	Private Sub CreateStarOutline()
		Me._starOutline = Me._d2DFactory.CreatePathGeometry
		Dim sink As GeometrySink = Me._starOutline.Open
		sink.BeginFigure(New Point2F(55.77759!, 8.513745!), FigureBegin.Filled)
		sink.AddBezier(New BezierSegment(New Point2F(56.94359!, 9.347045!), New Point2F(53.11058!, 6.513747!), New Point2F(52.44357!, 16.01374!)))
		sink.AddBezier(New BezierSegment(New Point2F(51.77757!, 25.51373!), New Point2F(44.44354!, 26.51373!), New Point2F(42.94353!, 26.84703!)))
		sink.AddBezier(New BezierSegment(New Point2F(41.44353!, 27.18033!), New Point2F(28.44388!, 22.01374!), New Point2F(22.27725!, 30.51373!)))
		sink.AddBezier(New BezierSegment(New Point2F(16.11053!, 39.01372!), New Point2F(21.77725!, 48.01372!), New Point2F(21.11055!, 54.51371!)))
		sink.AddBezier(New BezierSegment(New Point2F(20.44394!, 61.01371!), New Point2F(6.610489!, 64.3467!), New Point2F(3.777178!, 67.0137!)))
		sink.AddBezier(New BezierSegment(New Point2F(0.9438667!, 69.6807!), New Point2F(-0.8895407!, 73.51369!), New Point2F(0.4438647!, 77.68069!)))
		sink.AddBezier(New BezierSegment(New Point2F(1.77717!, 81.84669!), New Point2F(8.943799!, 88.84669!), New Point2F(8.943899!, 88.84669!)))
		sink.AddBezier(New BezierSegment(New Point2F(11.27721!, 91.34669!), New Point2F(10.77721!, 100.6807!), New Point2F(8.443897!, 102.1807!)))
		sink.AddBezier(New BezierSegment(New Point2F(6.110487!, 103.6807!), New Point2F(-2.389547!, 114.1807!), New Point2F(1.610469!, 120.6807!)))
		sink.AddBezier(New BezierSegment(New Point2F(5.610486!, 127.1807!), New Point2F(11.61051!, 126.8467!), New Point2F(15.94393!, 128.5137!)))
		sink.AddBezier(New BezierSegment(New Point2F(20.27724!, 130.1806!), New Point2F(21.77725!, 136.3466!), New Point2F(21.44395!, 137.8466!)))
		sink.AddBezier(New BezierSegment(New Point2F(21.11055!, 139.3466!), New Point2F(18.44394!, 151.0136!), New Point2F(19.94394!, 154.5136!)))
		sink.AddBezier(New BezierSegment(New Point2F(21.44395!, 158.0136!), New Point2F(23.77726!, 159.8466!), New Point2F(23.77726!, 159.8466!)))
		sink.AddBezier(New BezierSegment(New Point2F(24.94396!, 162.1806!), New Point2F(30.11058!, 162.5136!), New Point2F(34.2773!, 161.3466!)))
		sink.AddBezier(New BezierSegment(New Point2F(38.44352!, 160.1806!), New Point2F(44.11054!, 161.8466!), New Point2F(44.11054!, 161.8466!)))
		sink.AddBezier(New BezierSegment(New Point2F(53.94358!, 169.0136!), New Point2F(50.27757!, 173.3466!), New Point2F(54.44358!, 180.0136!)))
		sink.AddBezier(New BezierSegment(New Point2F(58.6106!, 186.6806!), New Point2F(63.61062!, 185.3466!), New Point2F(66.77763!, 184.8466!)))
		sink.AddBezier(New BezierSegment(New Point2F(69.94364!, 184.3466!), New Point2F(77.44367!, 178.8466!), New Point2F(79.61068!, 178.1806!)))
		sink.AddBezier(New BezierSegment(New Point2F(81.77769!, 177.5136!), New Point2F(86.61071!, 178.8466!), New Point2F(86.61071!, 178.8466!)))
		sink.AddBezier(New BezierSegment(New Point2F(96.77775!, 189.9306!), New Point2F(102.9438!, 190.0136!), New Point2F(105.9438!, 189.6806!)))
		sink.AddBezier(New BezierSegment(New Point2F(108.9438!, 189.3466!), New Point2F(115.2778!, 185.0136!), New Point2F(116.6108!, 180.5136!)))
		sink.AddBezier(New BezierSegment(New Point2F(117.9438!, 176.0136!), New Point2F(122.1109!, 173.3466!), New Point2F(122.9439!, 173.1806!)))
		sink.AddBezier(New BezierSegment(New Point2F(123.7779!, 173.0136!), New Point2F(124.7779!, 170.8466!), New Point2F(131.2779!, 173.1806!)))
		sink.AddBezier(New BezierSegment(New Point2F(137.7779!, 175.5136!), New Point2F(144.9439!, 175.6806!), New Point2F(149.778!, 173.3466!)))
		sink.AddBezier(New BezierSegment(New Point2F(154.611!, 171.0136!), New Point2F(153.278!, 163.1806!), New Point2F(153.278!, 161.1806!)))
		sink.AddBezier(New BezierSegment(New Point2F(153.278!, 159.1806!), New Point2F(153.111!, 155.5136!), New Point2F(156.278!, 151.1806!)))
		sink.AddBezier(New BezierSegment(New Point2F(159.444!, 146.8466!), New Point2F(171.6111!, 145.0136!), New Point2F(171.6111!, 145.0136!)))
		sink.AddBezier(New BezierSegment(New Point2F(180.4441!, 142.6806!), New Point2F(179.1111!, 136.1806!), New Point2F(179.9441!, 133.3466!)))
		sink.AddBezier(New BezierSegment(New Point2F(180.7781!, 130.5137!), New Point2F(174.6111!, 119.0137!), New Point2F(174.6111!, 119.0137!)))
		sink.AddBezier(New BezierSegment(New Point2F(174.6111!, 119.0137!), New Point2F(175.7781!, 112.8467!), New Point2F(176.6111!, 111.8467!)))
		sink.AddBezier(New BezierSegment(New Point2F(177.4441!, 110.8467!), New Point2F(189.7781!, 103.6807!), New Point2F(190.4441!, 96.68068!)))
		sink.AddBezier(New BezierSegment(New Point2F(191.1111!, 89.68069!), New Point2F(181.1111!, 83.18069!), New Point2F(179.1111!, 81.68069!)))
		sink.AddBezier(New BezierSegment(New Point2F(177.1111!, 80.18069!), New Point2F(172.9441!, 76.84669!), New Point2F(175.6111!, 70.6807!)))
		sink.AddBezier(New BezierSegment(New Point2F(178.2781!, 64.5137!), New Point2F(183.9441!, 52.01371!), New Point2F(176.4441!, 47.18072!)))
		sink.AddBezier(New BezierSegment(New Point2F(168.944!, 42.34702!), New Point2F(162.111!, 43.84672!), New Point2F(157.111!, 40.84702!)))
		sink.AddBezier(New BezierSegment(New Point2F(152.111!, 37.84702!), New Point2F(154.111!, 28.18043!), New Point2F(152.611!, 22.84703!)))
		sink.AddBezier(New BezierSegment(New Point2F(151.111!, 17.51374!), New Point2F(149.611!, 17.01374!), New Point2F(145.944!, 14.51374!)))
		sink.AddBezier(New BezierSegment(New Point2F(142.2779!, 12.01374!), New Point2F(131.9439!, 16.18044!), New Point2F(127.7779!, 17.18044!)))
		sink.AddBezier(New BezierSegment(New Point2F(123.6109!, 18.18034!), New Point2F(121.2778!, 15.51374!), New Point2F(121.2778!, 15.51374!)))
		sink.AddBezier(New BezierSegment(New Point2F(107.4438!, -2.652946!), New Point2F(110.2778!, 2.34715!), New Point2F(106.1108!, 0.3470517!)))
		sink.AddBezier(New BezierSegment(New Point2F(101.9438!, -1.652947!), New Point2F(93.50274!, 5.643948!), New Point2F(93.50274!, 5.643948!)))
		sink.AddBezier(New BezierSegment(New Point2F(91.92373!, 7.538846!), New Point2F(90.34373!, 9.433645!), New Point2F(88.58372!, 10.66194!)))
		sink.AddBezier(New BezierSegment(New Point2F(86.82372!, 11.89024!), New Point2F(84.88371!, 12.45194!), New Point2F(82.7477!, 12.26064!)))
		sink.AddBezier(New BezierSegment(New Point2F(80.61069!, 12.06924!), New Point2F(78.27768!, 11.12484!), New Point2F(76.08267!, 9.902645!)))
		sink.AddBezier(New BezierSegment(New Point2F(73.88866!, 8.680446!), New Point2F(71.83265!, 7.180447!), New Point2F(69.16664!, 6.235948!)))
		sink.AddBezier(New BezierSegment(New Point2F(66.49963!, 5.291548!), New Point2F(63.22162!, 4.902648!), New Point2F(60.88861!, 5.374848!)))
		sink.AddBezier(New BezierSegment(New Point2F(58.5556!, 5.847048!), New Point2F(57.16659!, 7.180447!), New Point2F(55.77759!, 8.513745!)))
		sink.EndFigure(FigureEnd.Closed)
		sink.Close()
	End Sub

	''' <summary>
	''' Create a RadialGradientBrush (star outline).
	''' </summary>
	''' <remarks></remarks>
	Private Sub CreateStarOutlineBrush()
		Dim gradientStopCollection As GradientStopCollection = Nothing
		Dim gradientStops As GradientStop() = New GradientStop(2 - 1) {}
		gradientStops(0).Color = New ColorF(1.0!, 0.48!, 0.0!, 1.0!)
		gradientStops(0).Position = 0.72093!
		gradientStops(1).Color = New ColorF(0.92!, 1.0!, 0.0!, 0.5!)
		gradientStops(1).Position = 1.0!
		gradientStopCollection = Me._renderTarget.CreateGradientStopCollection(gradientStops, Gamma.Gamma_22, ExtendMode.Clamp)
		Me._starOutlineBrush = Me._renderTarget.CreateRadialGradientBrush(New RadialGradientBrushProperties(New Point2F(95.0!, 95.0!), New Point2F(0.0!, 0.0!), 95.0!, 95.0!), gradientStopCollection)
	End Sub

	''' <summary>
	''' Draw the planet.
	''' </summary>
	''' <param name="planet"></param>
	''' <remarks></remarks>
	Private Sub DrawPlanet(ByVal planet As Ellipse)
		' Get the size of the RenderTarget.
		Dim rtWidth As Single = Me._renderTarget.Size.Width
		Dim rtHeight As Single = Me._renderTarget.Size.Height

		Me._renderTarget.Transform = Matrix3x2F.Translation(CSng((10 + Me._animateTranslateX)), ((rtHeight / 2.0!) - 100.0!))
		Me._renderTarget.FillEllipse(planet, Me._planetBackgroundBrush)
		Me._renderTarget.Transform = Matrix3x2F.Translation(CSng((&H17 + Me._animateTranslateX)), ((rtHeight / 2.0!) - 121.0!))
		Me._renderTarget.FillGeometry(Me._planetUpPath, Me._continentBrush)
		Me._renderTarget.Transform = Matrix3x2F.Translation(CSng((15 + Me._animateTranslateX)), ((rtHeight / 2.0!) + 1.0!))
		Me._renderTarget.FillGeometry(Me._planetDownPath, Me._continentBrush)
	End Sub

	Private Sub Host_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Me.CreateDeviceIndependentResource()
		AddHandler ComponentDispatcher.ThreadPreprocessMessage, New ThreadMessageEventHandler(AddressOf Me.ComponentDispatcher_ThreadPreprocessMessage)
		Me.host.Render = New RenderHandler(AddressOf Me.Render)
	End Sub

	''' <summary>
	''' The main rendering method.
	''' </summary>
	''' <remarks></remarks>
	Private Sub Render()
		Me.CreateDeviceResource()
		Me._renderTarget.BeginDraw()

		' Do some clearing.
		Me._renderTarget.Transform = Matrix3x2F.Identity
		Me._renderTarget.Clear(New ColorF(Microsoft.WindowsAPICodePack.DirectX.Colors.Black))
		Dim size As SizeF = Me._renderTarget.Size
		Dim rectBackground As New RectF(0.0!, 0.0!, size.Width, size.Height)

		' Get the size of the RenderTarget.
		Dim rtWidth As Single = Me._renderTarget.Size.Width
		Dim rtHeight As Single = Me._renderTarget.Size.Height

		' Draw some small stars
		Dim i As Integer
		For i = 0 To 300 - 1
			Dim x As Single = (CSng(Me._random.NextDouble) * rtWidth)
			Dim y As Single = (CSng(Me._random.NextDouble) * rtHeight)
			Dim smallStar As New Ellipse(New Point2F(x, y), 1.0!, 1.0!)
			Me._renderTarget.FillEllipse(smallStar, Me._smallStarBrush)
		Next i

		Dim planet As New Ellipse(New Point2F(100.0!, 100.0!), 100.0!, 100.0!)
		' When animating from right to left, draw the planet afte the star so it has a smaller z-index, and will be covered by the star.
		If Not Me._animateToRight Then
			Me.DrawPlanet(planet)
		End If

		' Draw the star.
		Dim star As New Ellipse(New Point2F(95.0!, 95.0!), 75.0!, 75.0!)
		' Scale the star, and translate it to the center of the screen. Note if translation is performed before scaling, you'll get different result.
		Dim scaleMatrix As Matrix3x2F = Matrix3x2F.Scale(2.0!, 2.0!, New Point2F(95.0!, 95.0!))
		Dim translationMatrix As Matrix3x2F = Matrix3x2F.Translation(((rtWidth / 2.0!) - 95.0!), ((rtHeight / 2.0!) - 95.0!))
		' Since the managed counter part of Matrix3x2F does not expose the multiply operaion, let's convert them to WPF matrixes to do the multiplication.
		Dim wpfScaleMatrix As New Matrix(CDbl(scaleMatrix.M11), CDbl(scaleMatrix.M12), CDbl(scaleMatrix.M21), CDbl(scaleMatrix.M22), CDbl(scaleMatrix.M31), CDbl(scaleMatrix.M32))
		Dim wpfTranslateMatrix As New Matrix(CDbl(translationMatrix.M11), CDbl(translationMatrix.M12), CDbl(translationMatrix.M21), CDbl(translationMatrix.M22), CDbl(translationMatrix.M31), CDbl(translationMatrix.M32))
		Dim wpfResultMatrix As Matrix = (wpfScaleMatrix * wpfTranslateMatrix)
		Me._renderTarget.Transform = New Matrix3x2F(CSng(wpfResultMatrix.M11), CSng(wpfResultMatrix.M12), CSng(wpfResultMatrix.M21), CSng(wpfResultMatrix.M22), CSng(wpfResultMatrix.OffsetX), CSng(wpfResultMatrix.OffsetY))
		Me._renderTarget.FillGeometry(Me._starOutline, Me._starOutlineBrush)
		' The transform matrix will be apllied to all rendered elements, until it is reset. So we don't need to set the matrix for the ellipse again.
		Me._renderTarget.FillEllipse(star, Me._starBrush)

		' By default, or when animating from left to right, draw the planet afte the star so it has a larger z-index.
		If Me._animateToRight Then
			Me.DrawPlanet(planet)
		End If
		If Me._animate Then
			' Perform a hit test. If the user clicked the planet, let's animate it to make it move around the star.
			Dim hitTestEllipse As EllipseGeometry = Me._d2DFactory.CreateEllipseGeometry(planet)
			Dim point As New Point2F(CSng(Me._clickedPointX), CSng(Me._clickedPointY))
			Dim matrix As Matrix3x2F = Matrix3x2F.Translation(10.0!, ((rtHeight / 2.0!) - 100.0!))
			If Not hitTestEllipse.FillContainsPoint(point, 0.0!, matrix) Then
				Me._animate = False
			ElseIf Me._animateToRight Then
				Me._animateTranslateX += 1
				If (Me._animateTranslateX > (rtWidth - 220.0!)) Then
					Me._animateToRight = False
				End If
			Else
				' When moving from left to right, translate transform becomes larger and lager.
				Me._animateTranslateX -= 1
				If (Me._animateTranslateX <= 0) Then
					Me._animateToRight = True
					Me._animateTranslateX = 0
					Me._animate = False
				End If
			End If
		End If

		' Finish drawing.
		Me._renderTarget.EndDraw()
	End Sub

End Class
