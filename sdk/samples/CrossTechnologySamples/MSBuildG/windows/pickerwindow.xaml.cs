using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Input;
using Microsoft.Win32;
using System.Runtime.InteropServices;
using System.Windows.Interop;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for PickerWindow.xaml
    

    public partial class PickerWindow : Window
    {
        public PickerWindow()
        {
            InitializeComponent();
        }

        private void ShowWizard(object sender, RoutedEventArgs e)
        {
            WizardWindow buildWizard = new WizardWindow();
            buildWizard.Show();

            this.Close();
        }

        private void ShowDesigner(object sender, RoutedEventArgs e)
        {
            DesignerWindow buildDesigner = new DesignerWindow();
            buildDesigner.Show();
            this.Close();
        }
   }
}