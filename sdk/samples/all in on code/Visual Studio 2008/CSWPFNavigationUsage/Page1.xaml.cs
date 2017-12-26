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

namespace CSWPFNavigationUsage
{
    /// <summary>
    /// Interaction logic for Page1.xaml
    /// </summary>
    public partial class Page1 : Page
    {
        public Page1()
        {
            InitializeComponent();
            // Get a reference to the NavigationService that navigated to this Page
            NavigationService ns = NavigationService.GetNavigationService(this);
        }
        void hyperlink_Click(object sender, RoutedEventArgs e)
        {
            // Create a pack URI
            Uri uri = new Uri("AnotherPage.xaml", UriKind.Relative);
            // Get the navigation service that was used to 
            // navigate to this page, and navigate to 
            // AnotherPage.xaml
            this.NavigationService.Navigate(uri);
        }
    }
}
