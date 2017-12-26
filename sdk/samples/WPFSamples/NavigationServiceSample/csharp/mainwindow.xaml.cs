using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace NavigationServiceSample
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        NavigationService NavigationService
        {
            // Retrieve the navigation service, provided by the frame,
            // for the frame content.
            get { return NavigationService.GetNavigationService((DependencyObject)this.browserFrame.Content); }
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigating += NavigationService_Navigating;
            this.NavigationService.Navigated += NavigationService_Navigated;
            this.NavigationService.NavigationProgress += NavigationService_NavigationProgress;
            this.NavigationService.NavigationStopped += NavigationService_NavigationStopped;
            this.NavigationService.NavigationFailed += NavigationService_NavigationFailed;
            this.NavigationService.LoadCompleted += NavigationService_LoadCompleted;
            this.NavigationService.FragmentNavigation += NavigationService_FragmentNavigation;

            // Once NavigationService delivers the content (a fully formed content tree) to the navigator, 
            // it's up to the navigator to do the rest, which include async loading each required image,
            // as per WPF framework standard behavior.

            this.backButton.IsEnabled = this.NavigationService.CanGoBack;
            this.forwardButton.IsEnabled = this.NavigationService.CanGoForward;
        }

        void goButton_Click(object sender, RoutedEventArgs e)
        {
            this.navigatingEventsListBox.Items.Clear();
            Uri uri = new Uri(this.addressTextBox.Text, UriKind.RelativeOrAbsolute);
            this.NavigationService.Navigate(uri);
        }
        void backButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.GoBack();
        }
        void forwardButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.GoForward();
        }
        void stopButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.StopLoading();
        }
        void refreshButton_Click(object sender, RoutedEventArgs e)
        {
            this.navigatingEventsListBox.Items.Clear();
            this.NavigationService.Refresh();
        }

        #region Navigation Events
        void NavigationService_Navigating(object sender, NavigatingCancelEventArgs e)
        {
            Log("Navigating: [" + e.Uri + "]");
        }
        void NavigationService_Navigated(object sender, NavigationEventArgs e)
        {
            Log("Navigated: [" + e.Uri + "]");
        }
        void NavigationService_NavigationProgress(object sender, NavigationProgressEventArgs e)
        {
            Log("Progress: " + e.BytesRead.ToString() + " of " + e.MaxBytes.ToString() + " [" + e.Uri + "]");
        }
        void NavigationService_NavigationStopped(object sender, NavigationEventArgs e)
        {
            Log("Navigation Stopped: [" + e.Uri + "]");
        }
        void NavigationService_NavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            Log("Navigation Failed: [" + e.Uri + " - " + e.Exception.Message + "]");
        }
        void NavigationService_LoadCompleted(object sender, NavigationEventArgs e)
        {
            Log("Load Completed: [" + e.Uri + "]");

            this.backButton.IsEnabled = this.NavigationService.CanGoBack;
            this.forwardButton.IsEnabled = this.NavigationService.CanGoForward;
        }
        void NavigationService_FragmentNavigation(object sender, FragmentNavigationEventArgs e)
        {
            Log("Fragment Navigation: [" + e.Fragment + "]");
        }
        #endregion

        void Log(string item)
        {
            this.navigatingEventsListBox.Items.Add(item);
            this.navigatingEventsListBox.SelectedIndex = this.navigatingEventsListBox.Items.Count - 1;
            this.navigatingEventsListBox.Focus();
        }
    }
}