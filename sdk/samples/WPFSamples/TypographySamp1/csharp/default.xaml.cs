using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Typography_Samp
{
	/// <summary>
	/// Interaction logic for Default.xaml
	/// </summary>

	public partial class Page1 : Page
	{
        public void changeArial(object sender, RoutedEventArgs e)
        {
            tf1.FontFamily = new FontFamily("Arial");
        }

        public void changePalatino(object sender, RoutedEventArgs e)
        {
            tf1.FontFamily = new FontFamily("Palatino Linotype");
        }

        public void changeTimes(object sender, RoutedEventArgs e)
        {
            tf1.FontFamily = new FontFamily("Times New Roman");
        }

        public void changeVerdana(object sender, RoutedEventArgs e)
        {
            tf1.FontFamily = new FontFamily("Verdana");
        }
    }
}