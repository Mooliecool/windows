using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{
	public partial class Window1 : Window
	{
        public void ttNone(object sender, RoutedEventArgs e)
        {
            tf1.TextTrimming = System.Windows.TextTrimming.None;
            txt1.Text = "TextTrimming is now " + tf1.TextTrimming.ToString();
        }
        public void ttCE(object sender, RoutedEventArgs e)
        {
            tf1.TextTrimming = System.Windows.TextTrimming.CharacterEllipsis;
            txt1.Text = "TextTrimming is now " + tf1.TextTrimming.ToString();
        }
        public void ttWE(object sender, RoutedEventArgs e)
        {
            tf1.TextTrimming = System.Windows.TextTrimming.WordEllipsis;
            txt1.Text = "TextTrimming is now " + tf1.TextTrimming.ToString();
        }
    }
}