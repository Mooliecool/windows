Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Public Sub LR(ByVal sender As Object, ByVal e As RoutedEventArgs)
            tf1.FlowDirection = System.Windows.FlowDirection.LeftToRight
            txt1.Text = "FlowDirection is now " + tf1.FlowDirection.ToString()
        End Sub
        Public Sub Rl(ByVal sender As Object, ByVal e As RoutedEventArgs)
            tf1.FlowDirection = System.Windows.FlowDirection.RightToLeft
            txt1.Text = "FlowDirection is now " + tf1.FlowDirection.ToString()
        End Sub


    End Class
End Namespace
