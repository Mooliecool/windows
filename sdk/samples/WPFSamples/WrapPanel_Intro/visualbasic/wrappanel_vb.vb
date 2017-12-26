Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Navigation

Namespace SDKSample

    Public Class WrapPanel_VB
        Inherits Page

        Public Sub New()
            WindowTitle = "WrapPanel Sample"

            ' Instantiate a new WrapPanel and set properties
            Dim myWrapPanel As New WrapPanel()
            myWrapPanel.Background = Brushes.Azure
            myWrapPanel.Orientation = Orientation.Horizontal
            myWrapPanel.ItemHeight = 25

            myWrapPanel.ItemWidth = 75
            myWrapPanel.Width = 150
            myWrapPanel.HorizontalAlignment = Windows.HorizontalAlignment.Left
            myWrapPanel.VerticalAlignment = Windows.VerticalAlignment.Top

            ' Define 3 button elements. Each button is sized at width of 75, so the third button wraps to the next line.
            Dim btn1 As New Button()
            btn1.Content = "Button 1"
            Dim btn2 As New Button()
            btn2.Content = "Button 2"
            Dim btn3 As New Button()
            btn3.Content = "Button 3"

            ' Add the buttons to the parent WrapPanel using the Children.Add method.
            myWrapPanel.Children.Add(btn1)
            myWrapPanel.Children.Add(btn2)
            myWrapPanel.Children.Add(btn3)

            ' Add the WrapPanel to the Page as Content
            Me.Content = myWrapPanel

        End Sub
    End Class
    'Displays the Sample
    Public Class app
        Inherits Application

        Protected Overrides Sub OnStartup(ByVal e As StartupEventArgs)
            MyBase.OnStartup(e)
            CreateAndShowMainWindow()
        End Sub

        Private Sub CreateAndShowMainWindow()
            ' Create the application's main window.
            Dim myWindow As New NavigationWindow()
            ' Display the sample
            Dim myContent As New WrapPanel_VB()
            myWindow.Navigate(myContent)
            MainWindow = myWindow
            myWindow.Show()
        End Sub
    End Class
    ' Starts the application.
    Public NotInheritable Class EntryClass

        Public Shared Sub Main()
            Dim app As New app()
            app.Run()
        End Sub
    End Class
End Namespace