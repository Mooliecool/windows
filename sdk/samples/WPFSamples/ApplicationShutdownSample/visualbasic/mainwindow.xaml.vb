Imports System
Imports System.Windows
Imports System.Windows.Controls

Public Class MainWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub explicitShutdownButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim exitCode As Integer = 0
        Integer.TryParse(Me.appExitCodeTextBox.Text, exitCode)
        Application.Current.Shutdown(exitCode)
    End Sub


    Private Sub MainWindow_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.shutdownModeListBox.Items.Add("OnLastWindowClose")
        Me.shutdownModeListBox.Items.Add("OnExplicitShutdown")
        Me.shutdownModeListBox.Items.Add("OnMainWindowClose")
        Me.shutdownModeListBox.SelectedValue = "OnLastWindowClose"
        AddHandler Me.shutdownModeListBox.SelectionChanged, New SelectionChangedEventHandler(AddressOf Me.shutdownModeListBox_SelectionChanged)
        Application.Current.ShutdownMode = ShutdownMode.OnLastWindowClose
    End Sub

    Private Sub newWindowButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        Dim window As ChildWindow = New ChildWindow
        window.Show()

    End Sub

    Private Sub shutdownModeListBox_SelectionChanged(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
        Application.Current.ShutdownMode = DirectCast(System.Enum.Parse(GetType(ShutdownMode), Me.shutdownModeListBox.SelectedValue.ToString), ShutdownMode)
    End Sub

End Class


