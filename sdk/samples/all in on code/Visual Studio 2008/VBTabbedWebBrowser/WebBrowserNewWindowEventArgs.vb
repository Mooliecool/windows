'*************************** Module Header ******************************'
' Module Name:  WebBrowserNavigateErrorEventArgs.cs
' Project:	    VBTabbedWebBrowser
' Copyright (c) Microsoft Corporation.
' 
' The class WebBrowserNavigateErrorEventArgs defines the event arguments used
' by WebBrowserEx.NewWindow3 event.
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



Public Class WebBrowserNewWindowEventArgs
    Inherits EventArgs

    Dim _url As String
    Public Property Url() As String
        Get
            Return _url
        End Get
        Set(ByVal value As String)
            _url = value
        End Set
    End Property

    Dim _cancel As Boolean
    Public Property Cancel() As Boolean
        Get
            Return _cancel
        End Get
        Set(ByVal value As Boolean)
            _cancel = value
        End Set
    End Property

    Public Sub New(ByVal url As String, ByVal cancel As Boolean)
        Me.Url = url
        Me.Cancel = cancel
    End Sub

End Class

