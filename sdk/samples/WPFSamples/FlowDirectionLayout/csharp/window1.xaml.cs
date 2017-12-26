using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{

	public partial class Window1 : Window
	{
        public void LR(object sender, RoutedEventArgs e)
        {
            tf1.FlowDirection = FlowDirection.LeftToRight;
            txt1.Text = "FlowDirection is now " + tf1.FlowDirection.ToString();
        }
        public void RL(object sender, RoutedEventArgs e)
        {
            tf1.FlowDirection = FlowDirection.RightToLeft;
            txt1.Text = "FlowDirection is now " + tf1.FlowDirection.ToString();
        }
    }
}