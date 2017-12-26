'****************************** Module Header ******************************'
' Module Name:                PersonEntity.vb
' Project:                    VBSL4COMInterop
' Copyright (c) Microsoft Corporation.
' 
' Entity definition file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Public Class PersonEntity
    Private _Name As String
    Public Property Name As String
        Get
            Return _Name
        End Get
        Set(ByVal value As String)
            _Name = value
        End Set
    End Property
    Private _Gender As String
    Public Property Gender As String
        Get
            Return _Gender
        End Get
        Set(ByVal value As String)
            _Gender = value
        End Set
    End Property
    Private _Age As Integer
    Public Property Age As Integer
        Get
            Return _Age
        End Get
        Set(ByVal value As Integer)
            _Age = value
        End Set
    End Property
End Class