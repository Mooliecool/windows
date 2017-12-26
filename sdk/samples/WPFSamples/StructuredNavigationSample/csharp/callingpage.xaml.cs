//<SnippetPassingDataCODEBEHIND1>
using System.Windows; // RoutedEventArgs, RoutedEventHandler, Visibility
using System.Windows.Controls; // Page
using System.Windows.Navigation; // ReturnEventArgs

namespace StructuredNavigationSample
{
    public partial class CallingPage : Page
    {
        public CallingPage()
        {
            InitializeComponent();
            this.pageFunctionHyperlink.Click += new RoutedEventHandler(pageFunctionHyperlink_Click);
        }
        void pageFunctionHyperlink_Click(object sender, RoutedEventArgs e)
        {
            // Instantiate and navigate to page function
            CalledPageFunction CalledPageFunction = new CalledPageFunction("Initial Data Item Value");
            CalledPageFunction.Return += pageFunction_Return;
            this.NavigationService.Navigate(CalledPageFunction);
        }
        void pageFunction_Return(object sender, ReturnEventArgs<string> e)
        {
            this.pageFunctionResultsTextBlock.Visibility = Visibility.Visible;

            // Display result
            this.pageFunctionResultsTextBlock.Text = (e != null ? "Accepted" : "Canceled");

            // If page function returned, display result and data
            if (e != null)
            {
                this.pageFunctionResultsTextBlock.Text += "\n" + e.Result;
            }
        }
    }
}
//</SnippetCallingPageDefaultCODEBEHIND3>