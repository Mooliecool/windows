'****************************** Module Header ******************************'
' Module Name:              Module1.vb
' Project:                  VBSL3SocketServer
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
Imports System.Threading
Imports System.IO

Namespace VBSL3SocketServer
    Class Program
        Shared policybytes As Byte()
        Public Shared Sub Main(ByVal args As String())
            ' Read policy.xml file to byte array.
            Dim filestream = New FileStream("policy.xml", FileMode.Open, FileAccess.Read)
            policybytes = New Byte(filestream.Length - 1) {}
            filestream.Read(policybytes, 0, CInt(filestream.Length))
            filestream.Close()

            ' Initialize policy socket listener
            Dim socketp = New SocketListener()
            socketp.ListenAsync(943, AddressOf socketp_SocketConnected)

            ' Initialize socket listener
            Dim socketp2 = New SocketListener()
            socketp2.ListenAsync(4502, AddressOf socketp2_SocketConnected)

            Console.Read()
        End Sub

        ' Client connected
        Private Shared Sub socketp2_SocketConnected(ByVal sock As Socket)
            ' Create new thread to handle client communication
            ' Initialize SocketClient
            ' Ready to receive 

            Dim thread As New Thread(AddressOf CreateSocketClient)
            thread.Start(sock)

        End Sub

        Private Shared Sub CreateSocketClient(ByVal param As Socket)
            Dim client = New SocketClient(param)
            Try
                AddHandler client.MessageReceived, AddressOf client_MessageReceived
                AddHandler client.MessageSended, AddressOf client_MessageSended
                client.StartReceiving()
                Console.WriteLine("Client connected.")
            Catch ex As Exception
                Console.WriteLine("Exception occured when start receiving message:" & vbLf & ex.Message)
                client.Close()
            End Try
        End Sub

        ' Handle message sended event
        Private Shared Sub client_MessageSended(ByVal sender As Object, ByVal e As SocketMessageEventArgs)
            If e.[Error] IsNot Nothing Then
                Console.WriteLine("Message send failed: " & e.[Error].Message)
                DirectCast(sender, SocketClient).Close()
                Console.WriteLine("Client disconnected.")
            Else
                Console.WriteLine("Message send successful")
            End If
        End Sub

        ' Handle message received event
        Private Shared Sub client_MessageReceived(ByVal sender As Object, ByVal e As SocketMessageEventArgs)
            If e.[Error] IsNot Nothing Then
                Console.WriteLine("Message receive failed: " & e.[Error].Message)
                DirectCast(sender, SocketClient).Close()
                Console.WriteLine("Client disconnected.")
            Else
                ' Idle 1 second and send message back
                Console.WriteLine("Message Received: " & e.Data)
                Thread.Sleep(1000)
                SendMessage(TryCast(sender, SocketClient), "Handled: " & e.Data)
            End If
        End Sub

        ' User SocketClient to send message
        Private Shared Sub SendMessage(ByVal client As SocketClient, ByVal data As String)
            Try
                client.SendAsync(data)
            Catch ex As Exception
                Console.WriteLine("Exception occured when sending message:" & vbLf & ex.Message)
                client.Close()
                Console.WriteLine("Client disconnected.")
            End Try
        End Sub

        ' Before Silverlight socket client connected to socket server,
        ' it will connect to server 943 port to request policy.
        Shared ReadOnly POLICY_REQUEST As String = "<policy-file-request/>"
        Private Shared Sub socketp_SocketConnected(ByVal sock As Socket)
            ' Run at other thread.

            ' Check if client request server policy
            ' Send policy
            Dim thread As New Thread(AddressOf CreatePolicySocket)
            thread.Start(sock)
        End Sub
        Private Shared Sub CreatePolicySocket(ByVal sock As Socket)
            Try
                Console.WriteLine("Policy client connected.")
                Dim receivebuffer As Byte() = New Byte(999) {}
                Dim receivedcount = sock.Receive(receivebuffer)
                Dim requeststr As String = Encoding.UTF8.GetString(receivebuffer, 0, receivedcount)
                If requeststr = POLICY_REQUEST Then
                    sock.Send(policybytes, 0, policybytes.Length, SocketFlags.None)
                End If
            Catch
                Console.WriteLine("Policy socket client get an error.")
            Finally
                sock.Close()
                Console.WriteLine("Policy client disconnected.")
            End Try
        End Sub
    End Class
End Namespace