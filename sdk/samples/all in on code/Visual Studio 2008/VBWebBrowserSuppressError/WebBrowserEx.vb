'*************************** Module Header ******************************'
' Module Name:  WebBrowserEx.vb
' Project:	    VBWebBrowserSuppressError
' Copyright (c) Microsoft Corporation.
' 
' This WebBrowserEx class inherits WebBrowser class and supplies following 
' features.
' 1. Disable JIT Debugger.
' 2. Suppress html element errors of document loaded in this browser.
' 3. Handle navigation error.
' 
' The class WebBrowser itself also has a Property ScriptErrorsSuppressed to hides
' all its dialog boxes that originate from the underlying ActiveX control, not 
' just script errors.  
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

    ''' <summary>
    ''' Get or Set whether JIT debugger needs to be disabled. You have to restart the
    ''' browser to take effect.
    ''' </summary>
    Public Shared Property JITDebuggerDisabled() As Boolean
        Get
            Using ieMainKey As RegistryKey = Registry.CurrentUser.OpenSubKey( _
                "Software\Microsoft\Internet Explorer\Main")
                Dim keyvalue As String = TryCast(ieMainKey.GetValue("Disable Script Debugger"), String)
                Return String.Equals(keyvalue, "yes", StringComparison.OrdinalIgnoreCase)
            End Using
        End Get
        Set(ByVal value As Boolean)
            Dim newValue = If(value, "yes", "no")

            Using ieMainKey As RegistryKey = Registry.CurrentUser.OpenSubKey( _
                "Software\Microsoft\Internet Explorer\Main", True)
                Dim keyvalue As String = TryCast(ieMainKey.GetValue("Disable Script Debugger"), String)
                If Not keyvalue.Equals(newValue, StringComparison.OrdinalIgnoreCase) Then
                    ieMainKey.SetValue("Disable Script Debugger", newValue)
                End If
            End Using
        End Set
    End Property

    ' Suppress html element errors.
    Dim _htmlElementErrorsSuppressed As Boolean
    Public Property HtmlElementErrorsSuppressed() As Boolean
        Get
            Return _htmlElementErrorsSuppressed
        End Get
        Set(ByVal value As Boolean)
            _htmlElementErrorsSuppressed = value
        End Set
    End Property

    Private cookie As AxHost.ConnectionPointCookie

    Private helper As WebBrowser2EventHelper

    Public Event NavigateError As EventHandler(Of WebBrowserNavigateErrorEventArgs)

    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New()
    End Sub

    ''' <summary>
    ''' Register the Document.Window.Error event.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Protected Overrides Sub OnDocumentCompleted(ByVal e As WebBrowserDocumentCompletedEventArgs)
        MyBase.OnDocumentCompleted(e)

        ' Occurs when script running inside of the window encounters a run-time error.
        AddHandler Document.Window.Error, AddressOf Window_Error

    End Sub



    ''' <summary>
    ''' Handle html element errors of document loaded in this browser. 
    ''' If HtmlElementErrorsSuppressed is set to true, then set the Handled flag to true so
    ''' that browser will not display this error.
    ''' </summary>
    Protected Sub Window_Error(ByVal sender As Object, ByVal e As HtmlElementErrorEventArgs)
        If HtmlElementErrorsSuppressed Then
            e.Handled = True
        End If
    End Sub

    ''' <summary>
    ''' Associates the underlying ActiveX control with a client that can 
    ''' handle control events including NavigateError event.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Protected Overrides Sub CreateSink()
        MyBase.CreateSink()

        helper = New WebBrowser2EventHelper(Me)
        cookie = New AxHost.ConnectionPointCookie(Me.ActiveXInstance, helper, GetType(DWebBrowserEvents2))
    End Sub

    ''' <summary>
    ''' Releases the event-handling client attached in the CreateSink method
    ''' from the underlying ActiveX control
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Protected Overrides Sub DetachSink()
        If Not cookie Is Nothing Then
            cookie.Disconnect()
            cookie = Nothing

        End If

        MyBase.DetachSink()
    End Sub

    ''' <summary>
    '''  Raises the NavigateError event.
    ''' </summary>
    Protected Overridable Sub OnNavigateError(ByVal e As WebBrowserNavigateErrorEventArgs)
        RaiseEvent NavigateError(Me, e)
    End Sub

End Class

