'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Text
Imports System.IO
Imports System.Net
Imports System.Net.Sockets


Module IPv6Server

    Sub Main()
        Dim localEndPoint As New IPEndPoint(IPAddress.Any, 5150)
        Dim serverSocket As New Socket( _
            localEndPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp)
        Try
            serverSocket.Bind(localEndPoint)
            serverSocket.Listen(Integer.MaxValue)
            Console.WriteLine("Server started.")

            While True
                Try
                    Dim clientSocket As Socket = serverSocket.Accept()
                    Console.WriteLine( _
                        "Accepted connection from: {0}", _
                        clientSocket.RemoteEndPoint.ToString())

                    Dim reader As StreamReader = Nothing
                    Dim writer As StreamWriter = Nothing
                    Try
                        Dim networkStream As New NetworkStream(clientSocket)
                        reader = New StreamReader(networkStream)
                        Dim clientMessage As String = reader.ReadLine()
                        Console.WriteLine( _
                            "Server received message: {0}", clientMessage)

                        writer = New StreamWriter(networkStream)
                        Dim serverMessage As String = "Hello!"
                        writer.WriteLine(serverMessage)
                        writer.Flush()
                        Console.WriteLine( _
                            "Server sent message: {0}", serverMessage)
                    Catch ex As SocketException
                        Console.WriteLine( _
                            "Message exchange failed: {0}", ex.Message)
                    Finally
                        If Not (reader Is Nothing) Then
                            reader.Close()
                        End If
                        If Not (writer Is Nothing) Then
                            writer.Close()
                        End If
                    End Try
                Catch ex As SocketException
                    Console.WriteLine( _
                        "Server could not accept connection: {0}", ex.Message)
                End Try
            End While
        Catch ex As SocketException
            Console.WriteLine("Failed to start server: {0}", ex.Message)
        Finally
            If Not (serverSocket Is Nothing) Then
                serverSocket.Close()
            End If
        End Try
    End Sub 'Main
End Module