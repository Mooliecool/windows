'*************************** Module Header ******************************'
' Module Name:  HTMLDocumentEventHelper.vb
' Project:	    VBBrowserHelperObject
' Copyright (c) Microsoft Corporation.
' 
' This ComVisible class HTMLDocumentEventHelper is used to set the event handler
' of the HTMLDocument. The interface DispHTMLDocument defines many events like 
' oncontextmenu, onclick and so on, and these events could be set to an
' HTMLEventHandler instance.
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
Imports mshtml

<ComVisible(True)> _
Public Class HTMLDocumentEventHelper
    Private document As HTMLDocument

    Public Sub New(ByVal document As HTMLDocument)
        Me.document = document
    End Sub

    Public Custom Event oncontextmenu As HtmlEvent
        AddHandler(ByVal value As HtmlEvent)
            Dim dispDoc As DispHTMLDocument = TryCast(Me.document, DispHTMLDocument)

            Dim existingHandler As Object = dispDoc.oncontextmenu
            Dim handler As HTMLEventHandler = _
                If(TypeOf existingHandler Is HTMLEventHandler, _
                   TryCast(existingHandler, HTMLEventHandler), _
                   New HTMLEventHandler(Me.document))

            ' Set the handler to the oncontextmenu event.
            dispDoc.oncontextmenu = handler

            AddHandler handler.eventHandler, value

        End AddHandler
        RemoveHandler(ByVal value As HtmlEvent)
            Dim dispDoc As DispHTMLDocument = TryCast(Me.document, DispHTMLDocument)
            Dim existingHandler As Object = dispDoc.oncontextmenu

            Dim handler As HTMLEventHandler = _
                If(TypeOf existingHandler Is HTMLEventHandler, _
                   TryCast(existingHandler, HTMLEventHandler), Nothing)

                If handler IsNot Nothing Then
                    RemoveHandler handler.eventHandler, value
                End If
        End RemoveHandler
        RaiseEvent(ByVal e As mshtml.IHTMLEventObj)

        End RaiseEvent
    End Event
End Class

