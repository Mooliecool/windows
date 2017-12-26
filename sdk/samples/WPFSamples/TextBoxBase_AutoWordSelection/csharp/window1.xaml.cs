using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.IO;

namespace SDKSample
{

    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
            myCheckBox.IsChecked = false;
            myTextBox.AutoWordSelection = false;

            LoadSampleContent();
        }

        private void AutoWordSelectionToggle(object sender, RoutedEventArgs e) 
        {
            myTextBox.AutoWordSelection = myCheckBox.IsChecked.Value;
        }

        private void LoadSampleContent()
        {
            // Assumes sample content file is in the root project folder, and that
            // the project exectutable will be in bin\debug relative to the root
            // project folder.  Tweak as necessary.
            string relativePathandFileName = "..\\..\\sample_content.txt";

            StreamReader fileToLoad = new StreamReader(relativePathandFileName);
            myTextBox.Text = fileToLoad.ReadToEnd();
            fileToLoad.Close();
        }
    }
}