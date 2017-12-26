'*************************** Module Header ******************************'
' Module Name:  FileUploadCompletedEventArgs.vb
' Project:	    VBFTPUpload
' Copyright (c) Microsoft Corporation.
' 
' The class FileUploadCompletedEventArgs defines the arguments used by the 
' FileUploadCompleted event of FTPClient.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.IO

Public Class FileUploadCompletedEventArgs
    Inherits EventArgs

    Dim _serverPath As Uri
    Public Property ServerPath() As Uri
        Get
            Return _serverPath
        End Get
        Set(ByVal value As Uri)
            _serverPath = value
        End Set
    End Property

    Dim _localFile As FileInfo
    Public Property LocalFile() As FileInfo
        Get
            Return _localFile
        End Get
        Set(ByVal value As FileInfo)
            _localFile = value
        End Set
    End Property

End Class
