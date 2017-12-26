'***************************** Module Header ******************************\
' Module Name:	MainWindow.xaml.vb
' Project:	    VBWPFCascadeDataGridColumns
' Copyright (c) Microsoft Corporation.
' 
' The VBWPFCascadeDataGridComboBoxColumns demonstrates how to implement
' cascade DataGridComboBoxColumn in WPF DataGrid control.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************/


Imports System.Data

Class MainWindow
    Public Enum Countries
        China
        UnitedStates
    End Enum

    Public Enum ChinaCities
        Beijing
        Shanghai
    End Enum

    Public Enum UnitedStatesCities
        NewYork
        Washington
    End Enum

    Dim table As DataTable = Nothing
    Dim strChinaCities(), strUnitedStateCities() As String

    Private Sub Window_Loaded(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs) Handles MyBase.Loaded

        ' Copy all Country enumeration values to a string array.
        Dim countries As Array = [Enum].GetValues(GetType(Countries))
        Dim strCountries(countries.Length) As String
        For i As Integer = 0 To countries.Length - 1
            strCountries(i) = DirectCast(countries, Countries())(i).ToString()
        Next

        ' Copy all ChinaCity enumeration values to a string array.
        Dim chinaCities As Array = [Enum].GetValues(GetType(ChinaCities))
        ReDim strChinaCities(chinaCities.Length)
        For i As Integer = 0 To chinaCities.Length - 1
            strChinaCities(i) = DirectCast(chinaCities, ChinaCities())(i).ToString()
        Next

        ' Copy all UnitedStateCity enumeration values to a string array.
        Dim unitedStateCities = [Enum].GetValues(GetType(UnitedStatesCities))
        ReDim strUnitedStateCities(unitedStateCities.Length)
        For i As Integer = 0 To unitedStateCities.Length - 1
            strUnitedStateCities(i) = DirectCast(unitedStateCities, UnitedStatesCities())(i).ToString()
        Next

        ' Combine both the two city enumeration value into one string array.
        Dim strAllCities(strChinaCities.Length + strUnitedStateCities.Length) As String
        strChinaCities.CopyTo(strAllCities, 0)
        strUnitedStateCities.CopyTo(strAllCities, strChinaCities.Length)

        ' Data bind the two DataGridComboBoxColumn's ItemsSource property respectively.
        BindingOperations.SetBinding(Me.column1, DataGridComboBoxColumn.ItemsSourceProperty, _
                                     New Binding() With {.Source = strCountries})
        BindingOperations.SetBinding(Me.column2, DataGridComboBoxColumn.ItemsSourceProperty, _
                                     New Binding() With {.Source = strAllCities})

        ' Create a DataTable and add two DataColumn into it.
        table = New DataTable()
        table.Columns.Add("Country")
        table.Columns.Add("City")

        ' Add a DataRow into this DataTable.
        table.Rows.Add(New Object() {"China", "Beijing"})

        ' Set the DataContext property of the DataGrid to the DataTable.
        Me.dataGrid.DataContext = table

        ' Set the Header of both DataGridComboBoxColumn and bind the SelectedItemBinding  
        ' property of both DataGridComboBoxColumn.
        Me.column1.Header = "Country"
        Me.column1.SelectedItemBinding = New Binding("Country") With {.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged}
        Me.column2.Header = "City"
        Me.column2.SelectedItemBinding = New Binding("City") With {.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged}
    End Sub

    ''' <summary>
    ''' This PreparingCellForEdit event handler gets the hosted editing ComboBox control 
    ''' and bind its ItemsSource property according to the value of the Country.
    ''' </summary>    
    Private Sub datagrid_PreparingCellForEdit(ByVal sender As Object, ByVal e As DataGridPreparingCellForEditEventArgs) Handles dataGrid.PreparingCellForEdit
        If e.Column.Header.Equals("City") Then
            Dim cmbEditingElement As ComboBox = CType(e.EditingElement, ComboBox)
            If CType(e.Row.Item, DataRowView)("Country").Equals("China") Then

                ' Bind the ItemsSource property of the cmbEditingElement to China cities
                ' string array if the selected country is China.
                BindingOperations.SetBinding(cmbEditingElement, ComboBox.ItemsSourceProperty, _
                                             New Binding() With {.Source = strChinaCities})
            ElseIf CType(e.Row.Item, DataRowView)("Country").Equals("UnitedStates") Then

                ' Bind the ItemsSource property of the cmbEditingElement to United State
                ' cities string array if the selected country is United State.
                BindingOperations.SetBinding(cmbEditingElement, ComboBox.ItemsSourceProperty, _
                                             New Binding() With {.Source = strUnitedStateCities})

            End If
        End If
    End Sub
End Class
