' *************************** Module Header ******************************\
' Module Name:  InternetProxy.vb
' Project:      VBWebBrowserWithProxy
' Copyright (c) Microsoft Corporation.
' 
' This class is used to describe a proxy server and the credential to connect to it.
' Please set the proxy servers in ProxyList.xml first.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
' **************************************************************************

Public Class InternetProxy

    Dim _proxyName As String
    Public Property ProxyName() As String
        Get
            Return _proxyName
        End Get
        Set(ByVal value As String)
            _proxyName = value
        End Set
    End Property

    Dim _address As String
    Public Property Address() As String
        Get
            Return _address
        End Get
        Set(ByVal value As String)
            _address = value
        End Set
    End Property

    Dim _userName As String
    Public Property UserName() As String
        Get
            Return _userName
        End Get
        Set(ByVal value As String)
            _userName = value
        End Set
    End Property

    Dim _password As String
    Public Property Password() As String
        Get
            Return _password
        End Get
        Set(ByVal value As String)
            _password = value
        End Set
    End Property

    Public Overloads Overrides Function Equals(ByVal obj As Object) As Boolean
        If TypeOf obj Is InternetProxy Then
            Dim proxy As InternetProxy = TryCast(obj, InternetProxy)

            Return Me.Address.Equals(proxy.Address, System.StringComparison.OrdinalIgnoreCase) _
                AndAlso Me.UserName.EndsWith(proxy.UserName, System.StringComparison.OrdinalIgnoreCase) _
                AndAlso Me.Password.Equals(proxy.Password, System.StringComparison.Ordinal)
        Else
            Return False
        End If
    End Function

    Public Overrides Function GetHashCode() As Integer
        Return Me.Address.GetHashCode() _
            + Me.UserName.GetHashCode() _
            + Me.Password.GetHashCode()
    End Function

    Public Shared ReadOnly NoProxy As InternetProxy = New InternetProxy With {.Address = String.Empty, .Password = String.Empty, .ProxyName = String.Empty, .UserName = String.Empty}

    Private Shared _proxyList As List(Of InternetProxy) = Nothing
    Public Shared ReadOnly Property ProxyList() As List(Of InternetProxy)
        Get
            ' Gets the proxy servers in ProxyList.xml.
            If _proxyList Is Nothing Then
                _proxyList = New List(Of InternetProxy)()
                Try
                    Dim listXml As XElement = XElement.Load("ProxyList.xml")
                    For Each proxy In listXml.Elements("Proxy")
                        _proxyList.Add(New InternetProxy With _
                                       {.ProxyName = proxy.Element("ProxyName").Value, _
                                        .Address = proxy.Element("Address").Value, _
                                        .UserName = proxy.Element("UserName").Value, _
                                        .Password = proxy.Element("Password").Value})
                    Next proxy
                Catch e1 As System.Exception
                End Try
            End If
            Return _proxyList
        End Get
    End Property
End Class