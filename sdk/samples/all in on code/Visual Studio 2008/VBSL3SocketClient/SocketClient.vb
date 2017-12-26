'****************************** Module Header ******************************'
' Module Name:              SocketClient.vb
' Project:                  VBSL3SocketClient
' Copyright (c) Microsoft Corporation.
' 
' Socket Server application code file, can serve Silverlight and normal socket
' Client.
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
Imports System.Text
Imports System.Net.Sockets
Imports System.Net

Namespace VBSL3SocketClient
    Public Class SocketMessageEventArgs
        Inherits EventArgs
        Private _Error As Exception
        Public Property [Error]() As Exception
            Get
                Return _Error
            End Get
            Set(ByVal value As Exception)
                _Error = value
            End Set
        End Property
        Private _Data As String
        Public Property Data() As String
            Get
                Return _Data
            End Get
            Set(ByVal value As String)
                _Data = value
            End Set
        End Property
    End Class

    Public Class SocketClient
        ' Define 3 events for async operation:
        ' Open,Receive and Send
        Public Event MessageReceived As EventHandler(Of SocketMessageEventArgs)
        Public Event MessageSended As EventHandler(Of SocketMessageEventArgs)
        Public Event ClientConnected As EventHandler(Of SocketMessageEventArgs)

        ' Set receive buffer size
        Shared ReadOnly BUFFER_SIZE As Integer = 65536

        ' Define the End-of-message char, which is used for separating
        ' byte array into string messages
        Shared ReadOnly EOM_MARKER As Char = ChrW(127)

        ' The encapsulated socket
        Private _InnerSocket As Socket
        Public Property InnerSocket() As Socket
            Get
                Return _InnerSocket
            End Get
            Private Set(ByVal value As Socket)
                _InnerSocket = value
            End Set
        End Property


        Public Sub New(ByVal socket As Socket)
            If socket Is Nothing Then
                Throw New Exception("Socket cannot be null")
            End If
            InnerSocket = socket

            ' Initialize string decoder
            encoding = New UTF8Encoding(False, True)
        End Sub
        Public Sub New(ByVal addfamily As AddressFamily, ByVal socktype As SocketType, ByVal protype As ProtocolType)
            InnerSocket = New Socket(addfamily, socktype, protype)
            encoding = New UTF8Encoding(False, True)
        End Sub

#Region "Socket async connect"

        ' Get socket connect status
        Public ReadOnly Property Connected() As Boolean
            Get
                Return InnerSocket.Connected
            End Get
        End Property

        ' Close socket
        Public Sub Close()
            InnerSocket.Close()
        End Sub

        ''' <summary>
        ''' Connect socket to endpoint asynchronously.
        ''' Possible exception:
        ''' ArgumentException
        ''' ArgumentNullException
        ''' InvalidOperationException
        ''' SocketException
        ''' NotSupportedException
        ''' ObjectDisposedException
        ''' SecurityException
        ''' Details at: http://msdn.microsoft.com/en-us/library/bb538102.aspx
        ''' </summary>
        ''' <param name="ep">remote endpoint</param>
        Public Sub ConnectAsync(ByVal ep As EndPoint)
            If InnerSocket.Connected Then
                Exit Sub
            End If

            ' Initialize socketAsyncEventArgs
            ' Set remote connect endpoint
            Dim connectEventArgs = New SocketAsyncEventArgs()
            connectEventArgs.RemoteEndPoint = ep
            AddHandler connectEventArgs.Completed, AddressOf connectEventArgs_Completed

            ' Call ConnectAsync method, if method returned false
            ' it means the result has returned synchronously
            If Not InnerSocket.ConnectAsync(connectEventArgs) Then
                ' Call method to handle connect result
                ProcessConnect(connectEventArgs)
            End If
        End Sub

        ' When connectAsync completed, call method to handle connect result
        Private Sub connectEventArgs_Completed(ByVal sender As Object, ByVal e As SocketAsyncEventArgs)
            ProcessConnect(e)
        End Sub

        ' Invoke ClientConnected event to return result 
        Private Sub ProcessConnect(ByVal e As SocketAsyncEventArgs)
            If e.SocketError = SocketError.Success Then
                OnClientConnected(Nothing)
            Else
                OnClientConnected(New SocketException(CInt(e.SocketError)))
            End If
        End Sub

        Private Sub OnClientConnected(ByVal [error] As Exception)
            Dim sockargs As New SocketMessageEventArgs()
            sockargs.Error = [error]
            RaiseEvent ClientConnected(Me, sockargs)
        End Sub
#End Region

#Region "Socket async Send"

        ''' <summary>
        ''' Use Socket to send string message.
        ''' Possible exception:
        ''' FormatException
        ''' ArgumentException
        ''' InvalidOperationException
        ''' NotSupportedException
        ''' ObjectDisposedException
        ''' SocketException
        ''' </summary>
        ''' <param name="data">message to be sent</param>
        Public Sub SendAsync(ByVal data As String)

            ' If message data contains EOM_MARKER char,
            ' throw exception
            If data.Contains(EOM_MARKER) Then
                Throw New Exception("Unallowed chars existed in message")
            End If

            ' Add End-of-message char at message end.
            data += EOM_MARKER

            ' Get UTF8 encoded byte array
            Dim bytesdata = encoding.GetBytes(data)

            ' Initialize SendEventArgs
            Dim sendEventArgs = New SocketAsyncEventArgs()
            sendEventArgs.SetBuffer(bytesdata, 0, bytesdata.Length)
            AddHandler sendEventArgs.Completed, AddressOf sendEventArgs_Completed

            ' Call SendAsync method, if method returned false
            ' it means the result has returned synchronously
            If Not InnerSocket.SendAsync(sendEventArgs) Then
                ProcessSend(sendEventArgs)
            End If
        End Sub

        ' When sendAsync completed, call method to handle send result
        Private Sub sendEventArgs_Completed(ByVal sender As Object, ByVal e As SocketAsyncEventArgs)
            ProcessSend(e)
        End Sub

        ' Invoke MessageSended event to return result 
        Private Sub ProcessSend(ByVal e As SocketAsyncEventArgs)
            If e.SocketError = SocketError.Success Then
                OnMessageSended(Nothing)
            Else
                OnMessageSended(New SocketException(CInt(e.SocketError)))
            End If
        End Sub

        Private Sub OnMessageSended(ByVal [error] As Exception)
            Dim sockargs As New SocketMessageEventArgs()
            sockargs.Error = [error]
            RaiseEvent MessageSended(Me, sockargs)
        End Sub
