Public Class MainWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub raiseRecoverableException_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Throw New DivideByZeroException("Recoverable Exception")
    End Sub

    Private Sub raiseUnecoverableException_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Throw New ArgumentNullException("Unrecoverable Exception")
    End Sub

End Class
