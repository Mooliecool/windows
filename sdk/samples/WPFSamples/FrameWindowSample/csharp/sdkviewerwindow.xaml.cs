using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace FrameWindowSample
{
    /// <summary>
    /// Interaction logic for SDKViewerWindow.xaml
    /// </summary>

    public partial class SDKViewerWindow : Window
    {

        public SDKViewerWindow()
        {
            InitializeComponent();
        }
        
        // Navigate to home page
        void homeHyperlink_Click(object sender, RoutedEventArgs e) {
            this.sdkFrame.Navigate(new Uri("SDKFramePage.xaml", UriKind.Relative));
        }

    }
}