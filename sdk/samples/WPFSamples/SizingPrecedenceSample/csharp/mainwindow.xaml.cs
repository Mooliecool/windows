using System;
using System.Windows;

namespace SizingPrecedenceSampleCSharp
{
    public partial class MainWindow : System.Windows.Window
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        void showWindowButton_Click(object sender, RoutedEventArgs e)
        {
            // Configure and show SizingWindow as specified

            SizingWindow sw = new SizingWindow();

            if (this.setWSCB.IsChecked == true) sw.WindowState = (WindowState)Enum.Parse(typeof(WindowState), this.wsLB.Text);
            if (this.setMinWidthCB.IsChecked == true) sw.MinWidth = double.Parse(this.minWidthTB.Text);
            if (this.setMinHeightCB.IsChecked == true) sw.MinHeight = double.Parse(this.minHeightTB.Text);
            if (this.setMaxWidthCB.IsChecked == true) sw.MaxWidth = double.Parse(this.maxWidthTB.Text);
            if (this.setMaxHeightCB.IsChecked == true) sw.MaxHeight = double.Parse(this.maxHeightTB.Text);
            if (this.setWidthCB.IsChecked == true) sw.Width = double.Parse(this.widthTB.Text);
            if (this.setHeightCB.IsChecked == true) sw.Height = double.Parse(this.heightTB.Text);
            if (this.setSTCCB.IsChecked == true) sw.SizeToContent = (SizeToContent)Enum.Parse(typeof(SizeToContent), this.stcLB.Text);

            sw.Show();
        }
    }
}