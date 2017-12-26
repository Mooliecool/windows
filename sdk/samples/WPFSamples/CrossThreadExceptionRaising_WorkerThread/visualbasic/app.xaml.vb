Imports System.Text ' StringBuilder
Imports System.Windows ' Application
Imports System.Windows.Threading ' DispatcherUnhandledException

Namespace SDKSample

    Partial Public Class App
        Inherits Application

        Private Sub App_DispatcherUnhandledException(ByVal sender As Object, ByVal e As DispatcherUnhandledExceptionEventArgs)

            ' Display exception message
            Dim sb As New StringBuilder
            sb.AppendFormat("{0}" & ChrW(10), e.Exception.InnerException.Message)
            sb.AppendFormat("{0}" & ChrW(10), e.Exception.Message)
            sb.AppendFormat("Exception handled on main UI thread {0}.", e.Dispatcher.Thread.ManagedThreadId)
            MessageBox.Show(sb.ToString)

            ' Keep application running in the face of this exception
            e.Handled = True

        End Sub

    End Class

End Namespace