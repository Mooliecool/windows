using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace InkColorPicker
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : System.Windows.Window
    {
        ColorPicker _colorPicker;
        private DrawingAttributes _markerDA, _highlightDA;

        /// <summary>
        /// Initialization.
        /// </summary>
        public Window1()
        {
            InitializeComponent();

            inkcanvas.PreferredPasteFormats = new InkCanvasClipboardFormat[] { InkCanvasClipboardFormat.InkSerializedFormat };
            _markerDA = new DrawingAttributes();
            _markerDA.Color = Color.FromRgb(200, 0, 200);
            _markerDA.StylusTip = StylusTip.Ellipse;
            _markerDA.Width = 3;
            _markerDA.Height = 3;

            inkcanvas.DefaultDrawingAttributes = _markerDA;
            
            _highlightDA = new DrawingAttributes();
            _highlightDA.IsHighlighter = true;
            _highlightDA.Color = Color.FromRgb(255, 255, 0);
            _highlightDA.Width = 10;
            _highlightDA.Height = 25;
            _highlightDA.StylusTip = StylusTip.Rectangle;

            _colorPicker = new ColorPicker();
            _colorPicker.SelectedDrawingAttributes = _markerDA;
            color_panel.Children.Add(_colorPicker);
        }

        #region Event Handlers

        /// <summary>
        /// Switch to inking mode.
        /// </summary>
        private void Ink_Click(object sender, RoutedEventArgs e)
        {
            _colorPicker.SelectedDrawingAttributes = _markerDA; 
            inkcanvas.DefaultDrawingAttributes = _markerDA;     

            inkcanvas.EditingMode = InkCanvasEditingMode.Ink;
        }

        /// <summary>
        /// Switch to highlighting mode.
        /// </summary>
        private void Highlight_Click(object sender, RoutedEventArgs e)
        {
            _colorPicker.SelectedDrawingAttributes = _highlightDA;
            inkcanvas.DefaultDrawingAttributes = _highlightDA;    

            inkcanvas.EditingMode = InkCanvasEditingMode.Ink;
        }

        /// <summary>
        /// Switch to erase mode.
        /// </summary>
        private void Erase_Click(object sender, RoutedEventArgs e)
        {
            inkcanvas.EditingMode = InkCanvasEditingMode.EraseByStroke;
        }

        /// <summary>
        /// Clear all strokes from the inkcanvas.
        /// </summary>
        private void Clear_Click(object sender, RoutedEventArgs e)
        {
            inkcanvas.Strokes.Clear();
        }
        #endregion

    }
}