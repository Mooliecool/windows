using System;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.ComponentModel;
using System.Windows.Data;
using System.Diagnostics;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Microsoft.Win32;
using System.Text;
//using System.Windows.Forms;
using Microsoft.SDK.Samples.VistaBridge.Library;


namespace Microsoft.SDK.Samples.VistaBridge
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        private TaskDialog taskDialog;

        bool initializedComplexDialog;
        private Guid dialog1Guid = new Guid("00000000000000000000000000000001");
        private Guid dialog2Guid = new Guid("00000000000000000000000000000002");
        private bool useFirstGuid = true;

        public Window1()
        {
            InitializeComponent();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            
        }


        #region MessageBox/TaskDialog Handlers and Helpers

        private void WFMessageBoxClicked(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.MessageBox.Show("Hello from Windows Forms!", "Hello World!");
        }

        private void WPFMessageBoxClicked(object sender, RoutedEventArgs e)
        {
            System.Windows.MessageBox.Show("Hello from WPF!", "Hello world!");
        }

        private void HelloWorldTDClicked(object sender, RoutedEventArgs e)
        {
            TaskDialog.Show("Hello from Vista!", "Hello World!", "Hello World!");
        }

        private void SimpleWaitTDClicked(object sender, RoutedEventArgs e)
        {
            taskDialog = FindTaskDialog("simpleWaitTD");
            taskDialog.Show();
        }

        private void ConfirmationTDClicked(object sender, RoutedEventArgs e)
        {
            taskDialog = FindTaskDialog("confirmationTD");
            taskDialog.Show();
        }

        private void ComplexTDClicked(object sender, RoutedEventArgs e)
        {
            taskDialog = FindTaskDialog("complexTD");

            if (initializedComplexDialog == false)
            {
                taskDialog.ExpandedText += " Link: <A HREF=\"Http://www.microsoft.com\">Microsoft</A>";
                initializedComplexDialog = true;
            }

            taskDialog.Show();
        }

        private void ComplexTD2Clicked(object sender, RoutedEventArgs e)
        {
            taskDialog = FindTaskDialog("complexTD2");
            taskDialog.Show();
        }
        private TaskDialog FindTaskDialog(string name)
        {
            return (TaskDialog)FindResource(name);
        }

        private void OnTick(object sender, TaskDialogTickEventArgs e)
        {
            taskDialog.Content = "Seconds elapsed:   " + e.Ticks / 1000;
            // Update the progress bar if there is one.
            if (taskDialog.ProgressBar != null)
            {
                if (taskDialog.ProgressBar.Value == taskDialog.ProgressBar.Maximum)
                {
                    taskDialog.ProgressBar.Value = taskDialog.ProgressBar.Minimum;
                }
                taskDialog.ProgressBar.Value = taskDialog.ProgressBar.Value + 10;
            }
        }

        private void OnDialogClosing(object sender, TaskDialogClosingEventArgs e)
        {
            // Can check e.CustomButton and e.StandardButton properties
            // to determine whether or not to cancel this Close event.
        }

        private void OnHyperlinkClick(object sender, TaskDialogHyperlinkClickedEventArgs e)
        {
            MessageBox.Show("Link clicked: " + e.LinkText);
        }
        
       
        // Executes if the user presses F1 when the dialog is showing.
        private void OnHelpInvoked(object sender, EventArgs e)
        {
            // Launch Windows Help and Support.
            string path = Environment.GetEnvironmentVariable("windir")
                + @"\helppane.exe";
            ProcessStartInfo info = new ProcessStartInfo(path, "-embedding");
            Process.Start(info);

        }

        #endregion

        #region File Dialog Handlers and Helpers

        private void WPFDialogClicked(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialogOpenFile = new Microsoft.Win32.OpenFileDialog();
            dialogOpenFile.ShowDialog();
        }

        private void SaveVistaFileDialogClicked(object sender, RoutedEventArgs e)
        {
            CommonSaveFileDialog saveDialog = new CommonSaveFileDialog();
            saveDialog.Title = "My Save File Dialog";

            CommonFileDialogResult result = saveDialog.ShowDialog();
            if (!result.Canceled)
                MessageBox.Show("File chosen: " + saveDialog.FileName);
        }

        private void OpenVistaFileDialogClicked(object sender, RoutedEventArgs e)
        {
            CommonOpenFileDialog openDialog = new CommonOpenFileDialog();
            openDialog.Title = "My Open File Dialog";
            openDialog.MultiSelect = true;

            if (useFirstGuid)
                openDialog.UsageIdentifier = dialog1Guid;
            else
                openDialog.UsageIdentifier = dialog2Guid;
            useFirstGuid = !useFirstGuid;

            // Add custom file filter.
            openDialog.Filters.Add(new CommonFileDialogFilter("My File Types", "john,doe"));

            // Add some standard filters.
            openDialog.Filters.Add(CommonFileDialogStandardFilters.TextFiles);
            openDialog.Filters.Add(CommonFileDialogStandardFilters.OfficeFiles);

            CommonFileDialogResult result = openDialog.ShowDialog();
            if (!result.Canceled)
            {
                StringBuilder output = new StringBuilder("Files selected: ");
                foreach (string file in openDialog.FileNames)
                {
                    output.Append(file);
                    output.Append(Environment.NewLine);
                }
                TaskDialog.Show(output.ToString(), "Files Chosen", "Files Chosen");
            }
        }

        #endregion

        private void TaskDialogRadioButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show(sender.ToString());
        }
    }
}