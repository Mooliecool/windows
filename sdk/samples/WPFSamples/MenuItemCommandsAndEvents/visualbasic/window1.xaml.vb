' Interaction logic for Window1.xaml
Partial Public Class Window1
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub


    Private Sub Bold_Checked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        textBox1.FontWeight = FontWeights.Bold
    End Sub

    Private Sub Bold_Unchecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        textBox1.FontWeight = FontWeights.Normal
    End Sub

    Private Sub Italic_Checked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        textBox1.FontStyle = FontStyles.Italic
    End Sub

    Private Sub Italic_Unchecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
        textBox1.FontStyle = FontStyles.Normal
    End Sub

    Private Sub IncreaseFont_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        If textBox1.FontSize < 18 Then
            textBox1.FontSize += 2
        End If
    End Sub

    Private Sub DecreaseFont_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        If textBox1.FontSize > 10 Then
            textBox1.FontSize -= 2
        End If
    End Sub
End Class
