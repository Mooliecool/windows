'*************************** Module Header ******************************'
' Module Name:  WebBrowserEx.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This WebBrowserEx class inherits WebBrowser class. It supplies following features:
' 1. Block the specified web sites.
' 2. Complete the html input elements automatically.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Linq
Imports System.Security.Permissions
Imports System.ComponentModel

Partial Public Class WebBrowserEx
    Inherits WebBrowser

    Private _privateCanAutoComplete As Boolean

    ''' <summary>
    ''' Specify whether the current page could be completed automatically.
    ''' </summary>
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), Browsable(False)> _
    Public Property CanAutoComplete() As Boolean
        Get
            Return _privateCanAutoComplete
        End Get
        Private Set(ByVal value As Boolean)
            _privateCanAutoComplete = value
        End Set
    End Property

    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New()
    End Sub

    ''' <summary>
    ''' After the docunment is loaded, check whether the page could be completed
    ''' automatically.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Protected Overrides Sub OnDocumentCompleted(ByVal e As WebBrowserDocumentCompletedEventArgs)

        ' Check whether the current page has been stored.
        Dim form As StoredSite = StoredSite.GetStoredSite(Me.Url.Host)
        CanAutoComplete = form IsNot Nothing _
            AndAlso form.Urls.Contains(Me.Url.AbsolutePath.ToLower())

        MyBase.OnDocumentCompleted(e)
    End Sub

    ''' <summary>
    ''' Complete the page automatically
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub AutoComplete()
        If CanAutoComplete Then
            Dim form As StoredSite = StoredSite.GetStoredSite(Me.Url.Host)
            form.FillWebPage(Me.Document, True)
        End If
    End Sub

    ''' <summary>
    ''' Check whether the url is included in the block list.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Protected Overrides Sub OnNavigating(ByVal e As WebBrowserNavigatingEventArgs)
        If BlockSites.Instance.Hosts.Contains(e.Url.Host.ToLower()) Then
            e.Cancel = True
            Me.Navigate(String.Format("{0}\Resources\Block.htm", _
                                      Environment.CurrentDirectory))
        Else
            MyBase.OnNavigating(e)
        End If
    End Sub

End Class
