Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Navigation

Namespace NavWindow_Events_VB
  Partial Class MyWindow
    Inherits Page
    Public WithEvents my_App As app

    Private page1 As Boolean = False
    Private navWindow As NavigationWindow
    Private IsNavigate As Boolean = False

    Sub On_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
      Dim newPage As Page1

            If (sender.Equals(simplexaml)) Then
                displayFrame.Navigate(New System.Uri("Page1.xaml", UriKind.RelativeOrAbsolute))
            ElseIf (sender.Equals(simpleobject)) Then
                newPage = New Page1()
                newPage.InitializeComponent()
                displayFrame.Navigate(newPage)
            ElseIf (sender.Equals(frames)) Then
                displayFrame.Navigate(New System.Uri("Page2.xaml", UriKind.RelativeOrAbsolute))
            ElseIf (sender.Equals(large)) Then
                displayFrame.Navigate(New System.Uri("Content.xaml", UriKind.RelativeOrAbsolute))
            ElseIf (sender.Equals(fragment)) Then
                displayFrame.Navigate(New System.Uri("content.xaml#SomeTarget", UriKind.RelativeOrAbsolute))
            ElseIf (sender.Equals(forward)) Then
                If (navWindow.CanGoForward) Then
                    navWindow.GoForward()
                Else
                    MessageBox.Show("Nothing in the Forward stack")
                End If
            ElseIf (sender.Equals(back)) Then
                If (navWindow.CanGoBack) Then
                    navWindow.GoBack()
                Else
                    MessageBox.Show("Nothing in the Back stack")
                End If
            ElseIf (sender.Equals(stopLoading)) Then
                navWindow.StopLoading()
            ElseIf (sender.Equals(refresh)) Then
                navWindow.Refresh()
            End If
    End Sub

    Sub Init(ByVal sender As Object, ByVal e As RoutedEventArgs)
      my_App = CType(System.Windows.Application.Current, app)
      navWindow = CType(my_App.MainWindow, NavigationWindow)
      navWindow.SizeToContent = SizeToContent.WidthAndHeight
    End Sub

    Sub displayFrame_RequestNavigate(ByVal sender As Object, ByVal e As RequestNavigateEventArgs)
      Dim p0 As System.Windows.Controls.TextBlock
      Dim p1 As System.Windows.Controls.TextBlock
      Dim p2 As System.Windows.Controls.TextBlock
      Dim p3 As System.Windows.Controls.TextBlock

      eventText.Children.Clear()
      IsNavigate = True
      p0 = New System.Windows.Controls.TextBlock()
      p1 = New System.Windows.Controls.TextBlock()
      p1.FontWeight = FontWeights.Black
      p1.Text = "RequestNavigate"
      p2 = New System.Windows.Controls.TextBlock()
      p2.Text = "Target: " & e.Target.ToString()
      p3 = New System.Windows.Controls.TextBlock()
      If (e.Uri Is Nothing) Then
        p3.Text = "URI: None"
      Else
        p3.Text = "URI: " & e.Uri.ToString()
      End If
      eventText.Children.Add(p0)
      eventText.Children.Add(p1)
      eventText.Children.Add(p2)
      eventText.Children.Add(p3)
    End Sub

    Private Sub my_app_LoadCompleted(ByVal sender As Object, ByVal e As System.Windows.Navigation.NavigationEventArgs) Handles my_App.LoadCompleted
      Dim p0 As System.Windows.Controls.TextBlock
      Dim p1 As System.Windows.Controls.TextBlock
      Dim p2 As System.Windows.Controls.TextBlock
      Dim p3 As System.Windows.Controls.TextBlock
      Dim p4 As System.Windows.Controls.TextBlock

      p0 = New System.Windows.Controls.TextBlock()
      p1 = New System.Windows.Controls.TextBlock()
      p1.FontWeight = FontWeights.Black
      p1.Text = "LoadCompleted"
      p2 = New System.Windows.Controls.TextBlock()
      p2.Text = "Content: " & e.Content.ToString()
      p3 = New System.Windows.Controls.TextBlock()
      p3.Text = "Navigation Initiator: " & e.Navigator.ToString()
      p4 = New System.Windows.Controls.TextBlock()
      If (e.Uri Is Nothing) Then
        p4.Text = "URI: None"
      Else
        p4.Text = "URI: " & e.Uri.ToString()
      End If
      eventText.Children.Add(p0)
      eventText.Children.Add(p1)
      eventText.Children.Add(p2)
      eventText.Children.Add(p3)
      eventText.Children.Add(p4)
    End Sub

    Private Sub my_app_Navigated(ByVal sender As Object, ByVal e As System.Windows.Navigation.NavigationEventArgs) Handles my_App.Navigated
      Dim p0 As System.Windows.Controls.TextBlock
      Dim p1 As System.Windows.Controls.TextBlock
      Dim p2 As System.Windows.Controls.TextBlock
      Dim p3 As System.Windows.Controls.TextBlock
      Dim p4 As System.Windows.Controls.TextBlock

      p0 = New System.Windows.Controls.TextBlock()
      p1 = New System.Windows.Controls.TextBlock()
      p1.FontWeight = FontWeights.Black
      p1.Text = "Navigated"
      p2 = New System.Windows.Controls.TextBlock()
      p2.Text = "Content: " & e.Content.ToString()
      p3 = New System.Windows.Controls.TextBlock()
      p3.Text = "Navigation Initiator: " & e.Navigator.ToString()
      p4 = New System.Windows.Controls.TextBlock()
      If (e.Uri Is Nothing) Then
        p4.Text = "URI: None"
      Else
        p4.Text = "URI: " & e.Uri.ToString()
      End If
      eventText.Children.Add(p0)
      eventText.Children.Add(p1)
      eventText.Children.Add(p2)
      eventText.Children.Add(p3)
      eventText.Children.Add(p4)
    End Sub

    Private Sub my_app_Navigating(ByVal sender As Object, ByVal e As System.Windows.Navigation.NavigatingCancelEventArgs) Handles my_App.Navigating
      Dim p0 As System.Windows.Controls.TextBlock
      Dim p1 As System.Windows.Controls.TextBlock
      Dim p2 As System.Windows.Controls.TextBlock
      Dim p3 As System.Windows.Controls.TextBlock
      Dim p4 As System.Windows.Controls.TextBlock

      If (IsNavigate <> True) Then
        eventText.Children.Clear()
      Else
        IsNavigate = False
      End If
      p0 = New System.Windows.Controls.TextBlock()
      p1 = New System.Windows.Controls.TextBlock()
      p1.FontWeight = FontWeights.Bold
      p1.Text = "Navigating"
      p2 = New System.Windows.Controls.TextBlock()
      If (e.Content Is Nothing) Then
        p2.Text = "Content: null"
      Else
        p2.Text = "Content: " & e.Content.ToString()
      End If
      p3 = New System.Windows.Controls.TextBlock()
      p3.Text = "Navigation Mode: " & e.NavigationMode.ToString()
      p4 = New System.Windows.Controls.TextBlock()
      If (e.Uri Is Nothing) Then
        p4.Text = "URI: None"
      Else
        p4.Text = "URI: " & e.Uri.ToString()
      End If
      eventText.Children.Add(p1)
      eventText.Children.Add(p2)
      eventText.Children.Add(p3)
      eventText.Children.Add(p4)
    End Sub

    Private Sub my_app_NavigationProgress(ByVal sender As Object, ByVal e As System.Windows.Navigation.NavigationProgressEventArgs) Handles my_App.NavigationProgress
      Dim p0 As System.Windows.Controls.TextBlock
      Dim p1 As System.Windows.Controls.TextBlock
      Dim p2 As System.Windows.Controls.TextBlock
      Dim p3 As System.Windows.Controls.TextBlock
      Dim p4 As System.Windows.Controls.TextBlock

      p0 = New System.Windows.Controls.TextBlock()
      p1 = New System.Windows.Controls.TextBlock()
      p1.FontWeight = FontWeights.Bold
      p1.Text = "NavigationProgress"
      p2 = New System.Windows.Controls.TextBlock()
      p2.Text = "Bytes Read: " & e.BytesRead.ToString()
      p3 = New System.Windows.Controls.TextBlock()
      p3.Text = "MaxBytes: " & e.MaxBytes.ToString()
      p4 = New System.Windows.Controls.TextBlock()
      If (e.Uri Is Nothing) Then
        p4.Text = "URI: None"
      Else
        p4.Text = "URI: " & e.Uri.ToString()
      End If
      eventText.Children.Add(p0)
      eventText.Children.Add(p1)
      eventText.Children.Add(p2)
      eventText.Children.Add(p3)
      eventText.Children.Add(p4)
    End Sub
    Sub myApp_NavigationStopped(ByVal sender As Object, ByVal e As NavigationEventArgs)
      Dim p0 As System.Windows.Controls.TextBlock
      Dim p1 As System.Windows.Controls.TextBlock
      Dim p2 As System.Windows.Controls.TextBlock
      Dim p3 As System.Windows.Controls.TextBlock

      p0 = New System.Windows.Controls.TextBlock()
      p1 = New System.Windows.Controls.TextBlock()
      p1.FontWeight = FontWeights.Black
      p1.Text = "NavigationStopped"
      p2 = New System.Windows.Controls.TextBlock()
      If (e.Content Is Nothing) Then
        p2.Text = "Content: None"
      Else
        p2.Text = "Content: " & e.Content.ToString()
      End If
      p3 = New System.Windows.Controls.TextBlock()
      p3.Text = "Navigation Initiator: " & e.Navigator.ToString()
      eventText.Children.Add(p0)
      eventText.Children.Add(p1)
      eventText.Children.Add(p2)
      eventText.Children.Add(p3)
    End Sub

  End Class

End Namespace