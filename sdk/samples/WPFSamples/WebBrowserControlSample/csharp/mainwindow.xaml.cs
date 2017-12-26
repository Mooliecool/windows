using System; // Uri
using System.IO;
using System.Windows; // Window, RoutedEventArgs
using System.Windows.Navigation; // NavigatingCancelEventArgs, NavigationEventArgs

namespace WebBrowserControlSample
{
    public partial class MainWindow : Window
    {
        ScriptableClass scriptableClass;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void backButton_Click(object sender, RoutedEventArgs e)
        {
            // Navigate to the previous HTML document, if there is one
            if (this.webBrowser.CanGoBack)
            {
                this.webBrowser.GoBack();
            }
            else
            {
                MessageBox.Show("Cannot go back. There needs to be something in the history to go back to.");
            }
        }
        private void forwardButton_Click(object sender, RoutedEventArgs e)
        {
            // Navigate to the next HTML document, if there is one
            if (this.webBrowser.CanGoForward)
            {
                this.webBrowser.GoForward();
            }
            else
            {
                MessageBox.Show("Cannot go Forward. There needs to be something in the history to go forward to.");
            }
        }
        private void refreshButton_Click(object sender, RoutedEventArgs e)
        {
            if (webBrowser.IsLoaded && webBrowser.Source != null)
            {
                this.webBrowser.Refresh();
            }
            else
            {
                MessageBox.Show("Refresh what? You need to load a page in the web browser before you can refesh.");
            }
        }

        private void goNavigateButton_Click(object sender, RoutedEventArgs e)
        {
            // Get URI to navigate to
            Uri uri = new Uri(this.addressTextBox.Text, UriKind.RelativeOrAbsolute);

            // Only absolute URIs can be navigated to.
            try
            {
              // Navigate to the desired URL by calling the .Navigate method
              this.webBrowser.Navigate(uri);
            }
            catch
            {
                MessageBox.Show("The Address URI must be absolute eg 'http://www.microsoft.com'");
                return;
            }
        }
        private void goSourceButton_Click(object sender, RoutedEventArgs e)
        {
            // Get URI to navigate to
            Uri uri = new Uri(this.addressTextBox.Text, UriKind.RelativeOrAbsolute);

            // Only absolute URIs can be navigated to.
            try
            {
                // Navigate to the desired URL by setting the .Source property
                this.webBrowser.Source = uri;
            }
            catch
            {
                MessageBox.Show("The Address URI must be absolute eg 'http://www.microsoft.com'");
                return;
            }
        }
        private void goNavigateToStringButton_Click(object sender, RoutedEventArgs e)
        {
            // Load HTML document as a string
            Uri uri = new Uri(@"pack://application:,,,/HTMLDocumentWithoutScript.html", UriKind.Absolute);
            Stream stream = Application.GetResourceStream(uri).Stream;
            using (StreamReader reader = new StreamReader(stream))
            {
                // Navigate to HTML document string
                this.webBrowser.NavigateToString(reader.ReadToEnd());
            }
        }
        private void goNavigateToStreamButton_Click(object sender, RoutedEventArgs e)
        {
            // Load HTML document as a stream
            Uri uri = new Uri(@"pack://application:,,,/HTMLDocumentWithoutScript.html", UriKind.Absolute);
            Stream source = Application.GetResourceStream(uri).Stream;

            // Navigate to HTML document stream
            this.webBrowser.NavigateToStream(source);
        }

        private void goNavigateToWordDoc_Click(object sender, RoutedEventArgs e)
        {
            // see http://nerddawg.blogspot.com/2005/12/more-on-resource-loading-in-wpf.html for
            // more information on pack URIs (pack://siteoforigin).
            Uri uri = new Uri(@"pack://siteoforigin:,,,/Test_Word_File.doc", UriKind.Absolute);

            // Load the word doc into the webBrowser. The user can change this document.
            this.webBrowser.Source = uri;

        }

