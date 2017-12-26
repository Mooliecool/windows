Imports System.ComponentModel

Partial Public Class CustomMediaPlayerWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub playButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Start media player
        Me.mediaElement.Play()
        Me.isMediaElementPlaying = True
    End Sub

    Private Sub stopButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Stop media player
        Me.mediaElement.Stop()
        Me.isMediaElementPlaying = False
    End Sub

    Private Sub window_Activated(ByVal sender As Object, ByVal e As EventArgs)
        ' Recommence playing media if window is activated
        If Me.isMediaElementPlaying Then
            Me.mediaElement.Play()
        End If
    End Sub

    Private Sub window_Deactivated(ByVal sender As Object, ByVal e As EventArgs)
        ' Pause playing if media is being played and window is deactivated
        If Me.isMediaElementPlaying Then
            Me.mediaElement.Pause()
        End If
    End Sub

    Private Sub exitMenu_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Close the window
        MyBase.Close()
    End Sub

    Private Sub window_Closing(ByVal sender As Object, ByVal e As CancelEventArgs)

        ' Ask user if they want to close the window
        If Me.isMediaElementPlaying Then

            Dim msg As String = "Media is playing. Really close?"
            Dim title As String = "Custom Media Player?"
            Dim buttons As MessageBoxButton = MessageBoxButton.YesNo
            Dim icon As MessageBoxImage = MessageBoxImage.Exclamation

            ' Show message box and get user's answer
            Dim result As MessageBoxResult = MessageBox.Show(msg, title, buttons, icon)

            ' Don't close window if user clicked No
            e.Cancel = (result = MessageBoxResult.No)

        End If

    End Sub


    Private isMediaElementPlaying As Boolean

End Class
