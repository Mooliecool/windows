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
Imports System.Security.Principal

Module NegotiateClient

    Sub Main()
        ' You should substitute in the SPN of the server you want to
        ' authenticate to using Kerberos.  Otherwise, NTLM authentication
        ' will be used.
        Connect("localhost", "howdy", "domain\\user")
    End Sub

    Private Sub Connect( _
        ByVal server As String, ByVal message As String, _
        ByVal servicePrincipalName As String)
        Dim negotiateStream As NegotiateStream = Nothing
        Try
            ' Create a TcpClient.
            ' Note, for this client to work you need to have a TcpServer 
            ' connected to the same address as specified by the server, 
            ' port combination.
            Dim port As Integer = 13000
            Dim client As TcpClient = New TcpClient(server, port)

            ' Translate the passed message into ASCII and store it as a 
            ' Byte array.
            Dim data() As Byte = System.Text.Encoding.ASCII.GetBytes(message)

            ' Get a client stream for reading and writing.
            ' Wrap it in a NegotiateStream.
            negotiateStream = New NegotiateStream(client.GetStream())

            ' This example uses the SPN which is required for Kerberos.  
            ' If you don't know your service principal name, you can do 
            ' NTLM authentication by commenting out the line below
            negotiateStream.AuthenticateAsClient( _
                CredentialCache.DefaultNetworkCredentials, _
                servicePrincipalName, _
                ProtectionLevel.EncryptAndSign, _
                TokenImpersonationLevel.Impersonation)
            ' And then uncomment this line
            ' authenticatedStream.AuthenticateAsClient();

            If negotiateStream.IsAuthenticated Then
                Console.WriteLine( _
                    "IsAuthenticated: {0}", _
                    negotiateStream.IsAuthenticated)
                Console.WriteLine( _
                    "IsMutuallyAuthenticated: {0}", _
                    negotiateStream.IsMutuallyAuthenticated)
                Console.WriteLine( _
                    "IsEncrypted: {0}", _
                    negotiateStream.IsEncrypted)
                Console.WriteLine( _
                    "IsSigned: {0}", _
                    negotiateStream.IsSigned)
                Console.WriteLine( _
                    "IsServer: {0}", _
                    negotiateStream.IsServer)
            End If

            ' Send the message to the connected TcpServer. 
            negotiateStream.Write(data, 0, data.Length)

            Console.WriteLine("Sent: {0}", message)

            ' Receive the TcpServer.response:
            ' Buffer to store the response bytes.
            ReDim data(256)

            ' String to store the response ASCII representation.
            Dim responseData As String = String.Empty

            ' Read the first batch of the TcpServer response bytes.
            Dim bytes As Integer = negotiateStream.Read(data, 0, data.Length)
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
            If negotiateStream IsNot Nothing Then
                negotiateStream.Close()
            End If
        End Try

        Console.WriteLine(Environment.NewLine & "Press Enter to continue...")
        Console.Read()
    End Sub

End Module
