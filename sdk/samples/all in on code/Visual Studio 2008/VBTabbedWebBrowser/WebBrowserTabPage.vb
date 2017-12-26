'*************************** Module Header ******************************'
' Module Name:  WebBrowserTabPage.vb
' Project:	    VBTabbedWebBrowser
' Copyright (c) Microsoft Corporation.
' 
' This class inherits the the System.Windows.Forms.TabPage class and contains
' a WebBrowserEx property. An instance of this class could be add to a tab control
' directly.
' 
' It exposes the NewWindow3 event of WebBrowserEx, and handle the DocumentTitleChanged
' event.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Security.Permissions

Public Class WebBrowserTabPage
    Inherits TabPage
    Private _webBrowser As WebBrowserEx
    Public Property WebBrowser() As WebBrowserEx
        Get
            Return _webBrowser
        End Get
        Private Set(ByVal value As WebBrowserEx)
            _webBrowser = value
        End Set
    End Property

    ' Expose the NewWindow3 event of WebBrowserEx.
    Public Custom Event NewWindow As EventHandler(Of WebBrowserNewWindowEventArgs)
        AddHandler(ByVal value As EventHandler(Of WebBrowserNewWindowEventArgs))
            AddHandler WebBrowser.NewWindow3, value
        End AddHandler
        RemoveHandler(ByVal value As EventHandler(Of WebBrowserNewWindowEventArgs))
            RemoveHandler WebBrowser.NewWindow3, value
        End RemoveHandler
        RaiseEvent()
        End RaiseEvent
    End Event

    ''' <summary>
    ''' Initialize the WebBrowserEx instance.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New()
        MyBase.New()
        WebBrowser = New WebBrowserEx()
        WebBrowser.Dock = DockStyle.Fill
        AddHandler WebBrowser.DocumentTitleChanged, AddressOf WebBrowser_DocumentTitleChanged

        Me.Controls.Add(WebBrowser)
    End Sub

    ''' <summary>
    ''' Change the title of the tab page.
    ''' </summary>
    Private Sub WebBrowser_DocumentTitleChanged(ByVal sender As Object, ByVal e As EventArgs)
        Me.Text = WebBrowser.DocumentTitle
    End Sub

End Class
