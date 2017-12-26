
Namespace SDKSample

    Partial Public Class MainWindow
        Inherits Window

        Private Shared tabPageCount As Integer = 1

        Public Sub New()
            InitializeComponent()
            AddBrowserTab()
        End Sub

        Private Sub fileAddTabMenuItem_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            AddBrowserTab()
        End Sub

        Private Sub fileRemoveTabMenuItem_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            RemoveBrowserTab()
        End Sub

        Private Sub fileExitMenuItem_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Me.Close()
        End Sub

        Private Sub AddBrowserTab()

            Dim ti As New TabItem
            ti.Header = String.Format("Tab {0}", MainWindow.tabPageCount)
            Dim frame As New Frame
            frame.Source = New Uri("Page1.xaml", UriKind.Relative)
            frame.NavigationUIVisibility = NavigationUIVisibility.Hidden
            ti.Content = frame
            Me.browserPagesTabControl.Items.Add(ti)

            Me.browserPagesTabControl.SelectedItem = ti

            MainWindow.tabPageCount += 1

        End Sub

        Private Sub RemoveBrowserTab()
            Dim currentTi As TabItem = TryCast(Me.browserPagesTabControl.SelectedItem, TabItem)
            Me.browserPagesTabControl.Items.Remove(currentTi)
        End Sub

    End Class

End Namespace
