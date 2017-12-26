Public Class Page2
    Inherits Page

    Implements IProvideCustomContentState

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Function GetContentState() As CustomContentState Implements IProvideCustomContentState.GetContentState
        Return ContentImageCustomContentState.GetContentImageCustomContentState(Me, CInt(MyBase.ActualWidth), CInt(MyBase.ActualHeight))
    End Function

End Class