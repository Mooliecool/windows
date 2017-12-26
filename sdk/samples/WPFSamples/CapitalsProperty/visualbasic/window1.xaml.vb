' Interaction logic for Window1.xaml
Partial Public Class Window1
    Inherits Window

    Public Sub New()
        InitializeComponent()
    End Sub

    ' Sample event handlers:
    'Private Sub OnLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs) Handles Me.Loaded

    'End Sub

    ' Event handler for a Button with a Name of Button1
    'Private Sub ButtonClick(ByVal sender As Object, ByVal e As RoutedEventArgs) Handles Button1.Click

    'End Sub

    Sub Normal(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.Normal
        txt1.Text = "The Capitals property is currently set to Normal."

    End Sub

    Sub AllSmallCaps(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.AllSmallCaps
        txt1.Text = "The Capitals property is currently set to AllSmallCaps."

    End Sub

    Sub SmallCaps(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.SmallCaps
        txt1.Text = "The Capitals property is currently set to SmallCaps."

    End Sub

    Sub AllPetiteCaps(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.AllPetiteCaps
        txt1.Text = "The Capitals property is currently set to AllPetiteCaps."

    End Sub

    Sub PetiteCaps(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.PetiteCaps
        txt1.Text = "The Capitals property is currently set to PetiteCap."

    End Sub

    Sub Unicase(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.Unicase
        txt1.Text = "The Capitals property is currently set to Unicase."

    End Sub

    Sub Titling(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
        txt2.Typography.Capitals = System.Windows.FontCapitals.Titling
        txt1.Text = "The Capitals property is currently set to Titling."

    End Sub

End Class
