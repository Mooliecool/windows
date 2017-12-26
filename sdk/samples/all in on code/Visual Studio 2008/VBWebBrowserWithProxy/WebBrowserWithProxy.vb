' *************************** Module Header ******************************\
' Module Name:  WebBrowserWithProxy.vb
' Project:      VBWebBrowserWithProxy
' Copyright (c) Microsoft Corporation.
' 
' This WebBrowserWithProxy class inherits WebBrowser class and has a feature 
' to set proxy server. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
' **************************************************************************

Imports System.Security.Permissions
Imports System.Text
Imports System.ComponentModel

Public Class WebBrowserWithProxy
    Inherits WebBrowser

    Private _proxy As InternetProxy = InternetProxy.NoProxy

    ' The proxy server to connect.
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), Browsable(False)> _
    Public Property Proxy() As InternetProxy
        Get
            Return _proxy
        End Get
        Set(ByVal value As InternetProxy)
            Dim newProxy = InternetProxy.NoProxy
            If value IsNot Nothing Then
                newProxy = value
            End If

            If Not _proxy.Equals(newProxy) Then
                _proxy = newProxy

                If _proxy IsNot Nothing AndAlso (Not String.IsNullOrEmpty(_proxy.Address)) Then
                    WinINet.SetConnectionProxy(False, _proxy.Address)
                Else
                    WinINet.RestoreSystemProxy()
                End If
            End If
        End Set
    End Property

    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New()
    End Sub

    ''' <summary>
    ''' Wrap the method Navigate and set the Proxy-Authorization header if needed.
    ''' </summary>
    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub [Goto](ByVal url As String)
        Dim uri As System.Uri = Nothing
        Dim result As Boolean = System.Uri.TryCreate(url, UriKind.RelativeOrAbsolute, uri)
        If Not result Then
            Throw New ArgumentException("The url is not valid. ")
        End If

        ' If the proxy contains user name and password, then set the Proxy-Authorization
        ' header of the request.
        If Proxy IsNot Nothing AndAlso (Not String.IsNullOrEmpty(Proxy.UserName)) AndAlso (Not String.IsNullOrEmpty(Proxy.Password)) Then

            ' This header uses Base64String to store the credential.
            Dim credentialStringValue = String.Format("{0}:{1}", Proxy.UserName, Proxy.Password)
            Dim credentialByteArray = ASCIIEncoding.ASCII.GetBytes(credentialStringValue)
            Dim credentialBase64String = Convert.ToBase64String(credentialByteArray)
            Dim authHeader As String = String.Format("Proxy-Authorization: Basic {0}", credentialBase64String)

            Navigate(uri, String.Empty, Nothing, authHeader)
        Else
            Navigate(uri)
        End If
    End Sub
End Class