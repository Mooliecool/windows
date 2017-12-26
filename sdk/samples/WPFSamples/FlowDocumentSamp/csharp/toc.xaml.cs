using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Media;

namespace CustomTOC
{
	public partial class Page1 : Page
	{
        public void expandTOC(object sender, SelectionChangedEventArgs args)
        {
            if (node1.IsSelected==true & lb1.Visibility == System.Windows.Visibility.Collapsed)
            {
                lb1.Visibility = System.Windows.Visibility.Visible;
                node1.IsSelected = false;
            }
            else if (node1.IsSelected == true & lb1.Visibility == System.Windows.Visibility.Visible)
            {
                lb1.Visibility = System.Windows.Visibility.Collapsed;
                node1.IsSelected = false;
            }
            else if (node2.IsSelected == true & lb2.Visibility == System.Windows.Visibility.Collapsed)
            {
                lb2.Visibility = System.Windows.Visibility.Visible;
                node2.IsSelected = false;
            }
            else if (node2.IsSelected == true & lb2.Visibility == System.Windows.Visibility.Visible)
            {
                lb2.Visibility = System.Windows.Visibility.Collapsed;
                node2.IsSelected = false;
            }
        }

	}
}