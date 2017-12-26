'************************** Module Header ******************************\
' Module Name:  WebBrowserEx.vb
' Project:      VBWebBrowserLoadComplete
' Copyright (c) Microsoft Corporation.
' 
' This WebBrowserEx class inherits WebBrowser class and supplies LoadCompleted 
' event.
' 
' In the case of a page with no frames, DocumentComplete is fired once after 
' everything is done. In case of multiple frames, DocumentComplete gets fired
' multiple times. So if the DocumentCompleted event is fired, it does not mean
' that the page is done loading.
' 
' So, to check if a page is done loading, you need to check if the 
' sender is the same as the the WebBrowser control. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************

Imports System.Security.Permissions


<PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust"),
PermissionSetAttribute(SecurityAction.InheritanceDemand, Name:="FullTrust")>
Partial Public Class WebBrowserEx
    Inherits WebBrowser
    Private cookie As AxHost.ConnectionPointCookie

    Private helper As DWebBrowserEvents2Helper

    Public Event LoadCompleted As EventHandler(Of WebBrowserDocumentCompletedEventArgs)

    Public Event StartNavigating As EventHandler(Of WebBrowserNavigatingEventArgs)

    ''' <summary>
    ''' Associates the underlying ActiveX control with a client that can 
    ''' handle control events including NavigateError event.
    ''' </summary>
    Protected Overrides Sub CreateSink()
        MyBase.CreateSink()

        helper = New DWebBrowserEvents2Helper(Me)
        cookie = New AxHost.ConnectionPointCookie(
            Me.ActiveXInstance, helper, GetType(DWebBrowserEvents2))
    End Sub

    ''' <summary>
    ''' Releases the event-handling client attached in the CreateSink method
    ''' from the underlying ActiveX control
    ''' </summary>
    Protected Overrides Sub DetachSink()
        If cookie IsNot Nothing Then
            cookie.Disconnect()
            cookie = Nothing
        End If
        MyBase.DetachSink()
    End Sub

    ''' <summary>
    ''' Raise the LoadCompleted event.
    ''' </summary>
    Protected Overridable Sub OnLoadCompleted(ByVal e As WebBrowserDocumentCompletedEventArgs)

        RaiseEvent LoadCompleted(Me, e)
    End Sub

    ''' <summary>
    ''' Raise the StartNavigating event.
    ''' </summary>
    Protected Overridable Sub OnStartNavigating(ByVal e As WebBrowserNavigatingEventArgs)
        RaiseEvent StartNavigating(Me, e)
    End Sub
End Class

