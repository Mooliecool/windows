'************************** Module Header ******************************'
' Module Name:  DownloaderHelper.vb
' Project:      VBWebDownloader
' Copyright (c) Microsoft Corporation.
' 
' This class supplies the methods to 
' 1. Initialize a HttpWebRequest object. 
' 2. Check the url and initialize some properties of a downloader.
' 3. Check whether the destination file exists. If not, create a file with 
'    the same size as the file to be downloaded.
'  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Net
Imports System.Text.RegularExpressions
Imports System.IO


Public NotInheritable Class DownloaderHelper

    Private Sub New()
    End Sub
    Public Shared Function InitializeHttpWebRequest(ByVal downloader As IDownloader) As HttpWebRequest
        Dim request = CType(WebRequest.Create(downloader.Url), HttpWebRequest)

        If downloader.Credentials IsNot Nothing Then
            request.Credentials = downloader.Credentials
        Else
            request.Credentials = CredentialCache.DefaultCredentials
        End If

        If downloader.Proxy IsNot Nothing Then
            request.Proxy = downloader.Proxy
        Else
            request.Proxy = WebRequest.DefaultWebProxy
        End If

        Return request
    End Function

    ''' <summary>
    ''' Check the URL to download, including whether it supports Range, 
    ''' </summary>
    ''' <param name="downloader"></param>
    ''' <returns></returns>
    Public Shared Function CheckUrl(ByVal downloader As IDownloader) As String
        Dim fileName As String = String.Empty

        ' Check the file information on the remote server.
        Dim webRequest = InitializeHttpWebRequest(downloader)

        Using response = webRequest.GetResponse()
            For Each header In response.Headers.AllKeys
                If header.Equals("Accept-Ranges", StringComparison.OrdinalIgnoreCase) Then
                    downloader.IsRangeSupported = True
                End If

                If header.Equals("Content-Disposition", StringComparison.OrdinalIgnoreCase) Then
                    Dim contentDisposition As String = response.Headers(header)

                    Dim pattern As String = ".[^;]*;\s+filename=""(?<file>.*)"""
                    Dim r As New Regex(pattern)
                    Dim m As Match = r.Match(contentDisposition)
                    If m.Success Then
                        fileName = m.Groups("file").Value
                    End If
                End If
            Next header

            downloader.TotalSize = response.ContentLength

            If downloader.TotalSize <= 0 Then
                Throw New ApplicationException("The file to download does not exist!")
            End If

            If Not downloader.IsRangeSupported Then
                downloader.StartPoint = 0
                downloader.EndPoint = Integer.MaxValue
            End If
        End Using

        If downloader.IsRangeSupported AndAlso (downloader.StartPoint <> 0 _
                                                OrElse downloader.EndPoint <> Long.MaxValue) Then
            webRequest = InitializeHttpWebRequest(downloader)

            If downloader.EndPoint <> Integer.MaxValue Then
                webRequest.AddRange(downloader.StartPoint, downloader.EndPoint)
            Else
                webRequest.AddRange(downloader.StartPoint)
            End If
            Using response = webRequest.GetResponse()
                downloader.TotalSize = response.ContentLength
            End Using
        End If

        Return fileName
    End Function


    ''' <summary>
    ''' Check whether the destination file exists. If not, create a file with the same
    ''' size as the file to be downloaded.
    ''' </summary>
    Public Shared Sub CheckFileOrCreateFile(ByVal downloader As IDownloader, ByVal fileLocker As Object)
        ' Lock other threads or processes to prevent from creating the file.
        SyncLock fileLocker
            Dim fileToDownload As New FileInfo(downloader.DownloadPath)
            If fileToDownload.Exists Then

                ' The destination file should have the same size as the file to be downloaded.
                If fileToDownload.Length <> downloader.TotalSize Then
                    Throw New ApplicationException(
                        "The download path already has a file which does not match" _
                        & " the file to download. ")
                End If

                ' Create a file.
            Else
                If downloader.TotalSize = 0 Then
                    Throw New ApplicationException("The file to download does not exist!")
                End If

                Using fileStream As FileStream = File.Create(downloader.DownloadPath)
                    Dim createdSize As Long = 0
                    Dim buffer(4095) As Byte
                    Do While createdSize < downloader.TotalSize
                        Dim bufferSize As Integer = If((downloader.TotalSize - createdSize) < 4096,
                                                       CInt(Fix(downloader.TotalSize - createdSize)), 4096)
                        fileStream.Write(buffer, 0, bufferSize)
                        createdSize += bufferSize
                    Loop
                End Using
            End If
        End SyncLock
    End Sub
End Class

