Imports System     
Imports System.Windows     
Imports System.Windows.Controls     
Imports System.Windows.Documents     

namespace Visibility_Layout_Samp

	'@ <summary>
	'@ Interaction logic for Window1.xaml
	'@ </summary>

    Partial Public Class Window1
        Inherits Window

        Private Sub contentVis(ByVal sender As Object, ByVal args As RoutedEventArgs)

            tb1.Visibility = System.Windows.Visibility.Visible
            txt1.Text = "Visibility is now set to Visible."
        End Sub

        Private Sub contentHid(ByVal sender As Object, ByVal args As RoutedEventArgs)

            tb1.Visibility = System.Windows.Visibility.Hidden
            txt1.Text = "Visibility is now set to Hidden. Notice that the TextBox still occupies layout space."
        End Sub

        Private Sub contentCol(ByVal sender As Object, ByVal args As RoutedEventArgs)

            tb1.Visibility = System.Windows.Visibility.Collapsed
            txt1.Text = "Visibility is now set to Collapsed. Notice that the TextBox no longer occupies layout space."
        End Sub

    End Class
    End Namespace
