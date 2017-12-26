using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace Visibility_Layout_Samp
{
	/// <summary>
	/// Interaction logic for Window1.xaml
	/// </summary>

	public partial class Window1 : Window
	{
        private void contentVis(object sender, RoutedEventArgs e)
        {
            tb1.Visibility = System.Windows.Visibility.Visible;
            txt1.Text = "Visibility is now set to Visible.";
        }

        private void contentHid(object sender, RoutedEventArgs e)
        {
            tb1.Visibility = System.Windows.Visibility.Hidden;
            txt1.Text = "Visibility is now set to Hidden. Notice that the TextBox still occupies layout space.";
        }

        private void contentCol(object sender, RoutedEventArgs e)
        {
            tb1.Visibility = System.Windows.Visibility.Collapsed;
            txt1.Text = "Visibility is now set to Collapsed. Notice that the TextBox no longer occupies layout space.";
        }

    }
}