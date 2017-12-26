Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Data

Namespace ExpenseIt

    Partial Public Class ExpenseReportPage
        Inherits Page


        Public Sub New(ByVal data As Object)

            Me.InitializeComponent()

            ' Bind to expense report data
            Me.DataContext = data

        End Sub

    End Class

End Namespace
