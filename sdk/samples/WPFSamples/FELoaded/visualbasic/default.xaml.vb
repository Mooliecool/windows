Imports System
Imports System.Windows
Imports System.Windows.Controls

Namespace SDKSample 
    Public Partial Class FELoaded
        Private Sub OnLoad(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim b1 As Button = New Button()
            b1.Content = "New Button"
            root.Children.Add(b1)
            b1.Height = 25
            b1.Width = 200
            b1.HorizontalAlignment = HorizontalAlignment.Left
        End Sub
    End Class
End Namespace
