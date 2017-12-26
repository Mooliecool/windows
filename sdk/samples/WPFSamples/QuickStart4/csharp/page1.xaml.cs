using System;
using System.Windows;
using System.Windows.Controls;

namespace QuickStart4
{
    public partial class Page1 : Page
    {
        void HandleClick(object sender, RoutedEventArgs e)
        {
            Button1.Content = "Hello World";
        }
    }
}