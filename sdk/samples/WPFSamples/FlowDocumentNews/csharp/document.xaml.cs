using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Media;

namespace document1
{
	public partial class DockPanel1 : DockPanel
	{

        public void colWidest(object sender, RoutedEventArgs args)
        {
            fd1.ColumnWidth = 150;
        }
        public void colMore(object sender, RoutedEventArgs args)
        {
            fd1.ColumnWidth = 250;
        }
        public void colAverage(object sender, RoutedEventArgs args)
        {
            fd1.ColumnWidth = 350;
        }
        public void colFewer(object sender, RoutedEventArgs args)
        {
            fd1.ColumnWidth = 450;
        }
        public void colLeast(object sender, RoutedEventArgs args)
        {
            fd1.ColumnWidth = 550;
        }

        public void textLargest(object sender, RoutedEventArgs args)
        {
            fd1.FontSize = 18;
        }
        public void textLarge(object sender, RoutedEventArgs args)
        {
            fd1.FontSize = 15;
        }
        public void textAverage(object sender, RoutedEventArgs args)
        {
            fd1.FontSize = 12;
        }
        public void textSmall(object sender, RoutedEventArgs args)
        {
            fd1.FontSize = 10;
        }
        public void textSmallest(object sender, RoutedEventArgs args)
        {
            fd1.FontSize = 8;
        }

     }

}