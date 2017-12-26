Imports System
Imports System.ComponentModel
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation

Namespace SharedPages
    Public Class CallingPage
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Sub startTaskHyperlink_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Launch the task
            Dim taskLauncher As New TaskLauncher
            AddHandler taskLauncher.Return, New ReturnEventHandler(Of TaskContext)(AddressOf Me.task_Return)
            MyBase.NavigationService.Navigate(taskLauncher)
        End Sub

        ' Determine how the task completed and, if accepted, process the collected task state
        Public Sub task_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of TaskContext))

            ' Get task state
            Dim taskContext As TaskContext = e.Result

            Me.taskResultsTextBlock.Visibility = Windows.Visibility.Visible

            ' How did the task end?
            Me.taskStateTextBlock.Text = taskContext.Result.ToString

            ' If the task completed by being accepted, display task data
            If (taskContext.Result = TaskResult.Finished) Then
                Me.taskStateTextBlock.Text = (Me.taskStateTextBlock.Text & ChrW(10) & "Data Item: " & DirectCast(taskContext.Data, TaskData).DataItem)
            End If
        End Sub

    End Class
End Namespace


