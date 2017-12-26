Partial Public Class ErrorWindow
    Inherits ChildWindow
    Public Sub New()
        InitializeComponent()
    End Sub
    Public Sub New(ByVal errormsg As String)
        InitializeComponent()
        DataContext = errormsg
    End Sub

    Private Sub OKButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.DialogResult = True
    End Sub

    Private Sub CancelButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.DialogResult = False
    End Sub
End Class