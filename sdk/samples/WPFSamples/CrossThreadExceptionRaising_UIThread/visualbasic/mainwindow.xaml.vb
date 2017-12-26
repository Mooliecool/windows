Imports System.Threading ' Thread, ApartmentState
Imports System.Windows ' Window, RoutedEventArgs
Imports System.Windows.Threading ' Dispatcher, DispatcherPriority

Namespace SDKSample

    Partial Public Class MainWindow
        Inherits Window

        ' Data to send back to main UI thread
        Class ExceptionData
            Public secondaryUIThreadId As Integer
            Public secondaryUIThreadException As Exception
        End Class

        Public Sub New()

            InitializeComponent()

            MyBase.Title = String.Format("Running on Main UI Thread {0}", Thread.CurrentThread.ManagedThreadId)

        End Sub

        ' THIS EVENT HANDLER RUNS ON THE MAIN UI THREAD
        Private Sub startSecondaryUIThreadButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

            ' Creates and starts a secondary thread in a single threaded apartment (STA)
            Dim thread As New Thread(New ThreadStart(AddressOf Me.MethodRunningOnSecondaryUIThread))
            thread.SetApartmentState(ApartmentState.STA)
            thread.IsBackground = True
            thread.Start()

        End Sub

        ' THIS METHOD RUNS ON A SECONDARY UI THREAD (THREAD WITH A DISPATCHER)
        Private Sub MethodRunningOnSecondaryUIThread()

            Try

                ' On secondary thread, show a new Window before starting a new Dispatcher
                ' ie turn secondary thread into a UI thread
                Dim window As New SecondaryUiThreadWindow
                window.Show()
                Dispatcher.Run()

            Catch ex As Exception

                ' Dispatch the exception back to the main UI thread and reraise it
                Dim secondaryUIThreadId As Integer = Thread.CurrentThread.ManagedThreadId
                Dim exceptionData As New ExceptionData
                exceptionData.secondaryUIThreadId = Thread.CurrentThread.ManagedThreadId
                exceptionData.secondaryUIThreadException = ex
                Application.Current.Dispatcher.Invoke(DispatcherPriority.Send, New DispatcherOperationCallback(AddressOf DispatchExceptionToMainUIThread), exceptionData)

            End Try


        End Sub

        ' THIS CODE RUNS BACK ON THE MAIN UI THREAD
        Private Function DispatchExceptionToMainUIThread(ByVal arg As Object) As Object
            Dim exceptionData As ExceptionData = arg
            Throw New Exception(String.Format("Exception forwarded from secondary UI thread {0}.", exceptionData.secondaryUIThreadId), exceptionData.secondaryUIThreadException)
        End Function

    End Class

End Namespace