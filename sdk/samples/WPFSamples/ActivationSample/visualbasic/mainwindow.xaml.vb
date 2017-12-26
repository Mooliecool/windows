' Interaction logic for MainWindow.xaml
Partial Public Class MainWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Public Sub AddMailItem(ByVal data As String)
        Me.mailListBox.Items.Add(data)
    End Sub

End Class
