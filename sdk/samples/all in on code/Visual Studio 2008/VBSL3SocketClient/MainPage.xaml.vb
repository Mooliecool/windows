'****************************** Module Header ******************************'
' Module Name:                 MainPage.xaml.cs
' Project:                     CSSL3Socket
' Copyright (c) Microsoft Corporation.
' 
' Silverlight socket client codebehind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

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
Imports System.Net.Sockets
Imports System.Text
Imports System.Threading
Imports VBSL3SocketClient.VBSL3SocketClient

Partial Public Class MainPage
    Inherits UserControl
    Public Sub New()
        InitializeComponent()
    End Sub

    Protected Overrides Sub Finalize()
        Try
            If _client IsNot Nothing Then
                _client.Close()
            End If
        Finally
            MyBase.Finalize()
        End Try
    End Sub

    Private _client As SocketClient

    ' Handle "Connect" button click event.
    Private Sub btnConnect_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        btnConnect.IsEnabled = False
        OpenSocketClientAsync(tboxServerAddress.Text)
    End Sub

    ' Handle "Send" button click event.
    Private Sub btnSend_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Try
            If _client IsNot Nothing Then
                _client.SendAsync(tboxMessage.Text)
            End If
        Catch ex As Exception
            MessageBox.Show("Exception occured when sending message:" & ex.Message)
            CloseSocketClient()
            Exit Sub
        End Try
    End Sub

    ' Close SocketClient
    Private Sub CloseSocketClient()
        If _client IsNot Nothing Then
            _client.Close()
            _client = Nothing
        End If

        ' Update UI
        btnConnect.IsEnabled = True
        btnSend.IsEnabled = False
    End Sub

    ' Create SocketClient and connect to server
    '
    ' For convience, we use 127.0.0.1 address to connect socket server
    ' at local. To let client access socket server at differnt machine
    ' on network, input server ip address in textbox and click 'connect'.
    ' To let socket server accessible from network, please see the 
    ' comment in CCSL3SocketServer.
    Private Function OpenSocketClientAsync(ByVal ip As String) As Boolean
        Try
            Dim endpoint = New IPEndPoint(IPAddress.Parse(ip), 4502)

            _client = New SocketClient(endpoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp)

            ' Register event
            AddHandler _client.ClientConnected, AddressOf _client_ClientConnected
            AddHandler _client.MessageReceived, AddressOf _client_MessageReceived
            AddHandler _client.MessageSended, AddressOf _client_MessageSended

            _client.ConnectAsync(endpoint)
            Return True
        Catch ex As Exception
            MessageBox.Show("Exception occured when Connecting Socket: " & ex.Message)
            CloseSocketClient()
            Return False
        End Try
    End Function

    Delegate Sub HandleSocketEvent(ByVal e As SocketMessageEventArgs)

    Private Sub _client_ClientConnected(ByVal sender As Object, ByVal e As SocketMessageEventArgs)
        Dim clientConnect As New HandleSocketEvent(AddressOf onClientConnect)
        Me.Dispatcher.BeginInvoke(clientConnect, New Object() {e})
    End Sub

    Private Sub onClientConnect(ByVal e As SocketMessageEventArgs)
        ' If connect successful, start receiving message
        If e.[Error] Is Nothing Then
            Try
                _client.StartReceiving()
            Catch ex As Exception
                MessageBox.Show("Exception occured when creating SocketClient:" & ex.Message)
                CloseSocketClient()
                Exit Sub
            End Try
            ' Update UI
            btnConnect.IsEnabled = False
            btnSend.IsEnabled = True
            tbSocketStatus.Text = "Connected"
        Else
            _client.Close()
            btnConnect.IsEnabled = True
            tbSocketStatus.Text = "Connect failed: " & e.[Error].Message
        End If
    End Sub

    ' Handle message received event
    Private Sub _client_MessageSended(ByVal sender As Object, ByVal e As SocketMessageEventArgs)
        Dim messageSended As New HandleSocketEvent(AddressOf onMessageSended)
        Me.Dispatcher.BeginInvoke(messageSended, New Object() {e})
    End Sub

    Private Sub onMessageSended(ByVal e As SocketMessageEventArgs)
        If e.[Error] Is Nothing Then
            tbSocketStatus.Text = "Sended"
        Else
            tbSocketStatus.Text = "Send failed: " & e.[Error].Message
            CloseSocketClient()
        End If
    End Sub

    ' Handle message sended event
    Private Sub _client_MessageReceived(ByVal sender As Object, ByVal e As SocketMessageEventArgs)
        Dim messageReceived As New HandleSocketEvent(AddressOf onMessageReceived)
        Me.Dispatcher.BeginInvoke(messageReceived, New Object() {e})
    End Sub

    Private Sub onMessageReceived(ByVal e As SocketMessageEventArgs)
        If e.[Error] Is Nothing Then
            tbSocketStatus.Text = "Received"
            lb1.Items.Insert(0, e.Data)
        Else
            tbSocketStatus.Text = "Receive failed: " & e.[Error].Message
            CloseSocketClient()
        End If
    End Sub
End Class