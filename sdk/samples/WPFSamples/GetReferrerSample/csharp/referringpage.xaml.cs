using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace GetReferrerSample
{
    public partial class ReferringPage : Page
    {
        public ReferringPage()
        {
            InitializeComponent();
        }

        void Next_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("ReferredPage.xaml", UriKind.Relative));
        }
    }
}