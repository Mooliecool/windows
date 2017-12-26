namespace FixedHierarchicalTopologySample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public partial class CallingPage : Page
    {
        public CallingPage()
        {
            InitializeComponent();
        }

        void startWizardHyperlink_Click(object sender, RoutedEventArgs e)
        {
            // Launch the Wizard
            WizardLauncher WizardStartPage = new WizardLauncher();
            WizardStartPage.Return += new ReturnEventHandler<WizardContext>(Wizard_Return);
            this.NavigationService.Navigate(WizardStartPage);
        }

        /// <summary>
        ///  Determine how the Wizard completed and, if accepted, process the collected Wizard state
        /// </summary>
        public void Wizard_Return(object sender, ReturnEventArgs<WizardContext> e)
        {
            // Get Wizard state
            WizardContext WizardContext = e.Result;

            this.WizardResultsTextBlock.Visibility = Visibility.Visible;

            // How did the Wizard end?
            this.WizardResultsTextBlock.Text += "\n" + WizardContext.Result.ToString();

            // If the Wizard completed by being accepted, display Wizard data
            if (WizardContext.Result == WizardResult.Finished)
            {
                this.WizardResultsTextBlock.Text += "\nData Item 1: " + ((WizardData)WizardContext.Data).DataItem1;
                this.WizardResultsTextBlock.Text += "\nData Item 2: " + ((WizardData)WizardContext.Data).DataItem2;
                this.WizardResultsTextBlock.Text += "\nData Item 3: " + ((WizardData)WizardContext.Data).DataItem3;
                this.WizardResultsTextBlock.Text += "\nData Item 4: " + ((WizardData)WizardContext.Data).DataItem4;
            }
        }
    }
}