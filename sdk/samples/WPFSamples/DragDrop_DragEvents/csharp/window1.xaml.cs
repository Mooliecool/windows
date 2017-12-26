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
            heightAdjustment = lblEventLogWindowTitle.Height + lblEventSelectionTitle.Height + eventChecksGrid.Height;
        }

        private void clickClearEventHistory(object sender, RoutedEventArgs args) { tbEventEvents.Clear(); eventCount = 0; }

        // This event handler fires whenever the main window changes size.  
        private void ehEventWindowSizeChanged(object sender, EventArgs args)
        {
            // Adjust the size of the TextBox so scrolling works properly.
            tbEventEvents.MaxHeight = tbEventEvents.Height = mainGrid.RowDefinitions[1].ActualHeight - heightAdjustment;
        }

        // Handlers for the drag/drop events.  
        private void ehDragEnter(object sender, DragEventArgs args)
        { if (cbDragEnter.IsChecked.Value) LogEvent(EventFireStrings.DragEnter, args); }
        private void ehDragLeave(object sender, DragEventArgs args)
        { if (cbDragLeave.IsChecked.Value) LogEvent(EventFireStrings.DragLeave, args); }
        private void ehDragOver(object sender, DragEventArgs args)
        { if (cbDragOver.IsChecked.Value) LogEvent(EventFireStrings.DragOver, args); }
        private void ehDrop(object sender, DragEventArgs args)
        { if (cbDrop.IsChecked.Value) LogEvent(EventFireStrings.Drop, args); }
        private void ehPreviewDragEnter(object sender, DragEventArgs args)
        { if (cbPreviewDragEnter.IsChecked.Value) LogEvent(EventFireStrings.PreviewDragEnter, args); }
        private void ehPreviewDragLeave(object sender, DragEventArgs args)
        { if (cbPreviewDragLeave.IsChecked.Value) LogEvent(EventFireStrings.PreviewDragLeave, args); }
        private void ehPreviewDragOver(object sender, DragEventArgs args)
        { if (cbPreviewDragOver.IsChecked.Value) LogEvent(EventFireStrings.PreviewDragOver, args); }
        private void ehPreviewDrop(object sender, DragEventArgs args)
        { if (cbPreviewDrop.IsChecked.Value) LogEvent(EventFireStrings.PreviewDrop, args); }

        private void LogEvent(string eventMessage, DragEventArgs args)
        {
            tbEventEvents.AppendText("[" + (++eventCount).ToString() + eventMessage);

            if (cbVerbose.IsChecked.Value)
            {
                tbEventEvents.AppendText("     Source Object: " + args.Source.ToString() + "\n");
                tbEventEvents.AppendText("     Drag Effects: " + args.Effects.ToString() + "\n");
                tbEventEvents.AppendText("     Key States: " + args.KeyStates.ToString() + "\n");
                tbEventEvents.AppendText("     Available Data Formats:\n");
                foreach (string format in args.Data.GetFormats())
                {
                    tbEventEvents.AppendText("          " + format + "\n");
                }

            }
            tbEventEvents.ScrollToEnd();
        }

        private struct EventFireStrings
        {
            public static string DragEnter = "]: The DragEnter event just fired.\n";
            public static string DragLeave = "]: The DragLeave event just fired.\n";
            public static string DragOver = "]: The DragOver event just fired.\n";
            public static string Drop = "]: The Drop event just fired.\n";
            public static string PreviewDragEnter = "]: The PreviewDragEnter event just fired.\n";
            public static string PreviewDragLeave = "]: The PreviewDragLeave event just fired.\n";
            public static string PreviewDragOver = "]: The PreviewDragOver event just fired.\n";
            public static string PreviewDrop = "]: The PreviewDrop event just fired.\n";
        }

        // To store the height of the other elements in the grid column containing
        // the event log.
        private double heightAdjustment;

        private uint eventCount = 0;
    }
}