using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Media;

namespace Guide
{
	public partial class Page1 : Page
	{
        Application app;

        public void menuExit(object sender, RoutedEventArgs args)
            {
                app = (Application)System.Windows.Application.Current;
                app.Shutdown();
            }

        public void menuAbout(object sender, RoutedEventArgs args)
        {
            frame2.Source = new Uri("about.xaml", UriKind.Relative);
        }
        public void launchXAMLPad(object sender, RoutedEventArgs args)
        {
            // This is the default installation path for XAMLPad in the Windows SDK.
            try
            {
                System.Diagnostics.Process.Start(@"C:\Program Files\Microsoft SDKs\Windows\v6.0\Bin\XAMLPad.exe");
            }
            catch (System.ComponentModel.Win32Exception)
            {
                MessageBox.Show("You must install the Windows SDK in order to run XAMLPad");
            }
        }

    }

}