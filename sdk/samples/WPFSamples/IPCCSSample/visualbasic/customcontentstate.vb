Imports System
Imports System.Windows.Navigation

<Serializable()> _
Public Class CustomPageContentState
    Inherits CustomContentState

    Public Sub New(ByVal verticalOffset As Double, ByVal readCount As Integer)

        ' "Remember" vertical offset and read count
        Me.verticalOffset = verticalOffset
        Me.readCount = readCount

        ' Note: The Value of the zoom scrollbar is automatically saved because
        ' the dependency property has the Journal metadata flag, and the control is
        ' placed in the primary logical tree.
        ' Neither ScrollViewer nor Label, however, is not automatically journaled.
    End Sub

    Public Overrides Sub Replay(ByVal navigationService As NavigationService, ByVal mode As NavigationMode)

        ' Apply previous scroll offset
        DirectCast(navigationService.Content, HomePage).documentScrollViewer.ScrollToVerticalOffset(Me.verticalOffset)

        ' Increment previous read count and apply
        Me.readCount += 1
        DirectCast(navigationService.Content, HomePage).readCountLabel.Content = Me.readCount

    End Sub


    Private readCount As Integer
    Private verticalOffset As Double

End Class