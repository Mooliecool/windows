'***************************** Module Header ******************************\
' Module Name:	MainWindow.vb
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
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Collections
Imports System.ComponentModel

Partial Public Class MainWindow
    Inherits Window

    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' Declare a custom sort class which implements IComparer interface 
    ''' </summary>
    Friend Class JobSort
        Implements IComparer

        Private _direction As Boolean
        Public Sub New(ByVal direction As Boolean)
            _direction = direction
        End Sub

        ''' <summary>
        ''' Implement the IComparer.Compare method to compare two objects
        ''' </summary>
        ''' <param name="x"></param>
        ''' <param name="y"></param>
        ''' <returns></returns>
        Public Function Compare(ByVal x As Object, ByVal y As Object) As Integer Implements IComparer.Compare

            Dim iXIndex As Integer = GetJobIndex(DirectCast(x, Employee).Job)
            Dim iYIndex As Integer = GetJobIndex(DirectCast(y, Employee).Job)

            ' Based on the index of this kind of job for a sort column
            Return If(_direction, 1, -1) * iXIndex.CompareTo(iYIndex)
        End Function

        ''' <summary>
        ''' Get the index value of the Job property based on the custom order
        ''' "CTO" > "Manager" > "Leader" > "Member"
        ''' </summary>
        ''' <param name="job"></param>
        ''' <returns></returns>
        Private Function GetJobIndex(ByVal job As String) As Integer
            Select Case job
                Case "CTO"
                    Return 1
                Case "Manager"
                    Return 2
                Case "Leader"
                    Return 3
                Case "Member"
                    Return 4
                Case Else
                    Return 0
            End Select
        End Function
    End Class

    ''' <summary>
    ''' This Sorting event handler when you click the Column Header named "Job"
    ''' and sort "Job" column content by Job kind 
    ''' </summary>
    Private Sub WPF_DataGrid_Sorting(ByVal sender As Object, ByVal e As DataGridSortingEventArgs)

        ' Check the sorted column which needs to implement the custom sort
        If e.Column.Header.Equals("Job") Then

            ' Get the ListCollectionView from the DataGrid
            Dim view As ListCollectionView = DirectCast(CollectionViewSource.GetDefaultView(DataGrid.ItemsSource), ListCollectionView)

            If e.Column IsNot Nothing AndAlso e.Column.CanUserSort = True Then

                ' Create an object to implement the custom sort 
                If e.Column.SortDirection = ListSortDirection.Ascending Then
                    view.CustomSort = New JobSort(False)
                    e.Column.SortDirection = ListSortDirection.Descending
                Else
                    view.CustomSort = New JobSort(True)
                    e.Column.SortDirection = ListSortDirection.Ascending
                End If

                e.Handled = True
            End If
        End If
    End Sub
End Class
