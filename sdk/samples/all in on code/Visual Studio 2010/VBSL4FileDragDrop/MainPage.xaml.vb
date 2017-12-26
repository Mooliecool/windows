'***************************** Module Header ******************************'
'* Module Name:                MainPage.xaml.vb
'* Project:                    VBSL4FileDragDrop
'* Copyright (c) Microsoft Corporation.
'* 
'* MainPage's code behind file.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\*************************************************************************'


Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media.Animation
Imports System.Windows.Media.Imaging
Imports System.IO

Partial Public Class MainPage
    Inherits UserControl
    Public Sub New()
        InitializeComponent()
    End Sub

    Private itemindex As Integer = 0

    ' Handle drop event here.
    Private Sub LayoutRoot_Drop(ByVal sender As Object, ByVal e As DragEventArgs)
        If e.Data IsNot Nothing Then
            ' Get fileinfos.
            Dim files = TryCast(e.Data.GetData(DataFormats.FileDrop), FileInfo())

            For Each file In files
                Dim image = New Image()
                If IsImage(file.Extension) Then
                    Try
                        ' Read file stream.
                        Dim stream = file.OpenRead()
                        Dim bitmapimage = New BitmapImage()
                        bitmapimage.SetSource(stream)
                        image.Source = bitmapimage
                    Catch
                        ' If read failed, use default icon .
                        image.Source = New BitmapImage(New Uri("/images/file.png", UriKind.Relative))
                    End Try
                Else
                    ' If not image type file, use default icon image.
                    image.Source = New BitmapImage(New Uri("/images/file.png", UriKind.Relative))
                End If

                image.DataContext = file
                image.Height = cellsize
                image.Width = cellsize

                Dim positon = e.GetPosition(LayoutRoot)
                LayoutRoot.Children.Add(image)
                Canvas.SetLeft(image, positon.X)
                Canvas.SetTop(image, positon.Y)

                ' Init icon drag&drop behavior
                InitIconDragDrop(image)

                ' Snap to grid
                FlyToGrid(image, itemindex)

                itemindex += 1
            Next
        End If
    End Sub

    ' Determine if is silverlight supported image format
    Private Function IsImage(ByVal extension As String) As Boolean
        extension = extension.ToLower()
        If extension = ".jpg" OrElse extension = ".jpeg" OrElse extension = ".png" Then
            Return True
        Else
            Return False
        End If
    End Function

    Private cellsize As Integer = 100

    ' Snap to grid animation
    Private Sub FlyToGrid(ByVal element As UIElement, ByVal index As Integer)
        Dim cellcolcount As Integer = CInt(LayoutRoot.ActualWidth) \ cellsize
        Dim row As Integer = index \ cellcolcount
        Dim col As Integer = index Mod cellcolcount

        Dim endposx As Integer = col * cellsize
        Dim endposy As Integer = row * cellsize

        Dim sb As New Storyboard()

        Dim dax As New DoubleAnimation()
        Storyboard.SetTarget(dax, element)
        Storyboard.SetTargetProperty(dax, New PropertyPath(Canvas.LeftProperty))
        dax.[To] = endposx
        dax.Duration = New Duration(TimeSpan.FromSeconds(0.5))
        Dim ce = New CubicEase()
        ce.EasingMode = EasingMode.EaseOut
        dax.EasingFunction = ce

        sb.Children.Add(dax)

        Dim day As New DoubleAnimation()
        Storyboard.SetTarget(day, element)
        Storyboard.SetTargetProperty(day, New PropertyPath(Canvas.TopProperty))
        day.[To] = endposy
        day.Duration = New Duration(TimeSpan.FromSeconds(0.5))
        day.EasingFunction = ce

        sb.Children.Add(day)

        sb.Begin()
    End Sub

    Private isdrag As Boolean = False
    Private relativepos As Point

    ' Initialize element drag&drop behavior
    Private Sub InitIconDragDrop(ByVal element As UIElement)

        AddHandler element.MouseLeftButtonDown, Function(s, e)
                                                    isdrag = True
                                                    element.CaptureMouse()
                                                    relativepos = e.GetPosition(element)
                                                End Function

        AddHandler element.MouseLeftButtonUp, Function(s, e)
                                                  Dim pos = e.GetPosition(LayoutRoot)
                                                  Dim colcount = CInt(LayoutRoot.ActualWidth) \ cellsize
                                                  Dim col = Math.Max(0, Math.Min(CInt(pos.X) \ cellsize, colcount - 1))
                                                  Dim rowcount = CInt(LayoutRoot.ActualHeight) \ cellsize
                                                  Dim row = Math.Max(0, Math.Min(CInt(pos.Y) \ cellsize, rowcount - 1))

                                                  Dim index = row * colcount + col

                                                  FlyToGrid(element, index)

                                                  element.ReleaseMouseCapture()
                                                  isdrag = False
                                              End Function

        AddHandler element.MouseMove, Function(s, e)
                                          If isdrag Then
                                              Dim pos = e.GetPosition(LayoutRoot)
                                              Canvas.SetLeft(element, pos.X - relativepos.X)
                                              Canvas.SetTop(element, pos.Y - relativepos.Y)
                                          End If
                                      End Function
    End Sub

End Class