#End Region

#Region "Socket async Receive"

        ' Define flag to indicate receive status
        Private _isReceiving As Boolean

        ''' <summary>
        ''' Start receiving bytes from socket and invoke
        ''' MessageReceived event when each message received.
        ''' Possible exception:
        ''' ArgumentException
        ''' InvalidOperationException
        ''' NotSupportedException
        ''' ObjectDisposedException
        ''' SocketException
        ''' Details at http://msdn.microsoft.com/en-us/library/system.net.sockets.socket.receiveasync.aspx
        ''' </summary>
        Public Sub StartReceiving()

            ' Check if socket is started receiving already
            If Not _isReceiving Then
                _isReceiving = True
            Else
                Exit Sub
            End If

            Try
                ' Initialize receiving buffer
                Dim buffer = New Byte(BUFFER_SIZE) {}

                ' Initialize receive event args
                Dim receiveEventArgs = New SocketAsyncEventArgs()
                receiveEventArgs.SetBuffer(buffer, 0, BUFFER_SIZE)
                AddHandler receiveEventArgs.Completed, AddressOf receiveEventArgs_Completed

                ' Call ReceiveAsync method, if method returned false
                ' it means the result has returned synchronously
                If Not InnerSocket.ReceiveAsync(receiveEventArgs) Then
                    ProcessReceive(receiveEventArgs)
                End If
            Catch ex As Exception
                StopReceiving()
                Throw ex
            End Try
        End Sub

        ' Stop receiving bytes from socket
        Public Sub StopReceiving()
            _isReceiving = False
        End Sub

        Private Sub receiveEventArgs_Completed(ByVal sender As Object, ByVal e As SocketAsyncEventArgs)
            ProcessReceive(e)
        End Sub

        ' Process receiveAsync complete event
        Private receivemessage As String = ""
        Private encoding As Encoding
        Private taillength As Integer
        Private Sub ProcessReceive(ByVal e As SocketAsyncEventArgs)
            ' When got Error, invoke MessageReceived event
            ' to pass the error info to user
            If e.SocketError <> SocketError.Success Then
                StopReceiving()
                OnMessageReceived(Nothing, New SocketException(CInt(e.SocketError)))
                Exit Sub
            End If

            Try
                '#Region "String Decoding"
                ' Decoding bytes to string.
                ' Note that UTF-8 is variable-length encode, we need check the byte
                ' array tail in case of separating one character into two.
                Dim receivestr As String = ""
                ' Try decode string
                Try
                    receivestr = encoding.GetString(e.Buffer, 0, taillength + e.BytesTransferred)
                    ' If decode successful, reset tail length
                    taillength = 0
                Catch ex As DecoderFallbackException
                    ' If got decode exception, remove the array tail and re decode
                    Try
                        receivestr = encoding.GetString(e.Buffer, 0, taillength + e.BytesTransferred - ex.BytesUnknown.Length)
                        ' reset tail length
                        taillength = ex.BytesUnknown.Length
                        ex.BytesUnknown.CopyTo(e.Buffer, 0)
                    Catch ex2 As DecoderFallbackException
                        ' If still got decode exception, stop receiving.
                        Throw New Exception("Message decode failed.", ex2)

                        '#End Region
                    End Try
                End Try
                ' Check if message ended
                Dim eompos As Integer = receivestr.IndexOf(EOM_MARKER)
                While eompos <> -1
                    ' Compose a complete message
                    receivemessage += receivestr.Substring(0, eompos)

                    ' Notify message received
                    OnMessageReceived(receivemessage, Nothing)

                    ' Get the remaining string
                    receivemessage = ""
                    receivestr = receivestr.Substring(eompos + 1, receivestr.Length - eompos - 1)

                    ' Check if it still has EOM in string
                    eompos = receivestr.IndexOf(EOM_MARKER)
                End While
                receivemessage += receivestr

                ' Stop receiving.
                If Not _isReceiving Then
                    Exit Sub
                End If

                ' Reset buffer offset
                e.SetBuffer(taillength, BUFFER_SIZE - taillength)

                ' Keep receiving
                If Not InnerSocket.ReceiveAsync(e) Then
                    ProcessReceive(e)
                End If
            Catch ex As Exception
                ' Return error through MessageReceived event
                OnMessageReceived(Nothing, ex)
                StopReceiving()
            End Try
        End Sub

        Private Sub OnMessageReceived(ByVal data As String, ByVal [error] As Exception)
            Dim sockargs As New SocketMessageEventArgs()
            sockargs.Data = data
            sockargs.Error = [error]
            RaiseEvent MessageReceived(Me, sockargs)
        End Sub
#End Region
    End Class
End Namespace