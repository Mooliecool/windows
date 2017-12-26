Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Input
Imports System.Windows.Media


Namespace SDKSample

    Partial Public Class Page1
        Inherits Page

        Dim app As Application

        Public Sub menuExit(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            app = CType(System.Windows.Application.Current, Application)
            app.Shutdown()
        End Sub
        Public Sub printPage(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            Dim pDialog As New PrintDialog
            pDialog.ShowDialog()
        End Sub

        Public Sub onLoaded(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            todayDate.Text += System.DateTime.Now.ToString()

        End Sub

        Public Sub nav1(ByVal Sender As Object, ByVal e As MouseButtonEventArgs)

            frame1.Source = New Uri("document.xaml", UriKind.Relative)
        End Sub

        Public Sub nav2(ByVal Sender As Object, ByVal e As MouseButtonEventArgs)

            frame1.Source = New Uri("document1.xaml", UriKind.Relative)
        End Sub
        Public Sub nav3(ByVal Sender As Object, ByVal e As MouseButtonEventArgs)

            frame1.Source = New Uri("document2.xaml", UriKind.Relative)
        End Sub
    End Class
End Namespace
