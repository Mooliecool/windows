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

Imports System.IO
Imports System.Net
Imports System.Net.Sockets
Imports System.Net.Security
Imports System.Security.Authentication
Imports System.Security.Cryptography.X509Certificates

Module SslClient

    Sub Main()
        Connect("localhost", "howdy")
    End Sub

    Private Sub Connect(ByVal server As String, ByVal message As String)
        Dim sslStream As SslStream = Nothing
        Try
            ' Create a TcpClient.
            ' Note, for this client to work you need to have a TcpServer 
            ' connected to the same address as specified by the server, 
            ' port combination.
            Dim port As Integer = 13000
            Dim client As New TcpClient(server, port)

            ' Translate the passed message into ASCII and store it as a 
            ' Byte array.
            Dim data() As Byte = System.Text.Encoding.ASCII.GetBytes(message)

            Dim callback As RemoteCertificateValidationCallback = _
                AddressOf OnCertificateValidation

            ' Get a client stream for reading and writing.
            ' Wrap it in an SslStream. 
            sslStream = New SslStream(client.GetStream(), False, callback)
            sslStream.AuthenticateAsClient("localhost")

            If sslStream.IsAuthenticated Then
                Console.WriteLine( _
                    "IsAuthenticated: {0}", _
                    sslStream.IsAuthenticated)
                Console.WriteLine( _
                    "IsMutuallyAuthenticated: {0}", _
                    sslStream.IsMutuallyAuthenticated)
                Console.WriteLine( _
                    "IsEncrypted: {0}", _
                    sslStream.IsEncrypted)
                Console.WriteLine( _
                    "IsSigned: {0}", _
                    sslStream.IsSigned)
                Console.WriteLine( _
                    "IsServer: {0}", _
                    sslStream.IsServer)
            End If

            ' Send the message to the connected TcpServer. 
            sslStream.Write(data, 0, data.Length)

            Console.WriteLine("Sent: {0}", message)

            ' Receive the TcpServer.response.
            ' Buffer to store the response bytes.
            ReDim data(256)

            ' String to store the response ASCII representation.
            Dim responseData As String = String.Empty

            ' Read the first batch of the TcpServer response bytes.
            Dim bytes As Integer = sslStream.Read(data, 0, data.Length)
            responseData = _
                System.Text.Encoding.ASCII.GetString(data, 0, bytes)
            Console.WriteLine("Received: {0}", responseData)
        Catch ex As AuthenticationException
            Console.WriteLine(ex.Message)
        Catch ex As SocketException
            Console.WriteLine(ex.Message)
        Catch ex As IOException
            Console.WriteLine(ex.Message)
        Finally
            If sslStream IsNot Nothing Then
                sslStream.Close()
            End If
        End Try

        Console.WriteLine(Environment.NewLine & "Press Enter to continue...")
        Console.Read()
    End Sub

    Function OnCertificateValidation( _
      ByVal sender As Object, _
        ByVal certificate As X509Certificate, _
        ByVal chain As X509Chain, _
        ByVal sslPolicyErrors As SslPolicyErrors) _
        As Boolean

        ' This sample returns true always so that you can see how
        ' it works.  You should change logic in the validator to only
        ' return true if you've inspected the certificate and approve it.
        Return True
    End Function

End Module
