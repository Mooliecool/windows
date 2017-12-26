' Interaction logic for MainWindow.xaml
Partial Public Class MainWindow
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub showMessageBoxButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' Configure the message box
        Dim result As MessageBoxResult
        Dim owner As Window = IIf(Me.ownerCheckBox.IsChecked.Value, Me, Nothing)
        Dim messageBoxText As String = Me.messageBoxText.Text
        Dim caption As String = Me.caption.Text
        Dim button As MessageBoxButton = DirectCast(System.Enum.Parse(GetType(MessageBoxButton), Me.buttonComboBox.Text), MessageBoxButton)
        Dim icon As MessageBoxImage = DirectCast(System.Enum.Parse(GetType(MessageBoxImage), Me.imageComboBox.Text), MessageBoxImage)
        Dim defaultResult As MessageBoxResult = DirectCast(System.Enum.Parse(GetType(MessageBoxResult), Me.defaultResultComboBox.Text), MessageBoxResult)
        Dim options As MessageBoxOptions = DirectCast(System.Enum.Parse(GetType(MessageBoxOptions), Me.optionsComboBox.Text), MessageBoxOptions)

        ' Show message box, passing the window owner if specified
        If (owner Is Nothing) Then
            result = MessageBox.Show(messageBoxText, caption, button, icon, defaultResult, options)
        Else
            result = MessageBox.Show(owner, messageBoxText, caption, button, icon, defaultResult, options)
        End If

        ' Show the result
        Me.resultTextBlock.Text = ("Result = " & result.ToString)

    End Sub

End Class
