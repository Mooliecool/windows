' Interaction logic for app.xaml
Partial Public Class app
    Inherits Application

    Sub AppStartup(ByVal sender As Object, ByVal e As StartupEventArgs) Handles Me.Startup
        Dim mainWindow As Window1 = New Window1
        mainWindow.Show()
    End Sub

End Class
