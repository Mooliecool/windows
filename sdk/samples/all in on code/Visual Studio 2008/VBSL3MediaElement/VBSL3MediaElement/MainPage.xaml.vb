'***************************** Module Header ******************************\
'* Module Name:  MainPage.xaml.cs
'* Project:      VBSL3MediaElement
'* Copyright (c) Microsoft Corporation.
'* 
'* This example illustrates basic usage of MediaElement.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 10/12/2009 03:00 PM Allen Chen Created
'\**************************************************************************


Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Net
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Animation
Imports System.Windows.Shapes
Imports System.Windows.Threading

Partial Public Class MainPage
    Inherits UserControl

    Private _timer As DispatcherTimer = New DispatcherTimer()

    Public Sub New()
        InitializeComponent()
        AddHandler Loaded, AddressOf MainPage_Loaded
        _timer.Interval = TimeSpan.FromMilliseconds(100)
        AddHandler _timer.Tick, AddressOf _timer_Tick
    End Sub


    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Attach events of SilverlightHost to subscribe the 
        ' FullScreenChanged event.
        AddHandler App.Current.Host.Content.FullScreenChanged, AddressOf Content_FullScreenChanged
    End Sub


    Private Sub Content_FullScreenChanged(ByVal sender As Object, ByVal e As EventArgs)
        If (Not App.Current.Host.Content.IsFullScreen) Then
            ' When shifting back to normal mode, reset width/height of MediaElement.
            Me.MyMediaElement.Width = Double.NaN
            Me.MyMediaElement.Height = Double.NaN
            ' When shifting back to normal mode, hide FullSreenPanel and show MenuPanel
            Me.MenuPanel.Visibility = Visibility.Visible
            Me.FullScreenPanel.Visibility = Visibility.Collapsed
        Else
            ' In full-screen mode, set width/height of MediaElement to a big value
            Me.MyMediaElement.Width = App.Current.Host.Content.ActualWidth
            Me.MyMediaElement.Height = App.Current.Host.Content.ActualHeight
            ' In full-screen mode, hide MenuPanel and show FullSreenPanel
            Me.MenuPanel.Visibility = Visibility.Collapsed
            Me.FullScreenPanel.Visibility = Visibility.Visible
        End If
    End Sub

    Private Sub _timer_Tick(ByVal sender As Object, ByVal e As EventArgs)
        If Me.MyMediaElement.CurrentState = MediaElementState.Playing Then
            ' Update Status of controls
            Dim ts As TimeSpan = Me.MyMediaElement.Position
            Me.CurrentPositionTextBlock.Text = String.Format("{0}:{1}:{2}/", ts.Hours, ts.Minutes, ts.Seconds)
            Me.CurrentPositionSlider.Value = ts.TotalMilliseconds
        ElseIf Me.MyMediaElement.CurrentState = MediaElementState.Stopped Then
            ' Reset status of controls
            Me.CurrentPositionTextBlock.Text = "0:0:0/"
            Me.CaptionTextBlock.Text = String.Empty
            Me.CurrentPositionSlider.Value = 0
        End If
    End Sub

    Private Sub PlayButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyMediaElement.Play()
        Me._timer.Start()
    End Sub

    Private Sub PauseButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyMediaElement.Pause()
    End Sub
    Private Sub StopButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyMediaElement.Stop()
    End Sub

    Private Sub MyMediaElement_CurrentStateChanged(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Show current state of MediaElement. Note only 4 states are catched here.
        ' You can add more when creating your own media player.
        Select Case Me.MyMediaElement.CurrentState
            Case MediaElementState.Stopped
                Me.StatusTextBlock.Text = "Stopped"
                Exit Select
            Case MediaElementState.Paused
                Me.StatusTextBlock.Text = "Paused"
                Exit Select
            Case MediaElementState.Playing
                Me.StatusTextBlock.Text = "Playing"
                Exit Select
            Case MediaElementState.Buffering
                Me.StatusTextBlock.Text = "Buffering"
                Exit Select
        End Select
    End Sub

    Private Sub MyMediaElement_MediaOpened(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim ts As TimeSpan = Me.MyMediaElement.NaturalDuration.TimeSpan
        Me.DurationTextBlock.Text = String.Format("{0}:{1}:{2}", ts.Hours, ts.Minutes, ts.Seconds)
        ' Init CurrentPosition Slider
        Me.CurrentPositionSlider.Maximum = ts.TotalMilliseconds
        Me.CurrentPositionSlider.Minimum = 0
        ' Init VolumeSlider
        Me.VolumeSlider.Value = Me.MyMediaElement.Volume
        ' Add a marker by code
        Dim timelinemarker As TimelineMarker = New TimelineMarker()
        timelinemarker.Text = "This marker is added by code!"
        timelinemarker.Type = "MyMarker"
        timelinemarker.Time = TimeSpan.FromSeconds(8)
        Me.MyMediaElement.Markers.Add(timelinemarker)

        ' Enable progress bar
        Me.CurrentPositionSlider.IsEnabled = True
    End Sub

    Private Sub MyMediaElement_MediaEnded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.MyMediaElement.Stop()
    End Sub

    Private Sub HorizontalThumb_DragStarted(ByVal sender As Object, ByVal e As System.Windows.Controls.Primitives.DragStartedEventArgs)
        Me.MyMediaElement.Pause()
    End Sub

    Private Sub HorizontalThumb_DragCompleted(ByVal sender As Object, ByVal e As System.Windows.Controls.Primitives.DragCompletedEventArgs)
        Me.MyMediaElement.Position = TimeSpan.FromMilliseconds(Me.CurrentPositionSlider.Value)
        Me.MyMediaElement.Play()
    End Sub

    Private Sub MyMediaElement_MarkerReached(ByVal sender As Object, ByVal e As TimelineMarkerRoutedEventArgs)
        If e.Marker.Type = "MyMarker" Then
            ' Show marker text made by code
            Me.CaptionTextBlock.FontSize = 22
            Me.CaptionTextBlock.Text = e.Marker.Text

        Else
            ' Show marker text made by Expression Blend 3
            ' Because the caption is "out of" the video you get more flexibility to customizing it
            Me.CaptionTextBlock.FontSize = 12
            Me.CaptionTextBlock.Text = e.Marker.Text
        End If
    End Sub

    Private Sub FullScreenButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Shift to full-screen mode
        App.Current.Host.Content.IsFullScreen = Not App.Current.Host.Content.IsFullScreen
    End Sub

    Private Sub VolumeSlider_ValueChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))
        Me.MyMediaElement.Volume = Me.VolumeSlider.Value
    End Sub

    Private Sub MyMediaElement_MediaFailed(ByVal sender As Object, ByVal e As ExceptionRoutedEventArgs)
        Me.CurrentPositionSlider.IsEnabled = False
        MessageBox.Show(e.ErrorException.Message & ". Please make sure you're viewing CSSL3MediaElementTestPage.aspx")

    End Sub

End Class