'************************** Module Header ******************************'
' Module Name:  IDownloader.vb
' Project:      CSWebDownloader
' Copyright (c) Microsoft Corporation.
' 
' This interface defines the basic properties and methods of a WebDownloader. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Net

Public Interface IDownloader
#Region "Basic settings of a WebDownloader."

    Property Url() As Uri
    Property DownloadPath() As String
    Property TotalSize() As Long

    Property Credentials() As ICredentials
    Property Proxy() As IWebProxy

#End Region


#Region "Support the ""Pause"", ""Resume"" and Multi-Threads feature."

    Property IsRangeSupported() As Boolean
    Property StartPoint() As Long
    Property EndPoint() As Long

#End Region

#Region "The downloaded data and status."

    Property DownloadedSize() As Long
    Property CachedSize() As Integer

    Property HasChecked() As Boolean
    Property Status() As DownloadStatus
    ReadOnly Property TotalUsedTime() As TimeSpan

#End Region

#Region "Advanced settings of a WebDownloader"

    Property BufferSize() As Integer
    Property BufferCountPerNotification() As Integer
    Property MaxCacheSize() As Integer

#End Region


    Event DownloadCompleted As EventHandler(Of DownloadCompletedEventArgs)
    Event DownloadProgressChanged As EventHandler(Of DownloadProgressChangedEventArgs)
    Event StatusChanged As EventHandler

    Sub CheckUrl(<System.Runtime.InteropServices.Out()> ByRef fileName As String)

    Sub BeginDownload()
    Sub Download()

    Sub Pause()

    Sub [Resume]()
    Sub BeginResume()

    Sub Cancel()
End Interface

