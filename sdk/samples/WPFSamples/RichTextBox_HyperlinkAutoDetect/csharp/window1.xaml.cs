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


namespace HyperlinkAutoDetect
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
            StackPanel stackPanel = new StackPanel();
            MyRichTextBox richTextBox = new MyRichTextBox();
            stackPanel.Children.Add(richTextBox);
            this.Title = "Custom RichTextBox that auto-detects hyperlink strings while typing";
            this.Content = stackPanel;

        }

    }
}