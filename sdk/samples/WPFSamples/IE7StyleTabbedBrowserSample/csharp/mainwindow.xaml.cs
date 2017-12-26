using System; // Uri, UriKind
using System.Windows; // Window, RoutedEventHandler, RoutedEventArgs, Visibility
using System.Windows.Controls; // TabItem
using System.Windows.Navigation; // NavigationUIVisibility

namespace SDKSample
{
    public partial class MainWindow : Window
    {
        static int tabPageCount = 1;

        public MainWindow()
        {
            InitializeComponent();

            AddBrowserTab();
        }

        void fileAddTabMenuItem_Click(object sender, RoutedEventArgs e)
        {
            AddBrowserTab();
        }

        void fileRemoveTabMenuItem_Click(object sender, RoutedEventArgs e)
        {
            RemoveBrowserTab();
        }

        void fileExitMenuItem_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        void AddBrowserTab()
        {
            // Create new tab item with unique header and a frame as content
            TabItem ti = new TabItem();
            ti.Header = string.Format("Tab {0}", MainWindow.tabPageCount++);
            Frame frame = new Frame();
            frame.Source = new Uri("Page1.xaml", UriKind.Relative);
            frame.NavigationUIVisibility = NavigationUIVisibility.Hidden;
            ti.Content = frame;
            this.browserPagesTabControl.Items.Add(ti);

            // Select the new tab item
            this.browserPagesTabControl.SelectedItem = ti;
        }

        void RemoveBrowserTab()
        {
            // Close the current browser tab by removing it from the collection of
            // tab items contained by the tab control
            TabItem currentTi = this.browserPagesTabControl.SelectedItem as TabItem;
            this.browserPagesTabControl.Items.Remove(currentTi);
        }
    }
}
