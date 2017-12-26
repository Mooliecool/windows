'****************************** Module Header ******************************'
' Module Name:  Student.vb
' Project:      VBWPFTwoLevelGrouping
' Copyright (c) Microsoft Corporation.
' 
' The sample demonstrates how to display two level grouped data in WPF.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Class Student
    Private _ID As Integer
    Public Property ID() As Integer
        Get
            Return _ID
        End Get
        Set(ByVal value As Integer)
            _ID = value
        End Set
    End Property
    Private _Name As String
    Public Property Name() As String
        Get
            Return _Name
        End Get
        Set(ByVal value As String)
            _Name = value
        End Set
    End Property
    Private _Class As String
    Public Property [Class]() As String
        Get
            Return _Class
        End Get
        Set(ByVal value As String)
            _Class = value
        End Set
    End Property
    Private _Grade As String
    Public Property Grade() As String
        Get
            Return _Grade
        End Get
        Set(ByVal value As String)
            _Grade = value
        End Set
    End Property
End Class
