Imports System ' Exception, EventArgs
Imports System.Threading ' Thread
Imports System.Windows ' Window, RoutedEventArgs
Imports System.Windows.Threading ' Dispatcher

Namespace SDKSample

    Partial Public Class SecondaryUiThreadWindow
        Inherits Window

        Public Sub New()

            InitializeComponent()

            Me.Title = String.Format("Running on Secondary UI Thread {0}", Thread.CurrentThread.ManagedThreadId)

        End Sub

        Private Sub raiseExceptionOnSecondaryUIThreadButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

            ' Raise an exception on the secondary UI thread
            Dim msg As String = String.Format("Exception raised on secondary UI thread {0}.", Dispatcher.CurrentDispatcher.Thread.ManagedThreadId)
            Throw New Exception(msg)

        End Sub

        Private Sub SecondaryUiThreadWindow_Closed(ByVal sender As Object, ByVal e As EventArgs)

            ' End this thread of execution
            Dispatcher.CurrentDispatcher.InvokeShutdown()

        End Sub

    End Class

End Namespace