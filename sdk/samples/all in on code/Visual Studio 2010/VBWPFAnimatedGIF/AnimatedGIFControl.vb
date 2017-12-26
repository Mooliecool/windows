'***************************** Module Header ******************************\
' Module Name:	AnimatedGIFControl.vb
' Project:	    VBWPFAnimatedGIF
' Copyright (c) Microsoft Corporation.
'
' The VBWPFAnimatedGIF demonstrates how to implement 
' an animated GIF image in WPF.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports System.Drawing
Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Windows
Imports System.Windows.Interop
Imports System.Windows.Media.Imaging
Imports System.Windows.Threading

Public Class AnimatedGIFControl
    Inherits System.Windows.Controls.Image

    Private _bitmap As Bitmap
    ' Local bitmap member to cache image resource
    Private _bitmapSource As BitmapSource
    Public Delegate Sub FrameUpdatedEventHandler()

    ''' <summary>
    ''' Delete local bitmap resource
    ''' Reference: http://msdn.microsoft.com/en-us/library/dd183539(VS.85).aspx
    ''' </summary>
    <DllImport("gdi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Private Shared Function DeleteObject(ByVal hObject As IntPtr) As Boolean
    End Function

    ''' <summary>
    ''' Override the OnInitialized method
    ''' </summary>
    Protected Overrides Sub OnInitialized(ByVal e As EventArgs)
        MyBase.OnInitialized(e)
    End Sub

    ''' <summary>
    ''' Load the embedded image for the Image.Source
    ''' </summary>

    Private Sub AnimatedGIFControl_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs) Handles Me.Loaded
        ' Get GIF image from Resources

        If (My.Resources.ProgressIndicator IsNot Nothing) Then
            _bitmap = My.Resources.ProgressIndicator
            Width = _bitmap.Width
            Height = _bitmap.Height

            _bitmapSource = GetBitmapSource()
            Source = _bitmapSource
        End If
    End Sub

    ''' <summary>
    ''' Close the FileStream to unlock the GIF file
    ''' </summary>
    Private Sub AnimatedGIFControl_Unloaded(ByVal sender As Object, ByVal e As RoutedEventArgs) Handles Me.Unloaded
        StopAnimate()
    End Sub

    ''' <summary>
    ''' Start animation
    ''' </summary>
    Public Sub StartAnimate()
        ImageAnimator.Animate(_bitmap, AddressOf OnFrameChanged)
    End Sub

    ''' <summary>
    ''' Stop animation
    ''' </summary>
    Public Sub StopAnimate()
        ImageAnimator.StopAnimate(_bitmap, AddressOf OnFrameChanged)
    End Sub

    ''' <summary>
    ''' Event handler for the frame changed
    ''' </summary>
    Private Sub OnFrameChanged(ByVal sender As Object, ByVal e As EventArgs)
        Dispatcher.BeginInvoke(DispatcherPriority.Normal, New FrameUpdatedEventHandler(AddressOf FrameUpdatedCallback))
    End Sub

    Private Sub FrameUpdatedCallback()
        ImageAnimator.UpdateFrames()

        If _bitmapSource IsNot Nothing Then
            _bitmapSource.Freeze()
        End If

        ' Convert the bitmap to BitmapSource that can be display in WPF Visual Tree
        _bitmapSource = GetBitmapSource()
        Source = _bitmapSource
        InvalidateVisual()
    End Sub

    Private Function GetBitmapSource() As BitmapSource
        Dim handle As IntPtr = IntPtr.Zero

        Try
            handle = _bitmap.GetHbitmap()
            _bitmapSource = Imaging.CreateBitmapSourceFromHBitmap(handle, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions())
        Finally
            If handle <> IntPtr.Zero Then
                DeleteObject(handle)
            End If
        End Try

        Return _bitmapSource
    End Function


End Class