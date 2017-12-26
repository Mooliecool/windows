'****************************** Module Header ******************************'
' Module Name:                MainPage.xaml.vb
' Project:                    VBSL4WCFNetTcp
' Copyright (c) Microsoft Corporation.
' 
' MainPage's code hehind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports VBSL4WCFNetTcp.ServiceReference1

Partial Public Class MainPage
    Inherits UserControl
    Implements IWeatherServiceCallback
    Public Sub New()
        InitializeComponent()
        AddHandler Me.Loaded, AddressOf MainPage_Loaded
    End Sub

    Private _subscribed As Boolean
    Private _client As WeatherServiceClient
    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        _client = New WeatherServiceClient(New System.ServiceModel.InstanceContext(Me))
        AddHandler _client.SubscribeCompleted, AddressOf _client_SubscribeCompleted
        AddHandler _client.UnSubscribeCompleted, AddressOf _client_UnSubscribeCompleted
    End Sub

    Private Sub _client_UnSubscribeCompleted(ByVal sender As Object, ByVal e As System.ComponentModel.AsyncCompletedEventArgs)
        If e.[Error] Is Nothing Then
            _subscribed = False
            btnSubscribe.Content = "Subscribe weather report"
            tbInfo.Text = ""
        Else
            tbInfo.Text = "Unable to connect to service."
        End If
        btnSubscribe.IsEnabled = True
    End Sub

    Private Sub _client_SubscribeCompleted(ByVal sender As Object, ByVal e As System.ComponentModel.AsyncCompletedEventArgs)
        If e.[Error] Is Nothing Then
            _subscribed = True
            btnSubscribe.Content = "UnSubscribe weather report"
            tbInfo.Text = ""
        Else
            tbInfo.Text = "Unable to connect to service."
        End If
        btnSubscribe.IsEnabled = True
    End Sub

    Private Sub Button_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        If Not _subscribed Then
            _client.SubscribeAsync()
        Else
            _client.UnSubscribeAsync()
        End If
        btnSubscribe.IsEnabled = False
    End Sub

    ' Display report when callback channel get called.
    Public Sub WeatherReport(ByVal report As String) Implements IWeatherServiceCallback.WeatherReport
        lbWeather.Items.Insert(0, New ListBoxItem() With { _
         .Content = String.Format("{0} {1}", DateTime.Now, report) _
        })
    End Sub
End Class
