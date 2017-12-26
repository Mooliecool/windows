using System; // Uri, UriKind
using System.IO; // Stream
using System.Windows; // RoutedEventHandler
using System.Windows.Controls; // Page

namespace WPFBrowserApplication
{
    public partial class HomePage : Page
    {
        public HomePage()
        {
            this.Loaded += new RoutedEventHandler(HomePage_Loaded);

            InitializeComponent();
        }

        void HomePage_Loaded(object sender, RoutedEventArgs e)
        {
            this.webBrowser.ObjectForScripting = new ScriptableClass();
        }

        private void sendMessageToSilverlightButton_Click(object sender, RoutedEventArgs e)
        {
            this.webBrowser.InvokeScript("SendMessageToSilverlight", this.msgTextBox.Text);
        }
    }
}
