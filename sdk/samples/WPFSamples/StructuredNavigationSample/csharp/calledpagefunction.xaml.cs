//<SnippetCalledPageFunctionCODEBEHIND1>
using System; // String
using System.Windows; // RoutedEventArgs, RoutedEventHandler
using System.Windows.Navigation; // PageFunction

namespace StructuredNavigationSample
{
    public partial class CalledPageFunction : PageFunction<String>
    {
        public CalledPageFunction()
        {
            InitializeComponent();
        }
        public CalledPageFunction(string initialDataItem1Value)
        {
            InitializeComponent();

            this.okButton.Click += new RoutedEventHandler(okButton_Click);
            this.cancelButton.Click += new RoutedEventHandler(cancelButton_Click);
            // Set initial value
            this.dataItem1TextBox.Text = initialDataItem1Value;
        }
        void okButton_Click(object sender, RoutedEventArgs e)
        {
            // Accept when Ok button is clicked
            OnReturn(new ReturnEventArgs<string>(this.dataItem1TextBox.Text));
        }

        void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            // Cancel 
            OnReturn(null);
        }
    }
}
//</SnippetCalledPageFunctionCODEBEHIND2>