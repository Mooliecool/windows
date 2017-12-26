using System; // Math
using System.Windows; // Window, RoutedEventArgs
using System.Windows.Controls.Primitives; // DragDeltaEventArgs
using System.Windows.Input; // MouseButtonEventArgs

namespace CustomChromeWindowSample
{
    public partial class CustomChromeWindow : Window
    {
        public CustomChromeWindow()
        {
            InitializeComponent();
        }

        void closeButtonRectangle_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.DragMove();
        }

        void resizeThumb_DragDelta(object sender, DragDeltaEventArgs e)
        {
            // Resize window width (honoring minimum width)
            int desiredWidth = (int)(this.ActualWidth + e.HorizontalChange);
            int minWidth = (int)(this.MinWidth + this.resizeThumb.Width + this.resizeThumb.Margin.Right);
            this.Width = Math.Max(desiredWidth, minWidth);

            // Resize window height (honoring minimum height)
            int desiredHeight = (int)(this.ActualHeight + e.VerticalChange);
            int minHeight = (int)(this.MinHeight + this.resizeThumb.Height + this.resizeThumb.Margin.Bottom);
            this.Height = Math.Max(desiredHeight, minHeight);
        }
    }
}