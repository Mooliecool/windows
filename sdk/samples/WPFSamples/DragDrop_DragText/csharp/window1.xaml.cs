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

        private void TextSelectionToDataObject(object sender, RoutedEventArgs e) 
        {
            // Create a new data object using one of the overloaded constructors.  This particular
            // overload accepts a string specifying the data format (provided by the DataFormats class),
            // and an Object (in this case a string) that represents the data to be stored in the data object.
            DataObject dataObject = new DataObject(DataFormats.Text, sourceTextBox.SelectedText);

            dataObjectInfoTextBox.Clear();

            // Get and display the native data formats (filtering out auto-convertable data formats).
            dataObjectInfoTextBox.Text = "\nNative data formats present:\n";
            foreach (string format in dataObject.GetFormats(false /*autoconvert*/)) dataObjectInfoTextBox.Text += format + "\n";

            // Display the data in the data object.
            dataObjectInfoTextBox.Text += "\nData contents:\n";
            dataObjectInfoTextBox.Text += dataObject.GetData(DataFormats.Text, false /*autoconvert*/).ToString();
        }

    }
}