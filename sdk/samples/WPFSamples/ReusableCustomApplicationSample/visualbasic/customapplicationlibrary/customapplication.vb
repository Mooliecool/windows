Public Class CustomApplication
    Inherits Application

    Protected Overrides Sub OnStartup(ByVal e As System.Windows.StartupEventArgs)

        MyBase.OnStartup(e)

        MessageBox.Show("Hello, reusable custom application!")

    End Sub

End Class
