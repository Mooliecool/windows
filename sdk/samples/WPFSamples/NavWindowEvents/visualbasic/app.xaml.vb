Imports System
Imports System.Windows
Imports System.Windows.Navigation

Namespace NavWindow_Events_VB
  Partial Class app
        Inherits Application
    Private my_Window As MyWindow
    Private navWindow As NavigationWindow

    Sub Init(ByVal Sender As Object, ByVal e As StartupEventArgs)
      navWindow = New NavigationWindow()
      my_Window = New MyWindow()
      my_Window.InitializeComponent()
      navWindow.Navigate(my_Window)
      navWindow.Visibility = Visibility.Visible
        End Sub
  End Class
End Namespace
