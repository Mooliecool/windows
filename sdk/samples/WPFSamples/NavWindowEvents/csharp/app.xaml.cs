using System;
using System.Windows;
using System.Windows.Navigation;
using System.Xml;
using System.Configuration;


namespace NavWindow_Events
{
  public partial class app : Application
  {
    MyWindow myWindow;
    NavigationWindow navWindow;

    private void Init(object sender, StartupEventArgs e)
    {
      navWindow = new NavigationWindow();
      myWindow = new MyWindow();
      myWindow.InitializeComponent();
      navWindow.Navigate(myWindow);
      navWindow.Visibility = Visibility.Visible;
    }
  }
}