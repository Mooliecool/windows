'****************************** Module Header ******************************\
' Module Name:  FillColor.vb
' Project:      VBSL3WriteableBitmap
' Copyright (c) Microsoft Corporation.
' 
' This example implements a common feature in painting programmers: Fill an area with a solid color. It demonstrates how to apply common algorithms to Silverlight WriteableBitmap, as well as how to avoid stack overflow in very deep recursive methods by simulating the way CLR invokes methods.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' ' 11/26/2009 18:53 Yilun Luo Created
'***************************************************************************/


Imports System.Windows.Media.Imaging

Partial Public Class FillColor
	Inherits UserControl

	Private _newColor As Integer
	Private _source As BitmapSource = New BitmapImage(New Uri("Heart Empty.png", UriKind.Relative))

	Public Sub New()
		InitializeComponent()
	End Sub

	Private Sub ColorSliderValueChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))
		If (Not Me.fillColorRectangle Is Nothing) Then
			Me.fillColorRectangle.Fill = New SolidColorBrush(Color.FromArgb(CByte(Me.alphaSlider.Value), CByte(Me.redSlider.Value), CByte(Me.greenSlider.Value), CByte(Me.blueSlider.Value)))
		End If
	End Sub

	''' <summary>
	''' Implement the edge detect algorithm.
	''' </summary>
	''' <param name="clickedPoint">The clicked point.</param>
	''' <param name="clickedColorPosition">The clicked point's postion in the pixel array.</param>
	''' <param name="clickedColor">The clicked point's color.</param>
	''' <param name="bmp">The WriteableBitmap.</param>
	Private Sub EdgeDetect(ByVal clickedPoint As Point, ByVal clickedColorPosition As Integer, ByVal clickedColor As Integer, ByVal bmp As WriteableBitmap)
		' For large images, you may get stack overflow if you use a recursive method. So let's manually walk through the "call stack" instead.
		Dim stack As New Stack(Of ParameterData)
		' Think ParameterData is the parameter you pass to the recursive method. CLR will store all parameters on the call stack. Let's do the same.
		stack.Push(New ParameterData With { _
		 .bmp = bmp, _
		 .centerColorPosition = clickedColorPosition, _
		 .centerPoint = clickedPoint _
		})
		' The call stack is larger than 0. So more methods must be invoked.
		Do While (stack.Count > 0)
			' The first step in executing a method is to pop the parameter from the call stack.
			Dim pd As ParameterData = stack.Pop

			' Left
			If (pd.centerPoint.X >= 1) Then
				Dim leftPoint As New Point((pd.centerPoint.X - 1), pd.centerPoint.Y)
				Dim leftColorPosition As Integer = (pd.centerColorPosition - 1)
				' Is the left point's color the same as the clicked point's color? If so, paint the left point with the new color. Otherwise, we don't need to continue left any more.
				If (bmp.Pixels(leftColorPosition) = clickedColor) Then
					bmp.Pixels(leftColorPosition) = Me._newColor
					' Create a new parametar, push it to the call stack, so another method will be "invoked".
					Dim pdLeft As New ParameterData With { _
					 .bmp = bmp, _
					 .centerColorPosition = leftColorPosition, _
					 .centerPoint = leftPoint _
					}
					stack.Push(pdLeft)
				End If
			End If

			' Right
			If (pd.centerPoint.X <= (bmp.PixelWidth - 2)) Then
				Dim rightPoint As New Point((pd.centerPoint.X + 1), pd.centerPoint.Y)
				Dim rightColorPosition As Integer = (pd.centerColorPosition + 1)
				If (bmp.Pixels(rightColorPosition) = clickedColor) Then
					bmp.Pixels(rightColorPosition) = Me._newColor
					Dim pdRight As New ParameterData With { _
					 .bmp = bmp, _
					 .centerColorPosition = rightColorPosition, _
					 .centerPoint = rightPoint _
					}
					stack.Push(pdRight)
				End If
			End If

			' Top
			If (pd.centerPoint.Y >= 1) Then
				Dim topPoint As New Point(pd.centerPoint.X, (pd.centerPoint.Y - 1))
				Dim topColorPosition As Integer = (pd.centerColorPosition - bmp.PixelWidth)
				If (bmp.Pixels(topColorPosition) = clickedColor) Then
					bmp.Pixels(topColorPosition) = Me._newColor
					Dim pdTop As New ParameterData With { _
					 .bmp = bmp, _
					 .centerColorPosition = topColorPosition, _
					 .centerPoint = topPoint _
					}
					stack.Push(pdTop)
				End If
			End If

			'Bottom
			If (pd.centerPoint.Y <= (bmp.PixelHeight - 2)) Then
				Dim bottomPoint As New Point(pd.centerPoint.X, (pd.centerPoint.Y + 1))
				Dim bottomColorPosition As Integer = (pd.centerColorPosition + bmp.PixelWidth)
				If (bmp.Pixels(bottomColorPosition) = clickedColor) Then
					bmp.Pixels(bottomColorPosition) = Me._newColor
					Dim pdBottom As New ParameterData With { _
					 .bmp = bmp, _
					 .centerColorPosition = bottomColorPosition, _
					 .centerPoint = bottomPoint _
					}
					stack.Push(pdBottom)
				End If
			End If
		Loop
	End Sub

	Private Sub img_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
		Dim bmp As New WriteableBitmap(Me._source)
		' Start from the clicked point.
		Dim clickedPoint As Point = e.GetPosition(Me.img)
		' The position of the clicked point in the pixel array.
		Dim clickedColorPosition As Integer = ((bmp.PixelWidth * CInt(clickedPoint.Y)) + CInt(clickedPoint.X))
		' The color of the clicked point.
		Dim clickedColor As Integer = bmp.Pixels(clickedColorPosition)
		' Get the value of the new color from the sliders.
		Me._newColor = ((((CInt(Me.alphaSlider.Value) << &H18) + (CInt(Me.redSlider.Value) << &H10)) + (CInt(Me.greenSlider.Value) << 8)) + CInt(Me.blueSlider.Value))
		' The clicked point is already in the new color. Terminate the algorithm.
		' Paint the clicked point with the new color.
		If (clickedColor <> Me._newColor) Then
			bmp.Pixels(clickedColorPosition) = Me._newColor
			Me.EdgeDetect(clickedPoint, clickedColorPosition, clickedColor, bmp)
			Me._source = bmp
			Me.img.Source = bmp
		End If
	End Sub

	Private Sub UserControl_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Me.img.Source = Me._source
	End Sub
End Class

''' <summary>
''' Use this class to simulate the paramater of a recursive method on the CLR call stack.
''' </summary>
Public Class ParameterData
	Public bmp As WriteableBitmap
	' The current point's position in the pixel array.
	Public centerColorPosition As Integer
	' The current (center) point.
	Public centerPoint As Point
End Class
