using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;

namespace SDKSample
{
    public partial class Window1 : Window
    {
        public Window1() 
        { 
            InitializeComponent();
        }

        private void clickClear(object sender, RoutedEventArgs e) { tbDropTarget.Clear(); }
        private void ehDragEnter(object sender, DragEventArgs args) { args.Effects = DragDropEffects.Copy; }

        private void ehPreviewDrop(object sender, DragEventArgs args)
        {
            // TextBox includes native drop handling.  This is to let the TextBox know that we're handling
            // the Drop event, and we don't want the native handler to execute.  
            args.Handled = true;

            ShowDataFormats(args);
        }

        private void ShowDataFormats(DragEventArgs args)
        {
            tbDropTarget.AppendText("The following data formats are present:\n");
            if (cbAutoConvert.IsChecked.Value)
            {
                foreach (string format in args.Data.GetFormats((bool)cbAutoConvert.IsChecked))
                {
                    if (args.Data.GetDataPresent(format, false)) tbDropTarget.AppendText("\t- " + format + " (native)\n");
                    else tbDropTarget.AppendText("\t- " + format + " (autoconvert)\n");
                }
            }
            else
            {
                foreach (string format in args.Data.GetFormats((bool)cbAutoConvert.IsChecked))
                {
                    tbDropTarget.AppendText("\t- " + format + " (native)\n");
                }
            }
            
            tbDropTarget.AppendText("\n");
            tbDropTarget.ScrollToEnd();
        }
    }
}