        private void saveWordDoc_Click(object sender, RoutedEventArgs e)
        {

            // The user can only save when a Word Doc is loaded. Error message provided.
            string msg = "You need to load a Word Document before you can save it." +
             " Press the 'Load Word Doc' button to load a test Word Document";

            // Ensure that the document in the web browser is actually a Microsoft Word Document
            if (webBrowser.Source != null)
            {
                if (webBrowser.Document.GetType().ToString() == "Microsoft.Office.Interop.Word.DocumentClass")
                {
                    // Cast the WebBrowser document as a Word doc so that you can manipulate it.
                    // In this example, Microsoft Word is the type of document being manipulated but you
                    // can use WebBrowser.Document to cast to other document types as well.
                    Microsoft.Office.Interop.Word.Document myWordDoc = webBrowser.Document as Microsoft.Office.Interop.Word.Document;

                    myWordDoc.Save();
                }
                else
                {
                    MessageBox.Show(msg);
                }
            }
            else
            {
                MessageBox.Show(msg);
            }

        }

        private void webBrowser_Navigating(object sender, NavigatingCancelEventArgs e)
        {

            string uriString = (e.Uri != null ? " to " + e.Uri : "");

            // The WebBrowser control is about to locate and download the specified HTML document
            this.informationStatusBarItem.Content = string.Format("Navigating{0}", uriString);

            // Cancel navigation?
            string msg = string.Format("Navigate{0}?", uriString);
            MessageBoxResult result = MessageBox.Show(msg, "Navigate", MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (result == MessageBoxResult.No)
            {
                e.Cancel = true;
                this.informationStatusBarItem.Content = string.Format("Canceled navigation to {0}", e.Uri);
            }
        }
        private void webBrowser_Navigated(object sender, NavigationEventArgs e)
        {
            // The WebBrowser control has located and begun downloading the specified HTML document
            string uriString = (e.Uri != null ? " to " + e.Uri : "");
            this.informationStatusBarItem.Content = string.Format("Navigated{0}", uriString);
        }
        private void webBrowser_LoadCompleted(object sender, NavigationEventArgs e)
        {
            // The WebBrowser control has completely downloaded the HTML document
            string uriString = (e.Uri != null ? " to " + e.Uri : "");
            this.informationStatusBarItem.Content = string.Format("Completed loading{0}", uriString);

        }

        private void loadHtmlDocWithScriptButton_Click(object sender, RoutedEventArgs e)
        {
            // Load HTML document as a stream
            Uri uri = new Uri(@"pack://application:,,,/HTMLDocumentWithScript.html", UriKind.Absolute);
            Stream source = Application.GetResourceStream(uri).Stream;

            // Set object for scripting to allow HTML document to change this WPF window's title
            this.scriptableClass = new ScriptableClass(this);
            this.webBrowser.ObjectForScripting = this.scriptableClass;

            // Navigate to HTML document stream
            this.webBrowser.NavigateToStream(source);

        }
        private void callScriptFunctionNoParamButton_Click(object sender, RoutedEventArgs e)
        {
            if (webBrowser.IsLoaded)
            {
                try
                {
                    this.webBrowser.InvokeScript("JavaScriptFunctionWithoutParameters");
                }
                catch (Exception ex)
                {
                    string msg = "Could not call script: " +
                                 ex.Message +
                                 "\n\nPlease click the 'Load HTML Document with Script' button to load.";
                    MessageBox.Show(msg);
                }
            }
        }
        private void callScriptFunctionParamButton_Click(object sender, RoutedEventArgs e)
        {
            if (webBrowser.IsLoaded)
            {
                try
                {
                    this.webBrowser.InvokeScript("JavaScriptFunctionWithParameters", this.messageTextBox.Text);
                }
                catch (Exception ex)
                {
                    string msg = "Could not call script: " +
                                 ex.Message +
                                 "\n\nPlease click the 'Load HTML Document with Script' button to load.";
                    MessageBox.Show(msg);
                }
            }
        }
    }
}
