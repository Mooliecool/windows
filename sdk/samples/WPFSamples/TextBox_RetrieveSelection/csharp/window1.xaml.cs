using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{
 
    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        void OnClick(object sender, RoutedEventArgs e)
        {
            String sSelectedText = tbSelectSomeText.SelectedText;
            tbDisplaySelectedText.Text = tbSelectSomeText.SelectedText;
        }

    }
}