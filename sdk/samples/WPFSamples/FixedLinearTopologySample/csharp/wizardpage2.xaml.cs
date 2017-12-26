namespace FixedLinearTopologySample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public partial class WizardPage2 : PageFunction<WizardResult>
    {
        public WizardPage2(WizardData WizardData)
        {
            InitializeComponent();

            // Bind Wizard state to UI
            this.DataContext = WizardData;
        }

        void backButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to previous Wizard page
            this.NavigationService.GoBack();
        }

        void nextButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to next Wizard page
            WizardPage3 WizardPage3 = new WizardPage3((WizardData)this.DataContext);
            WizardPage3.Return += new ReturnEventHandler<WizardResult>(WizardPage_Return);
            this.NavigationService.Navigate(WizardPage3);
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the Wizard and don't return any data
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Canceled));
        }

        public void WizardPage_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // If returning, Wizard was completed (finished or canceled),
            // so continue returning to calling page
            OnReturn(e);
        }
    }
}