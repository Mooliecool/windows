Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Navigation

Namespace SDKSample

    Partial Class Pane1

        Sub OnClick1(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt1.Text = "The FontFamily is set to Arial."
            txt2.FontFamily = New FontFamily("Arial")
        End Sub

        Sub OnClick2(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt1.Text = "The FontFamily is set to Courier New."
            txt2.FontFamily = New FontFamily("Courier New")
        End Sub

        Sub OnClick3(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt1.Text = "The FontFamily is set to Tahoma."
            txt2.FontFamily = New FontFamily("Tahoma")
        End Sub

        Sub OnClick4(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt1.Text = "The FontFamily is set to Times New Roman."
            txt2.FontFamily = New FontFamily("Times New Roman")
        End Sub

        Sub OnClick5(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt1.Text = "The FontFamily is set to Verdana."
            txt2.FontFamily = New FontFamily("Verdana")
        End Sub

        Sub OnClick6(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt3.Text = "The FontSize is set to 8 point."
            txt2.FontSize = 8
        End Sub

        Sub OnClick7(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt3.Text = "The FontSize is set to 10 point."
            txt2.FontSize = 10
        End Sub

        Sub OnClick8(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt3.Text = "The FontSize is set to 12 point."
            txt2.FontSize = 12
        End Sub

        Sub OnClick9(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt3.Text = "The FontSize is set to 14 point."
            txt2.FontSize = 14
        End Sub

        Sub OnClick10(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt3.Text = "The FontSize is set to 16 point."
            txt2.FontSize = 16
        End Sub

        Sub OnClick11(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt4.Text = "The Foreground color is set to Black."
            txt2.Foreground = Brushes.Black
        End Sub

        Sub OnClick12(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt4.Text = "The Foreground color is set to Blue."
            txt2.Foreground = Brushes.Blue
        End Sub

        Sub OnClick13(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt4.Text = "The Foreground color is set to Green."
            txt2.Foreground = Brushes.Green
        End Sub

        Sub OnClick14(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt4.Text = "The Foreground color is set to Red."
            txt2.Foreground = Brushes.Red
        End Sub

        Sub OnClick15(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            txt4.Text = "The Foreground color is set to Yellow."
            txt2.Foreground = Brushes.Yellow
        End Sub

    End Class

End Namespace