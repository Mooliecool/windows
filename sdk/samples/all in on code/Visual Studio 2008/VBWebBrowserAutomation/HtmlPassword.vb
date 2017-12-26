'*************************** Module Header ******************************'
' Module Name:  HtmlPassword.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This class HtmlPassword represents an HtmlElement with the tag "input" and its 
' type is "password".
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

Public Class HtmlPassword
    Inherits HtmlInputElement


    Dim _value As String
    Public Property Value() As String
        Get
            Return _value
        End Get
        Set(ByVal value As String)
            _value = value
        End Set
    End Property

    ''' <summary>
    ''' This parameterless constructor is used in deserialization.
    ''' </summary>
    Public Sub New()
    End Sub

    ''' <summary>
    ''' Initialize an instance of HtmlPassword. This constructor is used by 
    ''' HtmlInputElementFactory.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New(ByVal element As HtmlElement)
        MyBase.New(element.Id)
        Value = element.GetAttribute("value")
    End Sub

    ''' <summary>
    ''' Set the value of the HtmlElement.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Overrides Sub SetValue(ByVal element As HtmlElement)
        element.SetAttribute("value", Value)
    End Sub
End Class
