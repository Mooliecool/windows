namespace AdaptiveTopologySample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    /// <summary>
    /// Interaction logic for DataEntryPage.xaml
    /// </summary>

    public partial class DataEntryPage : PageFunction<WizardContext>
    {
        public DataEntryPage()
        {
            InitializeComponent();
        }

        void okButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to next Wizard - as determined by the navigation hub
            WizardNavigationDirection direction = ((bool)this.forwardsRadioButton.IsChecked ? WizardNavigationDirection.Forwards : WizardNavigationDirection.Reverse);
            OnReturn(new ReturnEventArgs<WizardContext>(new WizardContext(WizardResult.Finished, direction)));
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel the Wizard and don't return any data
            OnReturn(new ReturnEventArgs<WizardContext>(new WizardContext(WizardResult.Canceled, null)));
        }
    }
}