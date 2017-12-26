' Interaction logic for MainWindow.xaml
Partial Public Class MainWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub openSDKButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim window As SDKViewerWindow = New SDKViewerWindow
        window.Show()
    End Sub

End Class
