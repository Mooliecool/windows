using System;
using System.Windows;

namespace DispatcherUnhandledExceptionSample
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
        void raiseRecoverableException_Click(object sender, RoutedEventArgs e)
        {
            throw new DivideByZeroException("Recoverable Exception");
        }

        void raiseUnecoverableException_Click(object sender, RoutedEventArgs e)
        {
            throw new ArgumentNullException("Unrecoverable Exception");
        }
    }
}