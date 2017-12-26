Imports system.windows.navigation

Public Class MainWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private ReadOnly Property NavigationService() As NavigationService
        Get
            Return NavigationService.GetNavigationService(DirectCast(Me.browserFrame.Content, DependencyObject))
        End Get
    End Property


    Private Sub goButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.navigatingEventsListBox.Items.Clear()
        Dim uri As New Uri(Me.addressTextBox.Text, UriKind.RelativeOrAbsolute)
        Me.NavigationService.Navigate(uri)
    End Sub

    Private Sub backButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.NavigationService.GoBack()
    End Sub

    Private Sub forwardButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.NavigationService.GoForward()
    End Sub

    Private Sub Log(ByVal item As String)
        Me.navigatingEventsListBox.Items.Add(item)
        Me.navigatingEventsListBox.SelectedIndex = (Me.navigatingEventsListBox.Items.Count - 1)
        Me.navigatingEventsListBox.Focus()
    End Sub

    Private Sub MainWindow_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        AddHandler Me.NavigationService.Navigating, AddressOf Me.NavigationService_Navigating
        AddHandler Me.NavigationService.Navigated, AddressOf Me.NavigationService_Navigated
        AddHandler Me.NavigationService.NavigationProgress, AddressOf Me.NavigationService_NavigationProgress
        AddHandler Me.NavigationService.NavigationStopped, AddressOf Me.NavigationService_NavigationStopped
        AddHandler Me.NavigationService.NavigationFailed, AddressOf Me.NavigationService_NavigationFailed
        AddHandler Me.NavigationService.LoadCompleted, AddressOf Me.NavigationService_LoadCompleted
        AddHandler Me.NavigationService.FragmentNavigation, AddressOf Me.NavigationService_FragmentNavigation
        Me.backButton.IsEnabled = Me.NavigationService.CanGoBack
        Me.forwardButton.IsEnabled = Me.NavigationService.CanGoForward
    End Sub

#Region "Navigation Events"
    Private Sub NavigationService_LoadCompleted(ByVal sender As Object, ByVal e As NavigationEventArgs)
        Me.Log(("Load Completed: [" & e.Uri.ToString() & "]"))
        Me.backButton.IsEnabled = Me.NavigationService.CanGoBack
        Me.forwardButton.IsEnabled = Me.NavigationService.CanGoForward
    End Sub
    Private Sub NavigationService_Navigated(ByVal sender As Object, ByVal e As NavigationEventArgs)
        Me.Log(("Navigated: [" & e.Uri.ToString() & "]"))
    End Sub
    Private Sub NavigationService_Navigating(ByVal sender As Object, ByVal e As NavigatingCancelEventArgs)
        Me.Log(("Navigating: [" & e.Uri.ToString() & "]"))
    End Sub
    Private Sub NavigationService_NavigationProgress(ByVal sender As Object, ByVal e As NavigationProgressEventArgs)
        Me.Log(String.Concat(New Object() {"Progress: ", e.BytesRead.ToString, " of ", e.MaxBytes.ToString, " [", e.Uri, "]"}))
    End Sub
    Private Sub NavigationService_NavigationFailed(ByVal sender As Object, ByVal e As NavigationFailedEventArgs)
        Me.Log(("Navigation Stopped: [" & e.Uri.ToString() & " - " + e.Exception.Message + "]"))
    End Sub
    Private Sub NavigationService_NavigationStopped(ByVal sender As Object, ByVal e As NavigationEventArgs)
        Me.Log(("Navigation Stopped: [" & e.Uri.ToString() & "]"))
    End Sub
    Private Sub NavigationService_FragmentNavigation(ByVal sender As Object, ByVal e As FragmentNavigationEventArgs)
        Me.Log(("Navigation Stopped: [" & e.Fragment.ToString() & "]"))
    End Sub
#End Region

    Private Sub refreshButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.navigatingEventsListBox.Items.Clear()
        Me.NavigationService.Refresh()
    End Sub

    Private Sub stopButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.NavigationService.StopLoading()
    End Sub

End Class