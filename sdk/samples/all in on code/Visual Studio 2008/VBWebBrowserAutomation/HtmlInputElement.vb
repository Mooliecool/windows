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
Imports System.Security.Permissions

<XmlInclude(GetType(HtmlCheckBox)), _
XmlInclude(GetType(HtmlPassword)), _
XmlInclude(GetType(HtmlSubmit)), _
XmlInclude(GetType(HtmlText))> _
Public MustInherit Class HtmlInputElement

    Dim _id As String
    Public Property ID() As String
        Get
            Return _id
        End Get
        Set(ByVal value As String)
            _id = value
        End Set
    End Property

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
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Overridable Sub SetValue(ByVal element As HtmlElement)
    End Sub

End Class
