namespace WizardSample
{
    using System;
    using System.Windows;
    
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
        
        void runWizardButton_Click(object sender, RoutedEventArgs e) {
        
            
            WizardDialogBox wizard = new WizardDialogBox();
            bool dialogResult = (bool)wizard.ShowDialog();
            if( dialogResult ) {
                MessageBox.Show(string.Format("{0}\n{1}\n{2}", wizard.WizardData.DataItem1, wizard.WizardData.DataItem2, wizard.WizardData.DataItem3));
            }
            else {
                MessageBox.Show("Canceled.");
            }
        }
    }
}