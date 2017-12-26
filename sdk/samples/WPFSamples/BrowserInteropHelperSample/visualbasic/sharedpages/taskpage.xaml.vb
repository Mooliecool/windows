Imports System
Imports System.ComponentModel
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Navigation

Namespace SharedPages
    Public Class TaskPage
        Inherits PageFunction(Of TaskResult)

        Public Sub New(ByVal taskData As TaskData)
            Me.InitializeComponent()

            ' Bind task state to UI
            MyBase.DataContext = taskData

        End Sub

        Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Cancel the task and don't return any data
            Me.OnReturn(New ReturnEventArgs(Of TaskResult)(TaskResult.Canceled))
        End Sub

        Private Sub okButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Finish the task and return bound data to calling page
            Me.OnReturn(New ReturnEventArgs(Of TaskResult)(TaskResult.Finished))
        End Sub

    End Class
End Namespace


