'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWPFAnimationWhenDataChanged
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to trigger animation when the value of the 
' datagrid cell is changed.
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
Imports System.Windows.Threading
Imports Microsoft.Windows.Controls

Partial Public Class MainWindow
    Inherits Window
    Private list As New NameList()
    Public Sub New()
        InitializeComponent()
        'hook up AutoGeneratingColumn event 
        AddHandler Me.dataGrid1.AutoGeneratingColumn, AddressOf dataGrid1_AutoGeneratingColumn

        Me.DataContext = list
    End Sub
    'set NotifyOnTargetUpdated property of each binding to true. 
    Private Sub dataGrid1_AutoGeneratingColumn(ByVal sender As Object, ByVal e As Microsoft.Windows.Controls.DataGridAutoGeneratingColumnEventArgs)

        Dim binding As Binding = TryCast(TryCast(e.Column, DataGridTextColumn).Binding, Binding)

        binding.NotifyOnTargetUpdated = True
    End Sub


    ''' <summary> 
    ''' Handles the Click event of the btnHookupAnimation control, 
    ''' apply the style for each column here. 
    ''' </summary> 
    ''' <param name="sender">The source of the event.</param> 
    ''' <param name="e">The <see cref="System.Windows.RoutedEventArgs"/> 
    ''' instance containing the event data.</param> 
    Private Sub btnHookupAnimation_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        For Each col In dataGrid1.Columns
            Dim textCol As DataGridTextColumn = TryCast(col, DataGridTextColumn)
            textCol.CellStyle = TryCast(Me.FindResource("cellStyle"), Style)
        Next
    End Sub
End Class