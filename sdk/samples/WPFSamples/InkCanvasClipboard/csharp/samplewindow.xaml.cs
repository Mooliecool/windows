using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace InkCanvasClipboardSample
{
    /// <summary>
    /// Interaction logic for SampleWindow.xaml
    /// </summary>

    public partial class SampleWindow : System.Windows.Window
    {

        public SampleWindow()
        {
            InitializeComponent();
            miInk.IsChecked = true;
        }

        #region Action Menu Handlers
        /// <summary>
        /// clears strokes and elements from the InkCanvas
        /// </summary>
        void OnClear(object sender, RoutedEventArgs e)
        {
            inkCanvas.Strokes.Clear();
            inkCanvas.Children.Clear();
        }

        /// <summary>
        /// closes the application
        /// </summary>
        void OnExit(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        #endregion

        #region Edit Menu Handlers
        /// <summary>
        /// pastes clipboard content into the center
        /// of the InkCanvas
        /// </summary>
        void OnPasteInCenter(object sender, RoutedEventArgs e)
        {
            Point center = new Point(inkCanvas.ActualWidth / 2d, inkCanvas.ActualHeight / 2d);
            inkCanvas.Paste(center);
        }

        /// <summary>
        /// switches InkCanvas to selection mode
        /// </summary>
        void OnSelectChecked(object sender, RoutedEventArgs e)
        {
            miInk.IsChecked = false;
            inkCanvas.EditingMode = InkCanvasEditingMode.Select;
        }

        /// <summary>
        /// switches InkCanvas to inking mode
        /// </summary>
        void OnInkChecked(object sender, RoutedEventArgs e)
        {
            miSelect.IsChecked = false;
            inkCanvas.EditingMode = InkCanvasEditingMode.Ink;
        }
        #endregion

        #region PasteFormat Menu Handler
        /// <summary>
        /// updates InkCanvas preferred paste formats
        /// </summary>
        void OnPasteFormatChanged(object sender, RoutedEventArgs e)
        {
            List<InkCanvasClipboardFormat> preferredPasteFormats = new List<InkCanvasClipboardFormat>();

            if (miXaml.IsChecked)
            {
                preferredPasteFormats.Add(InkCanvasClipboardFormat.Xaml);
            }
            if (miIsf.IsChecked)
            {
                preferredPasteFormats.Add(InkCanvasClipboardFormat.InkSerializedFormat);
            }
            if (miText.IsChecked)
            {
                preferredPasteFormats.Add(InkCanvasClipboardFormat.Text);
            }

            inkCanvas.PreferredPasteFormats = preferredPasteFormats;
        }
        #endregion

        #region Insert Menu Handlers
        /// <summary>
        /// inserts a new TextBox into InkCanvas
        /// </summary>
        void OnInsertTextBox(object sender, RoutedEventArgs e)
        {
            TextBox textBoxToInsert = new TextBox();
            textBoxToInsert.Text = "New TextBox";
            textBoxToInsert.AcceptsReturn = true;
            textBoxToInsert.SetValue(InkCanvas.TopProperty, inkCanvas.ActualHeight / 2d);
            textBoxToInsert.SetValue(InkCanvas.LeftProperty, inkCanvas.ActualWidth / 2d);
            inkCanvas.Children.Add(textBoxToInsert);
        }

        /// <summary>
        /// inserts a new Label into InkCanvas
        /// </summary>
        void OnInsertLabel(object sender, RoutedEventArgs e)
        {
            Label labelToInsert = new Label();
            labelToInsert.Content = "New Label";
            labelToInsert.Background = Brushes.LightBlue;
            labelToInsert.SetValue(InkCanvas.TopProperty, inkCanvas.ActualHeight / 2d);
            labelToInsert.SetValue(InkCanvas.LeftProperty, inkCanvas.ActualWidth / 2d);
            inkCanvas.Children.Add(labelToInsert);
        }

        /// <summary>
        /// inserts a new Ellipse shape into InkCanvas
        /// </summary>
        void OnInsertShape(object sender, RoutedEventArgs e)
        {
            Ellipse ellipseToInsert = new Ellipse();
            ellipseToInsert.Width = 150d;
            ellipseToInsert.Height = 100d;
            ellipseToInsert.Fill = new LinearGradientBrush(Colors.Goldenrod, Colors.HotPink, 20d);
            ellipseToInsert.SetValue(InkCanvas.TopProperty, inkCanvas.ActualHeight / 2d);
            ellipseToInsert.SetValue(InkCanvas.LeftProperty, inkCanvas.ActualWidth / 2d);
            inkCanvas.Children.Add(ellipseToInsert);
        }
        #endregion
    }
}