Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window
        Public Sub ttNone(ByVal sender As Object, ByVal e As RoutedEventArgs)
            tf1.TextTrimming = System.Windows.TextTrimming.None
            txt1.Text = "TextTrimming is now " + tf1.TextTrimming.ToString()
        End Sub
        Public Sub ttCE(ByVal sender As Object, ByVal e As RoutedEventArgs)
            tf1.TextTrimming = System.Windows.TextTrimming.CharacterEllipsis
            txt1.Text = "TextTrimming is now " + tf1.TextTrimming.ToString()
        End Sub
        Public Sub ttWE(ByVal sender As Object, ByVal e As RoutedEventArgs)
            tf1.TextTrimming = System.Windows.TextTrimming.WordEllipsis
            txt1.Text = "TextTrimming is now " + tf1.TextTrimming.ToString()
        End Sub
    End Class
End Namespace
