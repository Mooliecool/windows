namespace WizardSample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public partial class WizardPage2 : PageFunction<WizardResult>
    {
        public WizardPage2(WizardData wizardData)
        {
            InitializeComponent();

            // Bind wizard state to UI
            this.DataContext = wizardData;
        }

        void backButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to previous wizard page
            this.NavigationService.GoBack();
        }

        void nextButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to next wizard page
            WizardPage3 wizardPage3 = new WizardPage3((WizardData)this.DataContext);
            wizardPage3.Return += new ReturnEventHandler<WizardResult>(wizardPage_Return);
            this.NavigationService.Navigate(wizardPage3);
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the wizard and don't return any data
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Canceled));
        }

        public void wizardPage_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // If returning, wizard was completed (finished or canceled),
            // so continue returning to calling page
            OnReturn(e);
        }
    }
}