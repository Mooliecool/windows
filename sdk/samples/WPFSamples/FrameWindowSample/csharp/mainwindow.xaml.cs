using System;
using System.Windows;

namespace FrameWindowSample
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
        
        void openSDKButton_Click(object sender, RoutedEventArgs e) {
            SDKViewerWindow window = new SDKViewerWindow();
            window.Show();
        }
    }
}