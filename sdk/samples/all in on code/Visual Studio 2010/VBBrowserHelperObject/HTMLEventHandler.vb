'*************************** Module Header ******************************'
' Module Name:  HTMLEventHandler.vb
' Project:	    VBBrowserHelperObject
' Copyright (c) Microsoft Corporation.
' 
' This ComVisible class HTMLEventHandler can be assign to the event properties
' of DispHTMLDocument interfaces, like oncontextmenu, onclick and so on. It can
' also be used in other HTMLElements.
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

Imports System.Runtime.InteropServices

' The delegate of the handler method.
Public Delegate Sub HtmlEvent(ByVal e As mshtml.IHTMLEventObj)

<ComVisible(True)>
Public Class HTMLEventHandler

    Private htmlDocument As mshtml.HTMLDocument

    Public Event eventHandler As HtmlEvent

    Public Sub New(ByVal htmlDocument As mshtml.HTMLDocument)
        Me.htmlDocument = htmlDocument
    End Sub

    <DispId(0)>
    Public Sub FireEvent()
        RaiseEvent eventHandler(Me.htmlDocument.parentWindow.event)
    End Sub
End Class
