using System;
using System.ComponentModel;
using System.Windows;

namespace ApplicationShutdownSample
{
    public partial class ChildWindow : Window
    {
        public ChildWindow()
        {
            InitializeComponent();
        }

        void ChildWindow_Closing(object sender, CancelEventArgs e)
        {
            Console.WriteLine("Closing");
            MessageBoxResult result = MessageBox.Show("Allow Shutdown?", "Application Shutdown Sample", MessageBoxButton.YesNo, MessageBoxImage.Question);
            e.Cancel = (result == MessageBoxResult.No);
        }

        void ChildWindow_Closed(object sender, EventArgs e)
        {
            Console.WriteLine("Closed");
        }
    }
}