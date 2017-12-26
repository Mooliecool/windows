Imports System.Data
Imports Microsoft.Windows.Controls

Class MainWindow
    Public Enum Country
        China
        UnitedStates
    End Enum
    Public Enum ChinaCity
        Beijing
        Shanghai
    End Enum
    Public Enum UnitedStatesCity
        NewYork
        Washington
    End Enum

    Dim table As DataTable = Nothing
    Dim strChinaCities(), strUnitedStateCities() As String

    Private Sub Window_Loaded(ByVal sender As System.Object, ByVal e As System.Windows.RoutedEventArgs) Handles MyBase.Loaded
        ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' get all enumeration values of type enum Country
        ''
        Dim countries As Array = [Enum].GetValues(GetType(Country))

        ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ''copy all Country enumeration values to a string array
        ''
        Dim strCountries(countries.Length) As String
        For i As Integer = 0 To countries.Length - 1
            strCountries(i) = DirectCast(countries, Country())(i).ToString()
        Next

        ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ''get all enumeration values of type enum ChinaCity
        ''
        Dim chinaCities As Array = [Enum].GetValues(GetType(ChinaCity))

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' copy all ChinaCity enumeration values to a string array
        ''
        ReDim strChinaCities(chinaCities.Length)
        For i As Integer = 0 To chinaCities.Length - 1
            strChinaCities(i) = DirectCast(chinaCities, ChinaCity())(i).ToString()
        Next

        ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' get all enumeration values of type enum UnitedStatesCity
        ''
        Dim unitedStateCities = [Enum].GetValues(GetType(UnitedStatesCity))

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' copy all UnitedStateCity enumeration values to a string array
        ''
        ReDim strUnitedStateCities(unitedStateCities.Length)
        For i As Integer = 0 To unitedStateCities.Length - 1
            strUnitedStateCities(i) = DirectCast(unitedStateCities, UnitedStatesCity())(i).ToString()
        Next
        ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' combine both the two city enumeration value into one string array
        ''
        Dim strAllCities(strChinaCities.Length + strUnitedStateCities.Length) As String
        strChinaCities.CopyTo(strAllCities, 0)
        strUnitedStateCities.CopyTo(strAllCities, strChinaCities.Length)

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ''data bind the two DataGridComboBoxColumn's ItemsSource property respectively
        ''
        BindingOperations.SetBinding(Me.column1, DataGridComboBoxColumn.ItemsSourceProperty, _
                                     New Binding() With {.Source = strCountries})
        BindingOperations.SetBinding(Me.column2, DataGridComboBoxColumn.ItemsSourceProperty, _
                                     New Binding() With {.Source = strAllCities})

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' create a DataTable and add two DataColumn into it
        ''
        table = New DataTable()
        table.Columns.Add("Country")
        table.Columns.Add("City")

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ''add a DataRow into this DataTable
        ''
        table.Rows.Add(New Object() {"China", "Beijing"})

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        '' set the DataContext property of the DataGrid to the DataTable
        ''
        Me.dataGrid.DataContext = table

        ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ''set the Header of both DataGridComboBoxColumn and bind the SelectedItemBinding  
        ''property of both DataGridComboBoxColumn
        ''
        Me.column1.Header = "Country"
        Me.column1.SelectedItemBinding = New Binding("Country")
        Me.column2.Header = "City"
        Me.column2.SelectedItemBinding = New Binding("City")
    End Sub

    ''' <summary>
    ''' this PreparingCellForEdit event handler gets the hosted editing ComboBox control 
    ''' and bind its ItemsSource property according to the value of the Country
    ''' </summary>    
    Private Sub datagrid_PreparingCellForEdit(ByVal sender As Object, ByVal e As DataGridPreparingCellForEditEventArgs) Handles dataGrid.PreparingCellForEdit
        If e.Column.Header.Equals("City") Then
            Dim cboEditingElement As ComboBox = CType(e.EditingElement, ComboBox)
            If CType(e.Row.Item, DataRowView)("Country").Equals("China") Then
                '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
                ''bind the ItemsSource property of the cmbEditingElement to China city
                ''string array if the selected country is China
                BindingOperations.SetBinding(cboEditingElement, ComboBox.ItemsSourceProperty, _
                                             New Binding() With {.Source = strChinaCities})
            Else
                ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
                ''bind the ItemsSource property of the cmbEditingElement to United State
                ''city string array if the selected country is United State
                BindingOperations.SetBinding(cboEditingElement, ComboBox.ItemsSourceProperty, _
                                             New Binding() With {.Source = strUnitedStateCities})

            End If
        End If
    End Sub
End Class
