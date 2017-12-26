Imports System.IO
Imports System.Windows.Navigation

Imports Microsoft.Win32

Public Class HomePage
    Inherits Page

    Implements IProvideCustomContentState

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Public Function GetContentState() As CustomContentState Implements IProvideCustomContentState.GetContentState
        ' Add to history that is in the opposite direction to which we are navigating
        Return New ImageCustomContentState(Me.viewImage.Source, CStr(Me.nameLabel.Content))
    End Function

    Private Sub uploadButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' Configure OpenFileDialog to open images
        Dim dlg As New OpenFileDialog
        dlg.Filter = "Image Files(*.BMP;*.JPG;*.GIF)|*.BMP;*.JPG;*.GIF|All files (*.*)|*.*"

        ' Open file, if user clicked "Open" button on OpenFileDialog
        Dim dialogResult As Nullable(Of Boolean) = dlg.ShowDialog
        If (dialogResult.GetValueOrDefault AndAlso dialogResult.HasValue) Then

            ' If existing image, put into back navigation history
            If (Not Me.viewImage.Source Is Nothing) Then
                Dim state As New ImageCustomContentState(Me.viewImage.Source, CStr(Me.nameLabel.Content))
                MyBase.NavigationService.AddBackEntry(state)
            End If

            ' Show new image
            Using stream As Stream = dlg.OpenFile
                Dim decoder As BitmapDecoder = BitmapDecoder.Create(stream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad)
                Me.viewImage.Source = decoder.Frames.Item(0)
                Me.nameLabel.Content = dlg.SafeFileName
            End Using

        End If

    End Sub

End Class