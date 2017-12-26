namespace FixedHierarchicalTopologySample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    /// <summary>
    /// Interaction logic for WizardPage1.xaml
    /// </summary>

    public partial class WizardPage1 : PageFunction<WizardResult>
    {
        public WizardPage1(WizardData WizardData)
        {
            InitializeComponent();

            // Bind Wizard state to UI
            this.DataContext = WizardData;
        }

        void nextButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to next Wizard page
            WizardPage2 WizardPage2 = new WizardPage2((WizardData)this.DataContext);
            WizardPage2.Return += new ReturnEventHandler<WizardResult>(WizardPage_Return);
            this.NavigationService.Navigate(WizardPage2);
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the Wizard and don't return any data
            OnReturn(new ReturnEventArgs<WizardResult>(WizardResult.Canceled)); ;
        }

        public void WizardPage_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // If returning, Wizard was completed (finished or canceled),
            // so continue returning to calling page
            OnReturn(e);
        }
    }
}