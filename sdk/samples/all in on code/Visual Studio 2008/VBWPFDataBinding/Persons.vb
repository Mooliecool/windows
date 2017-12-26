'****************************** Module Header ******************************'
' Module Name:  Persons.vb
' Project:      VBWPFDataBinding
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to use DataBinding in WPF
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Collections.ObjectModel

Public Class Persons
    Inherits ObservableCollection(Of Person)
    ''' <summary> 
    ''' Initializes a new instance of the <see cref="Persons"/> class. 
    ''' </summary> 
    Public Sub New()
        Me.Add(New Person() With {.Name = "employee1", .Age = 20, .Interest = "Basketball", .Job = "programmer", .Salary = 300})

        Me.Add(New Person() With {.Name = "employee2", .Age = 21, .Interest = "PC game", .Job = "programmer", .Salary = 200})

        Me.Add(New Person() With {.Name = "employee3", .Age = 21, .Interest = "PC game", .Job = "programmer", .Salary = 300})
    End Sub
End Class