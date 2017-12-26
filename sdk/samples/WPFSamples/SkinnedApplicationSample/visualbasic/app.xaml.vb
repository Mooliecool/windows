Partial Public Class App
    Inherits System.Windows.Application

    Private Sub App_Startup(ByVal sender As Object, ByVal e As StartupEventArgs)
        MyBase.Properties.Item("Blue") = DirectCast(Application.LoadComponent(New Uri("BlueSkin.xaml", UriKind.Relative)), ResourceDictionary)
        MyBase.Properties.Item("Yellow") = DirectCast(Application.LoadComponent(New Uri("YellowSkin.xaml", UriKind.Relative)), ResourceDictionary)
    End Sub

End Class
