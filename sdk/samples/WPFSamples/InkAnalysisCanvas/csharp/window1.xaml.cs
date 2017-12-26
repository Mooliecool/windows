using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace InkAnalysisCanvasSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : System.Windows.Window
    {
        /// <summary>
        /// Used to keep track of which results to highlight
        /// </summary>
        private int _resultIndex = 0;
        private string _searchText = "";

        public Window1()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Button click event handlers
        /// </summary>
        private void btnInk_Click(object sender, RoutedEventArgs e)
        {
            inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.InkAndGesture;
            SetToggleButtonState((ToggleButton)sender);
        }
        private void btnEraseByStroke_Click(object sender, RoutedEventArgs e)
        {
            inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.EraseByStroke;
            SetToggleButtonState((ToggleButton)sender);
        }
        private void btnEraseByPoint_Click(object sender, RoutedEventArgs e)
        {
            inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.EraseByPoint;
            SetToggleButtonState((ToggleButton)sender);
        }
        private void btnSelect_Click(object sender, RoutedEventArgs e)
        {
            inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.Select;
            SetToggleButtonState((ToggleButton)sender);
        }

        /// <summary>
        /// Sets the toggle button state to off for all ToggleButtons other than the one passed in
        /// </summary>
        private void SetToggleButtonState(ToggleButton button)
        {
            if (button != btnInk)
            {
                btnInk.IsChecked = false;
            }
            if (button != btnEraseByStroke)
            {
                btnEraseByStroke.IsChecked = false;
            }
            if (button != btnEraseByPoint)
            {
                btnEraseByPoint.IsChecked = false;
            }
            if (button != btnSelect)
            {
                btnSelect.IsChecked = false;
            }
        }


        /// <summary>
        /// Handler for turning feedback on / off
        /// </summary>
        private void ToggleFeedback_Click(object sender, RoutedEventArgs e)
        {
            inkAnalysisCanvas.ShowInkAnalysisFeedback = !inkAnalysisCanvas.ShowInkAnalysisFeedback;
        }

        private void tbFind_GotFocus(object sender, RoutedEventArgs e)
        {
            tbFind.Text = "";
        }

        /// <summary>
        /// Handles searching for text in the document
        /// </summary>
        private void Find_Click(object sender, RoutedEventArgs e)
        {
            if (_searchText != tbFind.Text)
            {
                _resultIndex = 0;
            }

            _searchText = tbFind.Text;
            StrokeCollection[] searchResults = inkAnalysisCanvas.InkAnalyzer.Search(_searchText);
            if (searchResults.Length == 0)
            {
                MessageBox.Show("Could not find the text '" + _searchText + "'");
            }
            else
            {
                //switch to selection mode and select the strokes
                //we found
                inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.Select;
                SetToggleButtonState(btnSelect);

                if (_resultIndex >= searchResults.Length)
                {
                    //we wrapped around to the beginning
                    _resultIndex = 0;
                }

                inkAnalysisCanvas.Select(searchResults[_resultIndex]);

                //increment the result index so if the user clicks find more 
                //than once, they advance to the next result
                _resultIndex++;
            }
        }
    }
}