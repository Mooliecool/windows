' Interaction logic for SDKViewerWindow.xaml
Partial Public Class SDKViewerWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub homeHyperlink_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.sdkFrame.Navigate(New Uri("SDKFramePage.xaml", UriKind.Relative))
    End Sub

End Class
