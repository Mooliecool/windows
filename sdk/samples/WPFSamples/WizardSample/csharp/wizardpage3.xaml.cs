namespace WizardSample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public partial class WizardPage3 : PageFunction<WizardResult>
    {
        public WizardPage3(WizardData wizardData)
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

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the wizard and don't return any data
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Canceled));
        }

        void finishButton_Click(object sender, RoutedEventArgs e)
        {
            // Finish the wizard and return bound data to calling page
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Finished));
        }
    }
}