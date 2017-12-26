Imports System.Windows.Threading

Public Class App
    Inherits Application

    Private Sub App_DispatcherUnhandledException(ByVal sender As Object, ByVal e As DispatcherUnhandledExceptionEventArgs)

        ' Process unhandled exception
        Dim shutdown As Boolean = False

        ' Process exception
        If TypeOf e.Exception Is DivideByZeroException Then
            ' Recoverable - continue processing
            shutdown = False
        ElseIf TypeOf e.Exception Is ArgumentNullException Then
            ' Unrecoverable - end processing
            shutdown = True
        End If

        If shutdown Then

            ' If unrecoverable, attempt to save data
            Dim result As MessageBoxResult = MessageBox.Show(("Application must exit:" & ChrW(10) & ChrW(10) & e.Exception.Message & ChrW(10) & ChrW(10) & "Save before exit?"), "app", MessageBoxButton.YesNo, MessageBoxImage.Hand)
            If (result = MessageBoxResult.Yes) Then
                ' Save data
            End If

            ' Add entry to event log
            EventLog.WriteEntry("app", ("Unrecoverable Exception: " & e.Exception.Message), EventLogEntryType.Error)

            ' Return exit code
            MyBase.Shutdown(-1)

        End If

        ' Prevent default unhandled exception processing
        e.Handled = True

    End Sub

End Class
