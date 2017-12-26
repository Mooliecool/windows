using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Navigation;
using System.Windows.Media;

namespace SDKSample
{
	public partial class Page1 : Page
	{
        Application app;

        public void menuExit(object sender, RoutedEventArgs args)
        {
            app = (Application)System.Windows.Application.Current;
            app.Shutdown();
        }
        public void printPage(object sender, RoutedEventArgs args)
        {
            PrintDialog pDialog = new PrintDialog();
            pDialog.ShowDialog();
        }

        void onLoaded(object sender, EventArgs e)
        {
            todayDate.Text += System.DateTime.Now.ToString();

        }

        void nav1(object sender, MouseButtonEventArgs e)
        {
            frame1.Source = new Uri("document.xaml", UriKind.Relative);
        }

        void nav2(object sender, MouseButtonEventArgs e)
        {
            frame1.Source = new Uri("document1.xaml", UriKind.Relative);
        }
        void nav3(object sender, MouseButtonEventArgs e)
        {
            frame1.Source = new Uri("document2.xaml", UriKind.Relative);
        }

       
     }

}