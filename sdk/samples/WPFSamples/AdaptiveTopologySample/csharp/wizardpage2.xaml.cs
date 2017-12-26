namespace AdaptiveTopologySample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    /// <summary>
    /// Interaction logic for WizardPage2.xaml
    /// </summary>

    public partial class WizardPage2 : PageFunction<WizardResult>
    {
        public WizardPage2(object WizardData)
        {
            InitializeComponent();

            // Bind Wizard state to UI
            this.DataContext = WizardData;
            
            this.Loaded += new RoutedEventHandler(WizardPage2_Loaded);
        }

        void WizardPage2_Loaded(object sender, RoutedEventArgs e)
        {
            // Enable buttons based on position
            this.backButton.IsEnabled = WizardNavigationHub.Current.CanGoBack(this);
            this.nextButton.IsEnabled = WizardNavigationHub.Current.CanGoNext(this);
            this.nextButton.IsDefault = WizardNavigationHub.Current.CanGoNext(this);
            this.finishButton.IsEnabled = WizardNavigationHub.Current.CanFinish(this);
            this.finishButton.IsDefault = WizardNavigationHub.Current.CanFinish(this);
        }

        void backButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to previous Wizard page
            this.NavigationService.GoBack();
        }

        void nextButton_Click(object sender, RoutedEventArgs e)
        {
            // Go to next Wizard page
            if (this.NavigationService.CanGoForward) this.NavigationService.GoForward();
            else
            {
                PageFunction<WizardResult> nextPage = WizardNavigationHub.Current.GetNextWizardPage(this);
                nextPage.Return += new ReturnEventHandler<WizardResult>(WizardPage_Return);
                this.NavigationService.Navigate(nextPage);
            }
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

        public void WizardPage_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // This is called when the next page returns, so return what they are returning
            OnReturn(e);
        }
    }
}