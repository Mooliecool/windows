'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
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

Imports System.Collections.ObjectModel

''' <summary> 
''' Interaction logic for MainWindow.xaml 
''' </summary> 
Partial Public Class MainWindow
    Inherits Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Private _students As ObservableCollection(Of Student)

    Private Sub Window_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Set up data source 
        _students = New ObservableCollection(Of Student)()

        _students.Add(New Student With {.ID = 1, .Name = "Johnson", .Class = "Class A", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 2, .Name = "Bill", .Class = "Class A", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 3, .Name = "Amy", .Class = "Class B", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 4, .Name = "Polo", .Class = "Class B", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 5, .Name = "Shalia", .Class = "Class C", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 6, .Name = "Olay", .Class = "Class C", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 7, .Name = "Sea", .Class = "Class C", .Grade = "Grade 1"})
        _students.Add(New Student With {.ID = 8, .Name = "Jim", .Class = "Class A", .Grade = "Grade 2"})
        _students.Add(New Student With {.ID = 9, .Name = "Tim", .Class = "Class A", .Grade = "Grade 2"})
        _students.Add(New Student With {.ID = 10, .Name = "Bruce", .Class = "Class A", .Grade = "Grade 2"})
        _students.Add(New Student With {.ID = 11, .Name = "Bockei", .Class = "Class B", .Grade = "Grade 2"})
        _students.Add(New Student With {.ID = 12, .Name = "Obama", .Class = "Class A", .Grade = "Grade 3"})
        _students.Add(New Student With {.ID = 13, .Name = "Linkon", .Class = "Class B", .Grade = "Grade 3"})
        _students.Add(New Student With {.ID = 14, .Name = "Asura", .Class = "Class B", .Grade = "Grade 3"})
        _students.Add(New Student With {.ID = 15, .Name = "Jack", .Class = "Class C", .Grade = "Grade 3"})
        _students.Add(New Student With {.ID = 16, .Name = "Rickie", .Class = "Class C", .Grade = "Grade 3"})

        Dim _csv As New CollectionViewSource()
        _csv.Source = _students

        ' Group by Grade and Class 
        _csv.GroupDescriptions.Add(New PropertyGroupDescription("Grade"))
        _csv.GroupDescriptions.Add(New PropertyGroupDescription("Class"))

        Me.listView1.DataContext = _csv
    End Sub
End Class

Public Class GroupItemStyleSelector
    Inherits StyleSelector
    Public Overloads Overrides Function SelectStyle(ByVal item As Object, ByVal container As DependencyObject) As Style
        Dim s As Style

        Dim group As CollectionViewGroup = TryCast(item, CollectionViewGroup)
        Dim window As Window = Application.Current.MainWindow

        If Not group.IsBottomLevel Then
            s = TryCast(window.FindResource("GroupHeaderStyleForFirstLevel"), Style)
        Else
            s = TryCast(window.FindResource("GroupHeaderStyleForSecondLevel"), Style)
        End If

        Return s
    End Function
End Class
