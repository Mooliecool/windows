using System.Windows; // RoutedEventHandler, RoutedEventArgs
using System.Windows.Browser; // HtmlPage
using System.Windows.Controls; // UserControl

namespace SilverlightApplication
{
    public partial class Page : UserControl
    {
        public Page()
        {
            this.Loaded += new RoutedEventHandler(Page_Loaded);

            InitializeComponent();
        }

        void Page_Loaded(object sender, RoutedEventArgs e)
        {
            HtmlPage.RegisterScriptableObject("scriptableClass", new ScriptableClass());
        }

        private void sendMessageToWPFButton_Click(object sender, RoutedEventArgs e)
        {
            HtmlPage.Window.Invoke("SendMessageToWPF", this.msgTextBox.Text);
        }
    }
}
