Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Navigation

Namespace SDKSample

    Partial Class Pane1

        Private Sub Wrap(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt2.TextWrapping = System.Windows.TextWrapping.Wrap
            txt1.Text = "The TextWrap property is currently set to Wrap."
        End Sub

        Private Sub NoWrap(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt2.TextWrapping = System.Windows.TextWrapping.NoWrap
            txt1.Text = "The TextWrap property is currently set to NoWrap."
        End Sub

        Private Sub WrapWithOverflow(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt2.TextWrapping = System.Windows.TextWrapping.WrapWithOverflow
            txt1.Text = "The TextWrap property is currently set to WrapWithOverflow."
        End Sub

    End Class

End Namespace