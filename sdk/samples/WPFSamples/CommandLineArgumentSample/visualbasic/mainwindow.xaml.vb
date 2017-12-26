Partial Public Class MainWindow
    Inherits System.Windows.Window

    Public Sub New()
        Me.InitializeComponent()

        ' Get command line args
        Dim key As String
        For Each key In App.CommandLineArgs.Keys
            Me.commandLineArgsListBox.Items.Add((key & ": " & App.CommandLineArgs.Item(key)))
        Next

    End Sub

End Class