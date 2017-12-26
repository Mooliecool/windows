'*************************** Module Header ******************************'
' Module Name:  ErrorEventArgs.vb
' Project:	    VBFTPUpload
' Copyright (c) Microsoft Corporation.
' 
' The class ErrorEventArgs defines the arguments used by the ErrorOccurred 
' event of FTPClient.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Public Class ErrorEventArgs
    Inherits EventArgs

    Dim _errorException As Exception

    Public Property ErrorException() As Exception
        Get
            Return _errorException
        End Get
        Set(ByVal value As Exception)
            _errorException = value
        End Set
    End Property
End Class

