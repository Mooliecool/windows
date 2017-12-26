Imports System
Imports System.Windows.Media
Imports System.Windows.Navigation

<Serializable()> _
    Public Class ImageCustomContentState
    Inherits CustomContentState

    Public Sub New(ByVal imageSource As ImageSource, ByVal filename As String)
        Me.imageSource = imageSource
        Me.filename = filename
    End Sub

    Public Overrides Sub Replay(ByVal navigationService As NavigationService, ByVal mode As NavigationMode)
        Dim homePage As HomePage = DirectCast(navigationService.Content, HomePage)
        homePage.viewImage.Source = Me.imageSource
        homePage.nameLabel.Content = Me.filename
    End Sub

    Public Overrides ReadOnly Property JournalEntryName() As String
        Get
            Return Me.filename
        End Get
    End Property

    Private filename As String
    Private imageSource As ImageSource

End Class



