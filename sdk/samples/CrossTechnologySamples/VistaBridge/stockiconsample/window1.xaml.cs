using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.SDK.Samples.VistaBridge.Library.StockIcons;

namespace Microsoft.SDK.Samples.StockIconSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            image40.Source = StockIcons.MobilePC;
            image41.Source = StockIcons.Printer;
            StockIcon b42 = new StockIcon(StockIconIdentifier.AutoList);
            image42.Source = b42.Bitmap;
            StockIcon b43 = new StockIcon(StockIconIdentifier.ServerShare, StockIconOptions.Selected);
            image43.Source = b43.Bitmap;
        }

        
    }
}
