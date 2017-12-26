'*************************** Module Header ******************************'
' Module Name:  HtmlInputElement.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This abstract class HtmlInputElement represents an HtmlElement with the tag "input".
' 
' The XmlIncludeAttribute allows the XmlSerializer to recognize the classes which 
' inherit HtmlInputElement when it serializes or deserializes an object.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Xml.Serialization

<XmlInclude(GetType(HtmlCheckBox)),
XmlInclude(GetType(HtmlPassword)),
XmlInclude(GetType(HtmlSubmit)),
XmlInclude(GetType(HtmlText))>
Public MustInherit Class HtmlInputElement
    Public Property ID() As String

    ''' <summary>
    ''' This parameterless constructor is used in deserialization.
    ''' </summary>
    Protected Sub New()
    End Sub

    Protected Sub New(ByVal ID As String)
        Me.ID = ID
    End Sub

    ''' <summary>
    ''' Set the value of the HtmlElement.
    ''' </summary>
    Public Overridable Sub SetValue(ByVal element As HtmlElement)
    End Sub

End Class
