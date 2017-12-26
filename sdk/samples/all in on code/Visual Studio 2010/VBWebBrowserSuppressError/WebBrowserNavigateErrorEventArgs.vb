'*************************** Module Header ******************************'
' Module Name:  WebBrowserNavigateErrorEventArgs.vb
' Project:	    VBWebBrowserSuppressError
' Copyright (c) Microsoft Corporation.
' 
' The class WebBrowserNavigateErrorEventArgs defines the event arguments used
' by WebBrowserEx.NavigateError event.
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


Public Class WebBrowserNavigateErrorEventArgs
    Inherits EventArgs
    Public Property Url() As String

    Public Property Frame() As String

    Public Property StatusCode() As Int32

    Public Property Cancel() As Boolean

    Public Sub New(ByVal url As String, ByVal frame As String,
                   ByVal statusCode As Int32, ByVal cancel As Boolean)
        Me.Url = url
        Me.Frame = frame
        Me.StatusCode = statusCode
        Me.Cancel = cancel
    End Sub

End Class

