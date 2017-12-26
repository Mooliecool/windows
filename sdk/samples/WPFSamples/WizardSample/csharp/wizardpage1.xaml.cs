namespace WizardSample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public partial class WizardPage1 : PageFunction<WizardResult>
    {
        public WizardPage1(WizardData wizardData)
        {
            InitializeComponent();
            
            // Bind wizard state to UI
            this.DataContext = wizardData;
        }

        void nextButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to next wizard page
            WizardPage2 wizardPage2 = new WizardPage2((WizardData)this.DataContext);
            wizardPage2.Return += new ReturnEventHandler<WizardResult>(wizardPage_Return);
            this.NavigationService.Navigate(wizardPage2);
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