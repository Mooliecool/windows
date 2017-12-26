//This is a list of commonly used namespaces for a window.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Data;
using System.Windows.Media;

namespace Microsoft.Samples.Graphics
{

    public partial class SampleViewer : Window
    {

        public SampleViewer()
        {
            InitializeComponent();
        }

        private void sampleSelected(object sender, RoutedEventArgs args)
        {
            RadioButton rButton = sender as RadioButton;

            if (rButton != null)
            {
            string buttonContent = (string)rButton.Content;

            if (buttonContent != null)
            {
            if (buttonContent == "Using DrawingBrush Example")
                mainFrame.Navigate(new DrawingBrushExample());
            else if (buttonContent == "Transform Example")
                mainFrame.Navigate(new TransformExample());
            else if (buttonContent == "ImageDrawing Example")
                mainFrame.Navigate(new ImageBrushExample());
            else
                mainFrame.Navigate(new AnimateGeometryDrawingExample());
        }
       }
     }

        private void exitApplication(object sender, RoutedEventArgs args)
        {
            Application.Current.Shutdown();
        }
    }
}