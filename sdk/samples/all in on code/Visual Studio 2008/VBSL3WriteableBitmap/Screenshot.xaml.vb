'****************************** Module Header ******************************\
' Module Name:  Screenshot.vb
' Project:      VBSL3WriteableBitmap
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates the screenshot feature of WriteableBitmap, as well as its limitations. It also implements a bmp encoder.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' ' 11/26/2009 18:53 Yilun Luo Created
'***************************************************************************/


Imports System.Windows.Threading
Imports System.Windows.Media.Imaging

Partial Public Class Screenshot
	Inherits UserControl

	Private _timer As DispatcherTimer

	Public Sub New()
		InitializeComponent()
	End Sub

	Private Sub _timer_Tick(ByVal sender As Object, ByVal e As EventArgs)
		Dim bmp As New WriteableBitmap(Me.uiElementSource, Me.uiElementSource.RenderTransform)
		Me.img.Source = bmp
	End Sub

	''' <summary>
	''' Construct a bmp file from the screenshot. For information about bmp file format, please refer to http://en.wikipedia.org/wiki/BMP_file_format.
	''' Note the limitations of WriteableBitmap's screenshot feature: Do not support projection. If the image has a RenderTransform, all empty pixels are rendered with black (000000).
	''' </summary>
	Private Sub SaveFileButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim sfd As New SaveFileDialog With { _
		 .Filter = "Bitmap Images|*.bmp" _
		}
		If sfd.ShowDialog.Value Then
			Dim stream As IO.Stream = sfd.OpenFile
			Dim bmp As New WriteableBitmap(Me.uiElementSource, Me.uiElementSource.RenderTransform)
			Dim header As Byte() = New Byte(&H36 - 1) {}
			' B
			header(0) = &H42
			' M
			header(1) = &H4D
			' Padding for 4 byte alignment (Could be a value other than zero).
			Dim padding As Integer = (bmp.PixelWidth Mod 4)
			Dim paddingData As Byte() = New Byte(padding - 1) {}
			' Calculate the size of the bitmap. Note the data is stored in the reverse order in any binary file.
			Dim size As Integer = (((bmp.Pixels.Length * 3) + 54) + (padding * bmp.PixelHeight))
			Me.Write4ByteBinaryData(header, 2, size)
			' The offset where the bitmap data (pixels) can be found.
			Me.Write4ByteBinaryData(header, 10, 54)
			' The number of bytes in the header (from this point).
			Me.Write4ByteBinaryData(header, 14, 40)
			' The width and height of the bitmap in pixels
			Me.Write4ByteBinaryData(header, 18, bmp.PixelWidth)
			Me.Write4ByteBinaryData(header, 22, bmp.PixelHeight)
			' 1 plane being used.
			header(26) = 1
			' The number of bits/pixel is 24. We do not take alpha into account.
			Me.Write2ByteBinaryData(header, 28, 24)
			' The size of the raw BMP data (after this header)
			Me.Write4ByteBinaryData(header, 34, 16)
			' The horizontal resolution of the image. 96 per inch is 3780 per meter.
			Me.Write4ByteBinaryData(header, 38, 3780)
			' The vertical resolution of the image.
			Me.Write4ByteBinaryData(header, 42, 3780)
			stream.Write(header, 0, 54)
			Dim x As Integer = 0
			Dim pixel As Integer
			For Each pixel In bmp.Pixels
				stream.WriteByte(BitConverter.GetBytes((pixel << &H18) >> &H18)(0))
				stream.WriteByte(BitConverter.GetBytes((pixel << &H10) >> &H18)(0))
				stream.WriteByte(BitConverter.GetBytes((pixel << 8) >> &H18)(0))
				x += 1
				If (x = bmp.PixelWidth) Then
					stream.Write(paddingData, 0, padding)
					x = 0
				End If
			Next
			stream.Close()
		End If
	End Sub

	Private Sub UserControl_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Me.sb.Begin()
		Me._timer = New DispatcherTimer
		Me._timer.Interval = TimeSpan.FromMilliseconds(10)
		AddHandler Me._timer.Tick, New EventHandler(AddressOf Me._timer_Tick)
		Me._timer.Start()
	End Sub

	''' <summary>
	''' Write the next 2 bytes with the integer data.
	''' </summary>
	Private Sub Write2ByteBinaryData(ByVal header As Byte(), ByVal start As Integer, ByVal data As Integer)
		header(start) = CByte(((data << &H18) >> &H18))
		header((start + 1)) = CByte(((data << &H10) >> &H18))
	End Sub

	''' <summary>
	''' Write the next 4 bytes with the integer data.
	''' </summary>
	Private Sub Write4ByteBinaryData(ByVal header As Byte(), ByVal start As Integer, ByVal data As Integer)
		header(start) = BitConverter.GetBytes((data << &H18) >> &H18)(0)
		header(start + 1) = BitConverter.GetBytes((data << &H10) >> &H18)(0)
		header(start + 2) = BitConverter.GetBytes((data << 8) >> &H18)(0)
		header(start + 3) = BitConverter.GetBytes(data >> &H18)(0)
	End Sub
End Class
