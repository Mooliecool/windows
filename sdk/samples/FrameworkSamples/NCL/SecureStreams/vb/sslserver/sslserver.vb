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
Imports System.Security.Cryptography
Imports System.Security.Cryptography.X509Certificates

Module SslServer

    Sub Main()
        Dim sslStream As SslStream = Nothing
        Try
            ' Set the TcpListener on port 13000.
            Dim port As Integer = 13000
            Dim localAddr As IPAddress = IPAddress.Loopback

            ' TcpListener server = new TcpListener(port);
            Dim server As New TcpListener(localAddr, port)

            ' Start listening for client requests.
            server.Start()

            ' Buffer for reading data
            Dim bytes(256) As Byte
            Dim data As String = Nothing

            ' Enter the listening loop.
            While True
                Console.Write("Waiting for a connection... ")

                ' Perform a blocking call to accept requests.
                ' You could also user server.AcceptSocket() here.
                Dim client As TcpClient = server.AcceptTcpClient()
                Console.WriteLine("Connected!")

                data = Nothing

                ' Get a stream object for reading and writing
                sslStream = New SslStream(client.GetStream())

                ' The exportedcertificate.cer file would contain a 
                ' certificate with public key that came from a certificate
                ' in the local machine store.
                Dim certificate As X509Certificate = _
                       X509Certificate.CreateFromCertFile( _
                       "exportedcertificate.cer")
                sslStream.AuthenticateAsServer(certificate)

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

                ' Loop to receive all the data sent by the client.
                Dim bytesRead As Integer = sslStream.Read(bytes, 0, bytes.Length)
                While bytesRead <> 0
                    ' Translate data bytes to a ASCII string.
                    data = System.Text.Encoding.ASCII.GetString( _
                        bytes, 0, bytesRead)
                    Console.WriteLine("Received: {0}", data)

                    ' Process the data sent by the client.
                    data = data.ToUpper( _
                        System.Globalization.CultureInfo.CurrentCulture)

                    Dim msg() As Byte = _
                        System.Text.Encoding.ASCII.GetBytes(data)

                    ' Send back a response.
                    sslStream.Write(msg, 0, msg.Length)
                    Console.WriteLine("Sent: {0}", data)

                    bytesRead = sslStream.Read(bytes, 0, bytes.Length)
                End While
            End While
        Catch ex As CryptographicException
            Console.WriteLine(ex.Message)
            Console.WriteLine( _
                "An error has occurred while using or authenticating " & _
                "this connection.  Check to make sure you've exported " & _
                "your certificate and that it is correctly referenced in " & _
                "CreateFromCertFile above.")
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
        Console.WriteLine(Environment.NewLine & "Hit enter to continue...")
        Console.Read()
    End Sub
End Module
