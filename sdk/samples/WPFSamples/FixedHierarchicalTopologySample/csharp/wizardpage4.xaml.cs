namespace FixedHierarchicalTopologySample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    /// <summary>
    /// Interaction logic for WizardPage4.xaml
    /// </summary>

    public partial class WizardPage4 : PageFunction<WizardResult>
    {
        public WizardPage4(WizardData WizardData)
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

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the Wizard and don't return any data
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Canceled));
        }

        void finishButton_Click(object sender, RoutedEventArgs e)
        {
            // Finish the Wizard and return bound data to calling page
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Finished));
        }
    }
}