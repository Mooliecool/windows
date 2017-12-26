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
using System.Windows.Shapes;
using System.Windows.Navigation;

namespace CSWPFNavigationUsage
{
    public partial class MainPage : Page
    {
        public MainPage()
        {
            InitializeComponent();           
        }

        private void OnHyperlink(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("Page1.xaml", UriKind.Relative));
        }
      
        private void OnNavagateToObject(object sender, RoutedEventArgs e)
        {
            MyDummy obj = new MyDummy() { Property1 = "Hello", Property2 = "everyone" };
            this.NavigationService.Navigate(obj);
        }
       
        private void OnNavagateToPage(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("FramePage.xaml", UriKind.Relative));
        }
    }

    public class MyDummy
    {
        public string Property1 { get; set; }
        public string Property2 { get; set; }
    }
}
