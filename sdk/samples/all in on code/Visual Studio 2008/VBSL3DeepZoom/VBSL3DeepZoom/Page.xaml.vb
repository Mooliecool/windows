'****************************** Module Header ******************************\
' Module Name:  Page.cs
' Project:      CSSL3DeepZoomProject
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to work with deep zoom programmatically in Silverlight using C#.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 9/4/2009 17:12 Yilun Luo Created
'***************************************************************************/

Imports System.ServiceModel
Imports System.Xml.Linq
Imports System.Windows.Controls.Primitives
Imports VBSL3DeepZoom.DeepZoomServiceReference

Partial Public Class Page
	Inherits UserControl

	Public Sub New()
		Dim handler As MouseEventHandler = Nothing
		Dim handler2 As MouseButtonEventHandler = Nothing
		Dim handler3 As MouseButtonEventHandler = Nothing
		Dim handler4 As MouseEventHandler = Nothing
		Dim handler5 As EventHandler(Of MouseWheelEventArgs) = Nothing
		Me._zoom = 1
		Me._duringDrag = False
		Me._mouseDown = False
		Me._lastMouseDownPos = New Point
		Me._lastMousePos = New Point
		Me._lastMouseViewPort = New Point
		Me.InitializeComponent()

		'Call a WCF service to download source images and generate deep zoom content.
		'Begin Comment:
		'Comment the following lines if you want to provide your own content.
		Dim str As String = Application.Current.Host.Source.ToString()
		Dim remoteAddress As New EndpointAddress((str.Substring(0, str.LastIndexOf("/ClientBin")) & "/GenerateDeepZoomService.svc"))
		Dim binding As New BasicHttpBinding
		Dim client As New GenerateDeepZoomServiceClient(binding, remoteAddress)
		AddHandler client.PrepareDeepZoomCompleted, New EventHandler(Of PrepareDeepZoomCompletedEventArgs)(AddressOf Me.client_PrepareDeepZoomCompleted)
		'Pass true if you want to force programatically generating deep zoom content. Pass false if you want to avoid generating the content again when it already exists.
		client.PrepareDeepZoomAsync(False)
		'End Comment.

		'Download Metadata.xml.
		'Begin download metadata.
		Dim client2 As New WebClient
		AddHandler client2.DownloadStringCompleted, New DownloadStringCompletedEventHandler(AddressOf Me.webClient_DownloadStringCompleted)
		client2.DownloadStringAsync(New Uri("GeneratedImages/Metadata.xml", UriKind.Relative))
		'End download metadata.

		AddHandler MyBase.Loaded, New RoutedEventHandler(AddressOf Me.Page_Loaded)
		'Firing an event when the MultiScaleImage is Loaded
		AddHandler Me.msi.Loaded, New RoutedEventHandler(AddressOf Me.msi_Loaded)
		'Firing an event when all of the images have been Loaded
		AddHandler Me.msi.ImageOpenSucceeded, New RoutedEventHandler(AddressOf Me.msi_ImageOpenSucceeded)

		'Handling all of the mouse and keyboard functionality
		If (handler Is Nothing) Then
			handler = AddressOf AnnoymousMethod_Hander
		End If
		AddHandler MyBase.MouseMove, handler
		If (handler2 Is Nothing) Then
			handler2 = AddressOf AnnoymousMethod_Hander2
		End If
		AddHandler MyBase.MouseLeftButtonDown, handler2
		If (handler3 Is Nothing) Then
			handler3 = AddressOf AnnoymousMethod_Hander3
		End If
		AddHandler MyBase.MouseLeftButtonUp, handler3
		If (handler4 Is Nothing) Then
			handler4 = AddressOf AnnoymousMethod_Hander4
		End If
		AddHandler MyBase.MouseMove, handler4
		If (handler5 Is Nothing) Then
			handler5 = AddressOf AnnoymousMethod_Hander5
		End If
		AddHandler New MouseWheelHelper(Me).Moved, handler5
	End Sub

	Sub AnnoymousMethod_Hander(ByVal sender As Object, ByVal e As MouseEventArgs)
		Me._lastMousePos = e.GetPosition(Me.msi)
		If Me._duringDrag Then
			Dim point As Point = Me._lastMouseViewPort
			point.X = (point.X + (((Me._lastMouseDownPos.X - Me._lastMousePos.X) / Me.msi.ActualWidth) * Me.msi.ViewportWidth))
			point.Y = (point.Y + (((Me._lastMouseDownPos.Y - Me._lastMousePos.Y) / Me.msi.ActualWidth) * Me.msi.ViewportWidth))
			Me.msi.ViewportOrigin = point
		End If
	End Sub

	Sub AnnoymousMethod_Hander2(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
		If Me._msiLoaded Then
			Me._lastMouseDownPos = e.GetPosition(Me.msi)
			Me._lastMouseViewPort = Me.msi.ViewportOrigin
			Me._mouseDown = True
			Me.msi.CaptureMouse()
		End If
	End Sub

	Sub AnnoymousMethod_Hander3(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
		If Me._msiLoaded Then
			Me._mouseDown = False
			Me.msi.ReleaseMouseCapture()

			'Move the ball to the new position.
			If Not (Not Me.moveBallToggleButton.IsChecked.Value OrElse Me._duringDrag) Then
				Me.MoveBallAndSubImages(e)
			ElseIf Not Me._duringDrag Then
				Dim flag As Boolean = ((Keyboard.Modifiers And ModifierKeys.Shift) = ModifierKeys.Shift)
				Dim newzoom As Double = Me._zoom
				If flag Then
					newzoom = (newzoom / 2)
				Else
					newzoom = (newzoom * 2)
				End If
				Me.Zoom(newzoom, Me.msi.ElementToLogicalPoint(Me._lastMousePos))
			End If
			Me._duringDrag = False
		End If
	End Sub

	Sub AnnoymousMethod_Hander4(ByVal sender As Object, ByVal e As MouseEventArgs)
		Dim point2 As Point
		Me._lastMousePos = e.GetPosition(Me.msi)
		If Not (Not Me._mouseDown OrElse Me._duringDrag) Then
			Me._duringDrag = True
			Dim viewportWidth As Double = Me.msi.ViewportWidth
			Dim point As New Point(Me.msi.ViewportOrigin.X, Me.msi.ViewportOrigin.Y)
			Me.msi.ViewportOrigin = New Point(point.X, point.Y)
			Me.msi.ViewportWidth = viewportWidth
			Me._zoom = (1 / viewportWidth)
		ElseIf Me._mouseDown Then
			Me._currentPosition = e.GetPosition(Me.msi)
			point2 = Me._lastMouseViewPort
		End If

		'Update the ball's matrix's OffsetX/Y when dragging.
		If Me._duringDrag Then
			Me.UpdateTranslation()
		End If
		Me._previousPositon = New Point?(e.GetPosition(Me.msi))

		'Displays the tag for a sub image that hit tested.
		Me.HitTestImage()

		If Me._duringDrag Then
			point2 = Me._lastMouseViewPort
			point2.X = (point2.X + (((Me._lastMouseDownPos.X - Me._lastMousePos.X) / Me.msi.ActualWidth) * Me.msi.ViewportWidth))
			point2.Y = (point2.Y + (((Me._lastMouseDownPos.Y - Me._lastMousePos.Y) / Me.msi.ActualWidth) * Me.msi.ViewportWidth))
			Me.msi.ViewportOrigin = point2
		End If
	End Sub

	Sub AnnoymousMethod_Hander5(ByVal sender As Object, ByVal e As MouseWheelEventArgs)
		e.Handled = True
		Dim newzoom As Double = Me._zoom
		If (e.Delta < 0) Then
			newzoom = (newzoom / 1.3)
		Else
			newzoom = (newzoom * 1.3)
		End If
		Me.Zoom(newzoom, Me.msi.ElementToLogicalPoint(Me._lastMousePos))
		Me.msi.CaptureMouse()
	End Sub

	Private Sub Page_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim str As String = Application.Current.Resources.Item("path").ToString
		Dim s As String = Application.Current.Resources.Item("zoomIn").ToString
		Me._zoom = Integer.Parse(s)
		Me._zoom = 1

		'Uncomment this line if you want to provide your own source, without generate the deep zoom content programatically.
		'this.DisplayDeepZoom();
	End Sub

	''' <summary>
	''' Display the tag for a sub image that hit tested.
	''' </summary>
	Private Sub HitTestImage()
		Dim i As Integer
		For i = 0 To Me.msi.SubImages.Count - 1
			Dim control As ConversationControl
			Dim subImage As MultiScaleSubImage = Me.msi.SubImages.Item(i)
			If Me.HitTest(subImage) Then
				'ZOrder starts from 1 rather than 0.
				Dim metadata As ImageMetadata = Me._imageMetadatas.Item((i + 1))
				control = Me._conversations.Item((i + 1))
				Dim point As Point = Me.msi.LogicalToElementPoint(New Point((-subImage.ViewportOrigin.X / subImage.ViewportWidth), ((-subImage.ViewportOrigin.Y / subImage.ViewportWidth) + ((1 / subImage.ViewportWidth) / subImage.AspectRatio))))
				control.translate.X = point.X
				control.translate.Y = point.Y
				control.Visibility = Visibility.Visible
			Else
				control = Me._conversations.Item((i + 1))
				control.Visibility = Visibility.Collapsed
			End If
		Next i
	End Sub

	'''<summary>
	'''Perform a hit test on the sub image.
	'''</summary>
	'''<param name="subimage">The sub image.</param>
	Private Function HitTest(ByVal subImage As MultiScaleSubImage) As Boolean
		'ViewportWitdh determines how large the sub image is.
		'Calculate the width and height of the sub image when its ViewportWidth is 1. That is, the original size. When you zoom out, ViewportWidth will increase, and when you zoom in, ViewportWidth will decrease.
		Dim width As Double = (Me.msi.ActualWidth / (Me.msi.ViewportWidth * subImage.ViewportWidth))
		'There's no ViewportHeight property. It is calculated by ViewportWidth * AspectRatio.
		Dim height As Double = (Me.msi.ActualWidth / ((Me.msi.ViewportWidth * subImage.ViewportWidth) * subImage.AspectRatio))
		'ViewportOrigin determines where the image is. Note the coordinate is always 0 or negative. Together with ViewportWidth, we can find the top left point of the sub image.
		Dim point As Point = Me.msi.LogicalToElementPoint(New Point((-subImage.ViewportOrigin.X / subImage.ViewportWidth), (-subImage.ViewportOrigin.Y / subImage.ViewportWidth)))
		Dim rect As New Rect(point.X, point.Y, width, height)
		Return rect.Contains(Me._lastMousePos)
	End Function

	''' <summary>
	''' Update the ball's matrix's OffsetX/Y when dragging.
	''' </summary>
	Private Sub UpdateTranslation()
		Dim num As Double = Me.ballTransform.Matrix.M11
		Dim num2 As Double = Me.ballTransform.Matrix.M11
		Dim offsetX As Double = Me.ballTransform.Matrix.OffsetX
		Dim offsetY As Double = Me.ballTransform.Matrix.OffsetY
		If Me._previousPositon.HasValue Then
			offsetX = (offsetX + (Me._lastMousePos.X - Me._previousPositon.Value.X))
			offsetY = (offsetY + (Me._lastMousePos.Y - Me._previousPositon.Value.Y))
		End If
		Dim matrix As New Matrix(num, Me.ballTransform.Matrix.M12, Me.ballTransform.Matrix.M21, num2, offsetX, offsetY)
		Me.ballTransform.Matrix = matrix
	End Sub

	''' <summary>
	''' Move the ball to the clicked position. Move sub images as well.
	''' </summary>
	Private Sub MoveBallAndSubImages(ByVal e As MouseButtonEventArgs)
		Dim position As Point = e.GetPosition(Me.ball)
		'The ball may have been scaled. So the delta reported by e.GetPostion must take scaling into account.
		position = New Point((position.X * Me.ballTransform.Matrix.M11), (position.Y * Me.ballTransform.Matrix.M11))
		'Move the ball.
		Dim matrix As New Matrix(Me.ballTransform.Matrix.M11, 0, 0, Me.ballTransform.Matrix.M22, (Me.ballTransform.Matrix.OffsetX + position.X), (Me.ballTransform.Matrix.OffsetY + position.Y))
		Me.ballTransform.Matrix = matrix

		'Use ElementToLogicalPoint to translate delta from the global coordinate to the MultiScaleImage's logical coordinate system.
		Dim point2 As Point = Me.msi.ElementToLogicalPoint(position)
		'Move every sub image, except the background.
		Dim i As Integer
		For i = 1 To Me.msi.SubImages.Count - 1
			Me.msi.SubImages.Item(i).ViewportOrigin = New Point((Me.msi.SubImages.Item(i).ViewportOrigin.X - ((point2.X - Me.msi.ViewportOrigin.X) * Me.msi.SubImages.Item(i).ViewportWidth)), (Me.msi.SubImages.Item(i).ViewportOrigin.Y - ((point2.Y - Me.msi.ViewportOrigin.Y) * Me.msi.SubImages.Item(i).ViewportWidth)))
		Next i
	End Sub

	''' <summary>
	''' When Metadata.xml is downloaded, let's parse it and create a list of ImageMetadata objects. ZOrder can be used to uniquely identify an image. So let's use it as the key of our dictionary for faster searching.
	''' </summary>
	Private Sub webClient_DownloadStringCompleted(ByVal sender As Object, ByVal e As DownloadStringCompletedEventArgs)
		Dim document As XDocument = XDocument.Parse(e.Result)
		Me._imageMetadatas = New Dictionary(Of Integer, ImageMetadata)
		Me._conversations = New Dictionary(Of Integer, ConversationControl)
		Dim enumerable As IEnumerable(Of XElement) = document.Root.Elements("Image")
		Dim element As XElement
		For Each element In enumerable
			Dim key As Integer = Integer.Parse(element.Element("ZOrder").Value)
			Dim metadata As New ImageMetadata
			metadata.FileName = element.Element("FileName").Value
			metadata.X = Double.Parse(element.Element("x").Value)
			metadata.Y = Double.Parse(element.Element("y").Value)
			metadata.Width = Double.Parse(element.Element("Width").Value)
			metadata.Height = Double.Parse(element.Element("Height").Value)
			metadata.ZOrder = key
			metadata.Tag = element.Element("Tag").Value
			Me._imageMetadatas.Add(key, metadata)
			Dim control As New ConversationControl
			control.ConversationText = metadata.Tag
			control.Visibility = Visibility.Collapsed
			Me.LayoutRoot.Children.Add(control)
			Me._conversations.Add(key, control)
		Next
	End Sub

	Private Sub msi_ImageOpenSucceeded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		'Store the original ViewportOrigins, so we can "go home" very easily.
		Dim count As Integer = Me.msi.SubImages.Count
		Me._originalSubImageViewportOrigions = New Point(count - 1) {}
		Dim i As Integer
		For i = 0 To count - 1
			Me._originalSubImageViewportOrigions(i) = Me.msi.SubImages.Item(i).ViewportOrigin
		Next i
		Me.msi.ViewportWidth = 1
	End Sub

	''' <summary>
	''' Zoom the ball together with the MultiScaleImage.
	''' </summary>
	Private Sub Zoom(ByVal newzoom As Double, ByVal p As Point)
		If (newzoom < 0.5) Then
			newzoom = 0.5
		End If
		Dim relativeTransform As MatrixTransform = DirectCast(Me.ball.TransformToVisual(Me.msi), MatrixTransform)
		Dim point As Point = Me.msi.ElementToLogicalPoint(New Point(relativeTransform.Matrix.OffsetX, relativeTransform.Matrix.OffsetY))
		Me.msi.ZoomAboutLogicalPoint((newzoom / Me._zoom), p.X, p.Y)
		Me.ZoomBall(newzoom, relativeTransform)
		Me._zoom = newzoom
	End Sub

	''' <summary>
	''' Zoom the ball.
	''' </summary>
	Private Sub ZoomBall(ByVal newzoom As Double, ByVal relativeTransform As MatrixTransform)
		'The mouse position minuse top left of the ball's bound.
		Dim num As Double = (Me._lastMousePos.X - relativeTransform.Matrix.OffsetX)
		Dim num2 As Double = (Me._lastMousePos.Y - relativeTransform.Matrix.OffsetY)
		'Pass the new zoom to M11 and M22.
		Dim num3 As Double = newzoom
		Dim num4 As Double = newzoom
		'The new offset is calculated by multiply the delta with (1 - this time's zoom value). Then let's add it to the previous offset. 
		Dim offsetX As Double = (Me.ballTransform.Matrix.OffsetX + (num * (1 - (newzoom / Me.ballTransform.Matrix.M11))))
		Dim offsetY As Double = (Me.ballTransform.Matrix.OffsetY + (num2 * (1 - (newzoom / Me.ballTransform.Matrix.M22))))
		Dim matrix As New Matrix(num3, Me.ballTransform.Matrix.M12, Me.ballTransform.Matrix.M21, num4, offsetX, offsetY)
		Me.ballTransform.Matrix = matrix
	End Sub

	Private Sub GoHomeClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Me.msi.ViewportWidth = 1
		Me.msi.ViewportOrigin = New Point(0, 0)
		Me.ballTransform.Matrix = New Matrix
		Dim count As Integer = Me.msi.SubImages.Count
		Dim i As Integer
		For i = 0 To count - 1
			Me.msi.SubImages.Item(i).ViewportOrigin = Me._originalSubImageViewportOrigions(i)
		Next i
		Me.ZoomFactor = 1
	End Sub

	'Handling the VSM states
	Private Sub LeaveMovie(ByVal sender As Object, ByVal e As MouseEventArgs)
		VisualStateManager.GoToState(Me, "FadeOut", True)
	End Sub

	Private Sub EnterMovie(ByVal sender As Object, ByVal e As MouseEventArgs)
		VisualStateManager.GoToState(Me, "FadeIn", True)
	End Sub

	'unused functions that show the inner math of Deep Zoom
	Public Function getImageRect() As Rect
		Return New Rect((-Me.msi.ViewportOrigin.X / Me.msi.ViewportWidth), (-Me.msi.ViewportOrigin.Y / Me.msi.ViewportWidth), (1 / Me.msi.ViewportWidth), ((1 / Me.msi.ViewportWidth) * Me.msi.AspectRatio))
	End Function

	Public Function ZoomAboutPoint(ByVal img As Rect, ByVal zAmount As Double, ByVal pt As Point) As Rect
		Return New Rect((pt.X + ((img.X - pt.X) / zAmount)), (pt.Y + ((img.Y - pt.Y) / zAmount)), (img.Width / zAmount), (img.Height / zAmount))
	End Function

	Public Sub LayoutDZI(ByVal rect As Rect)
		Dim aspectRatio As Double = Me.msi.AspectRatio
		Me.msi.ViewportWidth = (1 / rect.Width)
		Me.msi.ViewportOrigin = New Point((-rect.Left / rect.Width), (-rect.Top / rect.Width))
	End Sub

	Private Sub msi_ViewportChanged(ByVal sender As Object, ByVal e As RoutedEventArgs)
	End Sub

	''' <summary>
	''' This method is a helper event handlers that makes your user experience better.
	''' </summary>
	Private Sub client_PrepareDeepZoomCompleted(ByVal sender As Object, ByVal e As PrepareDeepZoomCompletedEventArgs)
		If e.Result Then
			Me.DisplayDeepZoom()
		Else
			Me.informationTextBlock.Text = "Failed to generate the deep zoom content. Please check if you have write access to the DeepZoomProjectSite\ClientBin\GeneratedImages directory. If you're using a local IIS, please make sure Network Service has the write access. If you're using Windows Azure, please modify the sample to output to a path in local storage. Then you can upload the output files to a public container in blob storage. MultiScaleImage is able to access a public container in blob storage."
		End If
	End Sub

	''' <summary>
	''' Call this method if you choose to porvide your own deep zoom content.
	''' </summary>
	Private Sub DisplayDeepZoom()
		Me.informationTextBlock.Text = "All done! Enjoy!"
		Me.informationTextBlock.Visibility = Visibility.Collapsed
		Me.informationProgressBar.Visibility = Visibility.Collapsed
		Me.msi.Visibility = Visibility.Visible
		Dim uriString As String = Application.Current.Resources.Item("path").ToString
		Me.msi.Source = New DeepZoomImageTileSource(New Uri(uriString, UriKind.Relative))
		Me._msiLoaded = True
	End Sub

	Private Sub msi_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
	End Sub


	Public Property ZoomFactor() As Double
		Get
			Return Me._zoom
		End Get
		Set(ByVal value As Double)
			Me._zoom = value
		End Set
	End Property


	' Fields
	Private _conversations As Dictionary(Of Integer, ConversationControl)
	Private _currentPosition As Point
	Private _duringDrag As Boolean
	Private _imageMetadatas As Dictionary(Of Integer, ImageMetadata)
	Private _lastMouseDownPos As Point
	Private _lastMousePos As Point
	Private _lastMouseViewPort As Point
	Private _mouseDown As Boolean
	Private _msiLoaded As Boolean
	Private _originalSubImageViewportOrigions As Point()
	Private _previousPositon As Point?
	Private _zoom As Double
End Class
