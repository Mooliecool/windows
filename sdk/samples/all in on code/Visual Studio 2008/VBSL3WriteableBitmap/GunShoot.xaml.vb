'****************************** Module Header ******************************\
' Module Name:  GunShoot.vb
' Project:      VBSL3WriteableBitmap
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to scale two images of different size to the same resolution, as well as how to work with individual pixels.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' ' 11/26/2009 18:53 Yilun Luo Created
'***************************************************************************/

Imports System.Windows.Media.Imaging
Imports System.IO

Partial Public Class GunShoot
	Inherits UserControl

	Private Shared _radius As Integer = 50
	Private Shared _resultPixelHeight As Integer = 600
	Private Shared _resultPixelWidth As Integer = 800
	Private _writeableBottom As WriteableBitmap
	Private _writeableTop As WriteableBitmap


	Public Sub New()
		InitializeComponent()
		Me._writeableTop = New WriteableBitmap(GunShoot._resultPixelWidth, GunShoot._resultPixelHeight)
		Me._writeableBottom = New WriteableBitmap(GunShoot._resultPixelWidth, GunShoot._resultPixelHeight)
	End Sub

	Private Sub BrowseButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim ofd As New OpenFileDialog With { _
		 .Multiselect = True, _
		 .Filter = "Bitmap Images|*.png;*.jpg" _
		}
		If ofd.ShowDialog.Value Then
			Dim fileCount As Integer = ofd.Files.Count()
			If (ofd.Files.Count() <> 2) Then
				MessageBox.Show("Please select at exactly 2 images.")
			Else
				Me._writeableBottom = New WriteableBitmap(GunShoot._resultPixelWidth, GunShoot._resultPixelHeight)
				Dim resolution As Integer = (GunShoot._resultPixelWidth * GunShoot._resultPixelHeight)
				Dim files As List(Of FileInfo) = ofd.Files.ToList()
				Me.ScaleImage(resolution, files.Item(0), Me._writeableBottom)
				Me.ScaleImage(resolution, files.Item(1), Me._writeableTop)
				Me.topImage.Source = Me._writeableTop
			End If
		End If
	End Sub

	''' <summary>
	''' Scales the source image to a fixed resolution.
	''' </summary>
	Private Sub ScaleImage(ByVal resolution As Integer, ByVal file As FileInfo, ByVal writeableResult As WriteableBitmap)
		Using fs As FileStream = file.OpenRead
			Dim bmpSource As New BitmapImage
			bmpSource.SetSource(fs)
			Dim writeableSource As New WriteableBitmap(bmpSource)
			' Scale the source image to be the same size as the result image
			Dim ratioX As Double = (CDbl(writeableSource.PixelWidth) / CDbl(writeableResult.PixelWidth))
			Dim ratioY As Double = (CDbl(writeableSource.PixelHeight) / CDbl(writeableResult.PixelHeight))
			Dim i As Integer
			For i = 0 To resolution - 1
				Dim yDest As Double = (i / Me._writeableBottom.PixelWidth)
				Dim xDest As Double = (i Mod Me._writeableBottom.PixelWidth)
				Dim y As Integer = CInt(Math.Floor((yDest * ratioY)))
				Dim x As Integer = CInt(Math.Floor((xDest * ratioX)))
				writeableResult.Pixels(i) = writeableSource.Pixels(y * writeableSource.PixelWidth + x)
			Next i
		End Using
	End Sub

	Private Sub topImage_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
		Dim clickedPoint As Point = e.GetPosition(Me.topImage)
		' Calculate the rectangle in which the gun shoots, so we don't need to iterate through all pixels.
		Dim topLeftX As Integer = (CInt(clickedPoint.X) - GunShoot._radius)
		If (topLeftX < 0) Then
			topLeftX = 0
		End If
		Dim topLeftY As Integer = (CInt(clickedPoint.Y) - GunShoot._radius)
		If (topLeftY < 0) Then
			topLeftY = 0
		End If
		Dim bottomRightX As Integer = (CInt(clickedPoint.X) + GunShoot._radius)
		If (bottomRightX > GunShoot._resultPixelWidth) Then
			bottomRightX = GunShoot._resultPixelWidth
		End If
		Dim bottomRightY As Integer = (CInt(clickedPoint.Y) + GunShoot._radius)
		If (bottomRightY > GunShoot._resultPixelHeight) Then
			bottomRightY = GunShoot._resultPixelHeight
		End If
		Dim y As Integer
		For y = topLeftY To bottomRightY - 1
			Dim x As Integer
			For x = topLeftX To bottomRightX - 1
				' Find the position of the current pixel in the pixel's array.
				Dim position As Integer = ((y * GunShoot._resultPixelWidth) + x)
				' The gun mark is a circle. So let's calculate the distance from the current point to the center point.
				Dim distance As Double = Math.Sqrt((Math.Pow((clickedPoint.X - x), 2) + Math.Pow((clickedPoint.Y - y), 2)))
				' If the distance is less then radius, the current point is in the gun mark's circle.
				If (distance <= GunShoot._radius) Then
					' The pixel in the top image.
					Dim currentPixel As UInt32 = BitConverter.ToUInt32(BitConverter.GetBytes(Me._writeableTop.Pixels(position)), 0)
					' The pixel in the bottom image.
					Dim blendPixel As UInt32 = BitConverter.ToUInt32(BitConverter.GetBytes(Me._writeableBottom.Pixels(position)), 0)
					' Let's draw a gun mark for 5 pixels.
					If (distance > (GunShoot._radius - 5)) Then
						' Obtain the RGB value of the pixels. The left side of the right shift operation must be uint, so that 0 will be filled when the operation is performed.
						Dim currentPixelR As UInt32 = ((currentPixel << 8) >> 24)
						Dim currentPixelG As UInt32 = ((currentPixel << 16) >> 24)
						Dim currentPixelB As UInt32 = ((currentPixel << 24) >> 24)
						Dim blendPixelR As UInt32 = ((blendPixel << 8) >> 24)
						Dim blendPixelG As UInt32 = ((blendPixel << 16) >> 24)
						Dim blendPixelB As UInt32 = ((blendPixel << 24) >> 24)
						' Make the pixel a little redder and reduce its alpha to represent the gun mark.
						currentPixelR = (currentPixelR + &H66)
						If (currentPixelR > &HFF) Then
							currentPixelR = &HFF
						End If
						Dim alpha As Double = 0.3
						' The standard formula to calculate a pixel's value: currentPixel * alpha + thePixelBelowIt * (1 - alpha).
						Dim newPixelR As UInt32 = CUInt(((currentPixelR * alpha) + (blendPixelR * (1 - alpha))))
						Dim newPixelG As UInt32 = CUInt(((currentPixelG * alpha) + (blendPixelG * (1 - alpha))))
						Dim newPixelB As UInt32 = CUInt(((currentPixelB * alpha) + (blendPixelB * (1 - alpha))))
						Dim newPixel As Long = ((((newPixelR << &H10) + (newPixelG << 8)) + newPixelB) + &HFF000000)
						Me._writeableTop.Pixels(position) = CInt(newPixel)
						' For the center of the gun mark, just copy the bottom image's value.
					Else
						Me._writeableTop.Pixels(position) = Me._writeableBottom.Pixels(position)
					End If
				End If
			Next x
		Next y
		Me.topImage.Source = Me._writeableTop
	End Sub
End Class
