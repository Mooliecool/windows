Public Class ReferringPage
    Inherits Page

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub Next_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        MyBase.NavigationService.Navigate(New Uri("ReferredPage.xaml", UriKind.Relative))
    End Sub

End Class
