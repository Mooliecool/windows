'****************************** Module Header ******************************'
' Module Name:              SocketListener.vb
' Project:                  VBSL3SocketServer
' Copyright (c) Microsoft Corporation.
' 
' Implement SocketListener class, which encapsulated the socket, and provide a 
' easy way to listen and return connected sockets.
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
Imports System.Net
Imports System.Threading
Imports System.Net.Sockets

Namespace VBSL3SocketServer
    Public Delegate Sub GetSocketCallBack(ByVal sock As Socket)
    Public Class SocketListener

        Public Sub ListenAsync(ByVal port As Integer, ByVal callback As GetSocketCallBack)
            ' Run on another thread.
            Dim thread As New Thread(AddressOf Listen)
            thread.Start(New Object() {port, callback})
        End Sub

        Public Sub Listen(ByVal param As Object)
            Dim params As Object() = param
            Dim port As Integer = param(0)
            Dim callback As GetSocketCallBack = param(1)
            ' As a matter of convenience, we use 127.0.0.1 as server socket
            ' address. This address is can only be accessed from local.
            ' To let server accessible from network, try use machine's network
            ' address.

            ' 127.0.0.1 address
            Dim localEP As New IPEndPoint(&H100007F, port)

            ' network ip address.
            'IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            'IPEndPoint localEP = new IPEndPoint(ipHostInfo.AddressList[0], port);

            Dim listener As New Socket(localEP.Address.AddressFamily, SocketType.Stream, ProtocolType.Tcp)

            Try
                listener.Bind(localEP)
                Console.WriteLine("Socket Listener opened: " & localEP.ToString())
                While True
                    listener.Listen(10)
                    Dim socket As Socket = listener.Accept()

                    ' Return connected socket through callback function.
                    If callback IsNot Nothing Then
                        callback(socket)
                    Else
                        socket.Close()
                        socket = Nothing
                    End If
                End While
            Catch ex As Exception
                Console.Write("Exception occured:" & ex.Message)
            End Try
            Console.WriteLine("Listener closed: " & localEP.ToString())
            listener.Close()
        End Sub
    End Class
End Namespace