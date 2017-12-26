'***************************** Module Header ******************************\
' Module Name:	DataCenter.vb
' Project:	    VBWPFDataGridCustomSort
' Copyright (c) Microsoft Corporation.
'
' The VBWPFDataGridCustomSort demonstrates how to implement
' a custom sort for one or severalcolumns in WPF DataGrid control.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************


Imports System.Windows
Imports System.Collections.ObjectModel

Public Class Employees
    Inherits ObservableCollection(Of Employee)

    Public Sub New()
        Add(New Employee("heryb", "Hern Brown", "Member"))
        Add(New Employee("smith", "Smith", "Manager"))
        Add(New Employee("mike", "Michael", "CTO"))
        Add(New Employee("abill", "Alen Bill", "Member"))
        Add(New Employee("leevick", "Vick Lee", "Leader"))
    End Sub
End Class

Public Class Employee

    Public Sub New(ByVal [alias] As String, ByVal name As String, ByVal job As String)
        m_Alias = [alias]
        m_Name = name
        m_Job = job
    End Sub

    Private m_Alias As String
    Public Property [Alias] As String
        Get
            Return m_Alias
        End Get
        Set(ByVal value As String)
            m_Alias = value
        End Set
    End Property

    Private m_Name As String
    Public Property Name As String
        Get
            Return m_Name
        End Get
        Set(ByVal value As String)
            m_Name = value
        End Set
    End Property

    Private m_Job As String
    Public Property Job As String
        Get
            Return m_Job
        End Get
        Set(ByVal value As String)
            m_Job = value
        End Set
    End Property
End Class
