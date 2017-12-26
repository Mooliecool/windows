'********************************* Module Header *********************************'
' Module Name:  Person.vb
' Project:      VBWinFormBindToNestedProperties
' Copyright (c) Microsoft Corporation.
' 
' The sample demonstrates how to bind a DataGridView column to a nested property 
' in the data source.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*********************************************************************************'

Imports System.ComponentModel

' Add a TypeDescriptionProviderAttribute on the Person class
<TypeDescriptionProvider(GetType(MyTypeDescriptionProvider))> _
Public Class Person
    Private _id As String
    Private _name As String
    Private _homeAddr As Address

    Public Property ID() As String
        Get
            Return _id
        End Get
        Set(ByVal value As String)
            _id = value
        End Set
    End Property

    Public Property Name() As String
        Get
            Return _name
        End Get
        Set(ByVal value As String)
            _name = value
        End Set
    End Property

    Public Property HomeAddr() As Address
        Get
            Return _homeAddr
        End Get
        Set(ByVal value As Address)
            _homeAddr = value
        End Set
    End Property
End Class

Public Class Address
    Private _cityname As String
    Private _postcode As String

    Public Property Cityname() As String
        Get
            Return _cityname
        End Get
        Set(ByVal value As String)
            _cityname = value
        End Set
    End Property

    Public Property Postcode() As String
        Get
            Return _postcode
        End Get
        Set(ByVal value As String)
            _postcode = value
        End Set
    End Property

End Class