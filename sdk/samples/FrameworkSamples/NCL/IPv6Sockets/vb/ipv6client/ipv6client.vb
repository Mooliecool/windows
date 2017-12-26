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
Imports System.Collections.Generic
Imports System.Text
Imports System.IO
Imports System.Net
Imports System.Net.Sockets

Module IPv6Client

    Sub Main(ByVal args() As String)
        If args.Length < 1 Then
            DisplayUsage()
            Return
        End If

        Dim serverDnsName As String = args(0)
        Try
            Dim resolvedServer As IPHostEntry = Dns.GetHostEntry(serverDnsName)
            Dim i As Integer
            For i = 0 To resolvedServer.AddressList.Length
                Dim address As IPAddress = resolvedServer.AddressList(i)
                Dim serverEndPoint As New IPEndPoint(address, 5150)
                Dim tcpSocket As New Socket( _
                    address.AddressFamily, SocketType.Stream, ProtocolType.Tcp)
                Try
                    tcpSocket.Connect(serverEndPoint)
                    Dim writer As StreamWriter = Nothing
                    Dim reader As StreamReader = Nothing
                    Try
                        Dim networkStream As New NetworkStream(tcpSocket)
                        writer = New StreamWriter(networkStream)
                        Dim clientMessage As String = "Hi there!"
                        writer.WriteLine(clientMessage)
                        writer.Flush()
                        Console.WriteLine( _
                            "Client sent message: {0}", clientMessage)

                        reader = New StreamReader(networkStream)
                        Dim serverMessage As String = reader.ReadLine()
                        Console.WriteLine( _
                            "Client received message: {0}", serverMessage)
                    Catch ex As SocketException
                        Console.WriteLine( _
                            "Message exchange failed: {0}", ex.Message)
                    Catch ex As IOException
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
                    Exit For
                Catch
                    If Not (tcpSocket Is Nothing) Then
                        tcpSocket.Close()
                    End If
                    If i = resolvedServer.AddressList.Length - 1 Then
                        Console.WriteLine("Failed to connect to the server.")
                    End If
                End Try
            Next i

        Catch ex As SocketException
            Console.WriteLine( _
                "Could not resolve server DNS name: {0}", ex.Message)
        End Try

    End Sub 'Main


    Private Sub DisplayUsage()
        Console.WriteLine("IPv6Client server_name")
    End Sub 'DisplayUsage

End Module 'IPv6Client