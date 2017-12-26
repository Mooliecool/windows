'*************************** Module Header ******************************'
' Module Name:  WebBrowserEx.vb
' Project:	    VBTabbedWebBrowser
' Copyright (c) Microsoft Corporation.
' 
' This WebBrowserEx class inherits WebBrowser class and can handle NewWindow3 event.
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
Imports Microsoft.Win32


Partial Public Class WebBrowserEx
    Inherits WebBrowser

    Private _cookie As AxHost.ConnectionPointCookie

    Private _helper As DWebBrowserEvent2Helper

    Public Event NewWindow3 As EventHandler(Of WebBrowserNewWindowEventArgs)

    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")>
    Public Sub New()
    End Sub

    ''' <summary>
    ''' Associates the underlying ActiveX control with a client that can 
    ''' handle control events including NewWindow3 event.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")>
    Protected Overrides Sub CreateSink()
        MyBase.CreateSink()

        _helper = New DWebBrowserEvent2Helper(Me)
        _cookie = New AxHost.ConnectionPointCookie(
            Me.ActiveXInstance, _helper, GetType(DWebBrowserEvents2))
    End Sub


    ''' <summary>
    ''' Releases the event-handling client attached in the CreateSink method
    ''' from the underlying ActiveX control
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")>
    Protected Overrides Sub DetachSink()
        If _cookie IsNot Nothing Then
            _cookie.Disconnect()
            _cookie = Nothing
        End If
        MyBase.DetachSink()
    End Sub


    ''' <summary>
    '''  Raises the NewWindow3 event.
    ''' </summary>
    Protected Overridable Sub OnNewWindow3(ByVal e As WebBrowserNewWindowEventArgs)
        RaiseEvent NewWindow3(Me, e)
    End Sub
End Class
