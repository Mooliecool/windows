Imports System
Imports System.ComponentModel
Imports System.Windows
Imports System.Windows.Controls

Namespace SharedPages
    Public Class TaskDialog
        Inherits Window

        Public Sub New(ByVal taskData As TaskData)

            Me.InitializeComponent()

            ' Bind task state to UI
            MyBase.DataContext = taskData

        End Sub

        Private Sub okButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            MyBase.DialogResult = New Nullable(Of Boolean)(True)
        End Sub

    End Class
End Namespace

