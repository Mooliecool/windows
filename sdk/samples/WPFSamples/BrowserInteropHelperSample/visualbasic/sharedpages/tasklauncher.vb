Imports System
Imports System.Windows
Imports System.Windows.Interop
Imports System.Windows.Navigation

Namespace SharedPages
    Public Class TaskLauncher
        Inherits PageFunction(Of TaskContext)

        Public Sub New()
            Me.taskData = New TaskData
        End Sub

        Protected Overrides Sub Start()

            ' Retain instance (and state) in navigation history until task is complete
            MyBase.KeepAlive = True

            MyBase.WindowTitle = "Task Launcher"

            ' Is this assembly running in a browser-hosted application (XBAP)?
            If BrowserInteropHelper.IsBrowserHosted Then
                ' If so, use browser-style UI
                Me.UseTaskPageUI()
            Else
                ' If not, use window-style UI
                Me.UseTaskDialogBoxUI()
            End If

        End Sub

        Private Sub taskPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of TaskResult))
            ' Task was completed (finished or canceled), return TaskResult and TaskData
            Me.OnReturn(New ReturnEventArgs(Of TaskContext)(New TaskContext(e.Result, Me.taskData)))
        End Sub

        Private Sub UseTaskDialogBoxUI()

            ' Create and show dialog box
            Dim dialog As New TaskDialog(Me.taskData)
            dialog.Owner = Application.Current.MainWindow
            Dim dialogResult As Nullable(Of Boolean) = dialog.ShowDialog

            ' Return results
            Dim result As TaskResult = IIf(dialogResult.Value, TaskResult.Finished, TaskResult.Canceled)
            Me.OnReturn(New ReturnEventArgs(Of TaskContext)(New TaskContext(result, Me.taskData)))
        End Sub

        Private Sub UseTaskPageUI()

            ' Launch the task
            Dim page As New TaskPage(Me.taskData)
            AddHandler page.Return, New ReturnEventHandler(Of TaskResult)(AddressOf Me.taskPage_Return)
            MyBase.NavigationService.Navigate(page)

        End Sub

        Private taskData As TaskData

    End Class
End Namespace


