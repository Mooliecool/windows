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
Imports System.IO

Module FtpSample

    Sub Main(ByVal args() As String)
        If args.Length = 0 OrElse args(0).Equals("/?") Then
            DisplayUsage()
        ElseIf args.Length = 1 Then
            Download(args(0))
        ElseIf args.Length = 2 Then
            If args(0).Equals("/list") Then
                List(args(1))
            Else
                Upload(args(0), args(1))
            End If
        Else
            Console.WriteLine("Unrecognized argument.")
        End If
    End Sub

    Private Sub DisplayUsage()
        Console.WriteLine("USAGE:")
        Console.WriteLine("    FtpSample [/? | <FTP download URL> | <local file>")
        Console.WriteLine("               <FTP upload URL> | /list <FTP list URL>]")
        Console.WriteLine()
        Console.WriteLine("where")
        Console.WriteLine("    FTP download URL   URL of a file to download from an FTP server.")
        Console.WriteLine("    FTP upload URL     Location on a FTP server to upload a file to.")
        Console.WriteLine("    FTP list URL       Location on a FTP server to list the contents of.")
        Console.WriteLine("    local file         A local file to upload to an FTP server.")
        Console.WriteLine()
        Console.WriteLine("    Options:")
        Console.WriteLine("        /?             Display this help message.")
        Console.WriteLine("        /list          Specifies the list command.")
        Console.WriteLine()
        Console.WriteLine("EXAMPLES:")
        Console.WriteLine("    Download a file    FtpSample ftp://myserver/download.txt")
        Console.WriteLine("    Upload a file      FtpSample upload.txt ftp://myserver/upload.txt")
    End Sub

    Private Sub Download(ByVal downloadUrl As String)
        Dim responseStream As Stream = Nothing
        Dim fileStream As FileStream = Nothing
        Dim reader As StreamReader = Nothing
        Try
            Dim downloadRequest As FtpWebRequest = _
                WebRequest.Create(downloadUrl)
            Dim downloadResponse As FtpWebResponse = _
                downloadRequest.GetResponse()
            responseStream = downloadResponse.GetResponseStream()

            Dim fileName As String = _
                Path.GetFileName(downloadRequest.RequestUri.AbsolutePath)

            If fileName.Length = 0 Then
                reader = New StreamReader(responseStream)
                Console.WriteLine(reader.ReadToEnd())
            Else
                fileStream = File.Create(fileName)
                Dim buffer(1024) As Byte
                Dim bytesRead As Integer
                While True
                    bytesRead = responseStream.Read(buffer, 0, buffer.Length)
                    If bytesRead = 0 Then
                        Exit While
                    End If
                    fileStream.Write(buffer, 0, bytesRead)
                End While
            End If
            Console.WriteLine("Download complete.")
        Catch ex As UriFormatException
            Console.WriteLine(ex.Message)
        Catch ex As WebException
            Console.WriteLine(ex.Message)
        Catch ex As IOException
            Console.WriteLine(ex.Message)
        Finally
            If reader IsNot Nothing Then
                reader.Close()
            ElseIf responseStream IsNot Nothing Then
                responseStream.Close()
            End If
            If fileStream IsNot Nothing Then
                fileStream.Close()
            End If
        End Try
    End Sub

    Private Sub Upload(ByVal fileName As String, ByVal uploadUrl As String)
        Dim requestStream As Stream = Nothing
        Dim fileStream As FileStream = Nothing
        Dim uploadResponse As FtpWebResponse = Nothing
        Try
            Dim uploadRequest As FtpWebRequest = WebRequest.Create(uploadUrl)
            uploadRequest.Method = WebRequestMethods.Ftp.UploadFile

            ' UploadFile is not supported through an Http proxy
            ' so we disable the proxy for this request.
            uploadRequest.Proxy = Nothing

            requestStream = uploadRequest.GetRequestStream()
            fileStream = File.Open(fileName, FileMode.Open)

            Dim buffer(1024) As Byte
            Dim bytesRead As Integer
            While True
                bytesRead = fileStream.Read(buffer, 0, buffer.Length)
                If bytesRead = 0 Then
                    Exit While
                End If
                requestStream.Write(buffer, 0, bytesRead)
            End While

            ' The request stream must be closed before getting the response.
            requestStream.Close()

            uploadResponse = uploadRequest.GetResponse()
            Console.WriteLine("Upload complete.")
        Catch ex As UriFormatException
            Console.WriteLine(ex.Message)
        Catch ex As IOException
            Console.WriteLine(ex.Message)
        Catch ex As WebException
            Console.WriteLine(ex.Message)
        Finally
            If uploadResponse IsNot Nothing Then
                uploadResponse.Close()
            End If
            If fileStream IsNot Nothing Then
                fileStream.Close()
            End If
            If requestStream IsNot Nothing Then
                requestStream.Close()
            End If
        End Try
    End Sub

    Private Sub List(ByVal listUrl As String)
        Dim reader As StreamReader = Nothing
        Try
            Dim listRequest As FtpWebRequest = WebRequest.Create(listUrl)
            listRequest.Method = WebRequestMethods.Ftp.ListDirectoryDetails
            Dim listResponse As FtpWebResponse = listRequest.GetResponse()
            reader = New StreamReader(listResponse.GetResponseStream())
            Console.WriteLine(reader.ReadToEnd())
            Console.WriteLine("List complete.")
        Catch ex As UriFormatException
            Console.WriteLine(ex.Message)
        Catch ex As WebException
            Console.WriteLine(ex.Message)
        Finally
            If reader IsNot Nothing Then
                reader.Close()
            End If
        End Try
    End Sub

End Module
