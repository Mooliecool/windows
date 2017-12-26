'************************** Module Header ******************************'
' Module Name:  DownloadProgressChangedEventArgs.vb
' Project:      VBWebDownloader
' Copyright (c) Microsoft Corporation.
' 
' The class DownloadProgressChangedEventArgs defines the arguments used by
' the DownloadProgressChanged event of HttpDownloadClient.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Public Class DownloadProgressChangedEventArgs
    Inherits EventArgs

    Private _receivedSize As Int64
    Public Property ReceivedSize() As Int64
        Get
            Return _receivedSize
        End Get
        Private Set(ByVal value As Int64)
            _receivedSize = value
        End Set
    End Property

    Private _totalSize As Int64
    Public Property TotalSize() As Int64
        Get
            Return _totalSize
        End Get
        Private Set(ByVal value As Int64)
            _totalSize = value
        End Set
    End Property

    Private _downloadSpeed As Integer
    Public Property DownloadSpeed() As Integer
        Get
            Return _downloadSpeed
        End Get
        Private Set(ByVal value As Integer)
            _downloadSpeed = value
        End Set
    End Property

    Public Sub New(ByVal receivedSize As Int64, _
                   ByVal totalSize As Int64, _
                   ByVal downloadSpeed As Integer)
        Me.ReceivedSize = receivedSize
        Me.TotalSize = TotalSize
        Me.DownloadSpeed = DownloadSpeed
    End Sub
End Class

