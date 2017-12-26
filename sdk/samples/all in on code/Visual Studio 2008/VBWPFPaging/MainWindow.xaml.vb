'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWPFPaging
' Copyright (c) Microsoft Corporation.
' 
' The sample demonstrates how to page data in WPF.
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

    Private view As New CollectionViewSource()
    Private customers As New ObservableCollection(Of Customer)()
    Private currentPageIndex As Integer = 0
    Private itemPerPage As Integer = 20
    Private totalPage As Integer = 0

    Private Sub Window_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim itemcount As Integer = 107
        For j As Integer = 0 To itemcount - 1
            customers.Add(New Customer With {.ID = j, .Name = "item" + j.ToString(), .Age = 10 + j, .Country = "China"})
        Next

        ' Calculate the total pages 
        totalPage = itemcount / itemPerPage
        If itemcount Mod itemPerPage <> 0 Then
            totalPage += 1
        End If

        view.Source = customers

        AddHandler view.Filter, AddressOf view_Filter

        Me.listView1.DataContext = view

        tbTotalPage.Text = totalPage.ToString()
        tbCurrentPage.Text = "1"

    End Sub

    Private Sub view_Filter(ByVal sender As Object, ByVal e As FilterEventArgs)
        Dim index As Integer = customers.IndexOf(DirectCast(e.Item, Customer))

        If index >= itemPerPage * currentPageIndex AndAlso index < itemPerPage * (currentPageIndex + 1) Then
            e.Accepted = True
        Else
            e.Accepted = False
        End If
    End Sub

    Private Sub btnFirst_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Display the first page 
        If currentPageIndex <> 0 Then
            currentPageIndex = 0
            UpdateCurrentPage()

        End If
    End Sub

    Private Sub btnPrev_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Display previous page 
        If currentPageIndex > 0 Then
            currentPageIndex -= 1
            UpdateCurrentPage()
        End If
    End Sub

    Private Sub btnNext_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Display next page 
        If currentPageIndex < totalPage - 1 Then
            currentPageIndex += 1
            UpdateCurrentPage()
        End If
    End Sub

    Private Sub btnLast_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Display the last page 
        If currentPageIndex <> totalPage - 1 Then
            currentPageIndex = totalPage - 1
            UpdateCurrentPage()
        End If
    End Sub

    Private Sub UpdateCurrentPage()
        view.View.Refresh()
        tbCurrentPage.Text = (currentPageIndex + 1).ToString()
    End Sub
End Class