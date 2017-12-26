Imports System
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Navigation

' <summary>
' Stores an image in a way that can be stored with a page in navigation history.
' This is used to show page content snapshot in the navigation history combobox
' on the custom NavigationWindow navigation chrome.
' </summary>
<Serializable()> _
    Public Class ContentImageCustomContentState
    Inherits CustomContentState

    ' Methods
    Public Sub New(ByVal contentImage As BitmapSource)
        Me._contentImage = contentImage
    End Sub

    Public Shared Function GetContentImageCustomContentState(ByVal visual As Visual, ByVal width As Integer, ByVal height As Integer) As ContentImageCustomContentState
        Dim contentImage As New RenderTargetBitmap(width, height, 96, 96, New PixelFormat)
        contentImage.Render(visual)
        Return New ContentImageCustomContentState(contentImage)
    End Function

    Public Overrides Sub Replay(ByVal navigationService As NavigationService, ByVal mode As NavigationMode)
        ' Don't replay
    End Sub

    ' Properties
    Public ReadOnly Property ContentImage() As BitmapSource
        Get
            Return Me._contentImage
        End Get
    End Property


    ' Fields
    Private _contentImage As BitmapSource

End Class