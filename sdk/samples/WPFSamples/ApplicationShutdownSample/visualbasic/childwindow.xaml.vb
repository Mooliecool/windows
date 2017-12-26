Imports System
Imports System.ComponentModel
Imports System.Windows

Public Class ChildWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub ChildWindow_Closed(ByVal sender As Object, ByVal e As EventArgs)
        Console.WriteLine("Closed")
    End Sub

    Private Sub ChildWindow_Closing(ByVal sender As Object, ByVal e As CancelEventArgs)
        Console.WriteLine("Closing")
        Dim result As MessageBoxResult = MessageBox.Show("Allow Shutdown?", "Application Shutdown Sample", MessageBoxButton.YesNo, MessageBoxImage.Question)
        e.Cancel = (result = MessageBoxResult.No)
    End Sub

End Class