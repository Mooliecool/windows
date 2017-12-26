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

Imports System.Net
Imports System.Text

Module WebClientSample

    Sub Main(ByVal args() As String)
        Dim serverName As String

        ' This example assumes the site uses anonymous logon.
        Dim credentials As NetworkCredential = _
             New NetworkCredential("anonymous", "")

        If args.Length <> 1 OrElse _
            args(0).Equals("/?") OrElse _
            args(0).Equals("/help") Then

            PrintUsage()
            Exit Sub
        Else
            serverName = args(0)
        End If
        DemonstrateWebClient(serverName, credentials)
    End Sub

    Private Sub PrintUsage()
        Console.WriteLine("To run the sample:")
        Console.WriteLine("    WebClientSample.exe <FTP server name>")
        Console.WriteLine("To display usage:")
        Console.WriteLine("    WebClientSample.exe [/? | /help]")
    End Sub

    ' Read the name of the file which needs to be uploaded or downloaded.
    Private Function ReadFileName(ByVal operation As String) As String
        ' Operation will be download or upload.
        Console.Write( _
            "Enter the name of the file to " & _
            operation & _
            " relative to the logon directory: ")

        ' Returns the name of the file read.
        Return Console.ReadLine()
    End Function

    ' Read in some text data to upload.
    Private Function ReadData() As String
        Console.WriteLine("Enter some text data to upload:")
        Return Console.ReadLine()
    End Function

    Private Sub DemonstrateWebClient(ByVal serverName As String, ByVal credentials As ICredentials)
        Dim serverAddress As String = "ftp://" + serverName + "/"

        ' Create a new WebClient object and set the credentials.
        Dim webClient As WebClient = New WebClient()
        webClient.Credentials = credentials

        ' Download file.
        Try
            Dim fileName As String = ReadFileName("download")
            Dim downloadAddress As String = serverAddress + fileName
            Console.WriteLine("Downloading file " + downloadAddress)

            ' The string passed in should include the URI path that has 
            ' the file name to be downloaded.
            webClient.DownloadFile(downloadAddress, fileName)
        Catch ex As WebException
            Console.WriteLine(ex.Message)
        End Try

        ' Upload file.
        Try
            Dim fileName As String = ReadFileName("upload")
            Dim uploadAddress As String = serverAddress + fileName
            Console.WriteLine("Uploading file " + uploadAddress)
            webClient.UploadFile(uploadAddress, fileName)
        Catch ex As WebException
            Console.WriteLine(ex.Message)
        End Try

        ' Upload file as data.
        Try
            Dim fileName As String = ReadFileName("upload to")
            Dim data As String = ReadData()
            Dim uploadAddress As String = serverAddress + fileName

            ' Apply ASCII Encoding to obtain the string as a byte array.
            Dim byteArray() As Byte = Encoding.UTF8.GetBytes(data)
            Console.WriteLine("Uploading data to " + uploadAddress)

            ' Upload the data, and using the specified verb. 
            ' The STOR verb is an FTP command creates a file on the 
            ' server using the specified data.
            Dim responseArray() As Byte = webClient.UploadData( _
                uploadAddress, WebRequestMethods.Ftp.UploadFile, byteArray)

            Dim response As String
            If responseArray.Length = 0 Then
                response = "<nothing>"
            Else
                ' Decode and display the response.
                response = Encoding.UTF8.GetString(responseArray)
            End If

            Console.WriteLine()
            Console.WriteLine("Response received was {0}", response)

        Catch ex As WebException
            Console.WriteLine(ex.Message)
        End Try

        webClient.Dispose()
    End Sub
End Module
