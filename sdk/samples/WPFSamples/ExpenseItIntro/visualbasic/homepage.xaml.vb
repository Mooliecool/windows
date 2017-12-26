Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Data

Namespace ExpenseIt

    Partial Public Class HomePage
        Inherits Page

        Private Sub viewButton_Click(ByVal sender As Object, ByVal args As RoutedEventArgs)

            ' View Expense Report
            Dim expenseReportPage As ExpenseReportPage = New ExpenseReportPage(Me.peopleListBox.SelectedItem)
            Me.NavigationService.Navigate(expenseReportPage)

        End Sub
    End Class
End Namespace
