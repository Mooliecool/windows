using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using Microsoft.ServiceModel.Samples;
using System.Windows.Threading;

namespace WpfDataBindingToWcfResults
{
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            // Handle button click
            _equalsButton.Click += _equalsButton_Click;
        }

        // Keep proxy for lifetime of Window
        CalcPlusClient client = new CalcPlusClient();

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            //Closing the client gracefully closes the connection and cleans up resources
            client.Close();
        }

        void _equalsButton_Click(object sender, RoutedEventArgs e)
        {
            double operand1 = double.Parse(_operand1.Text);
            double operand2 = double.Parse(_operand2.Text);
            string opcode = ((TextBlock)_op.SelectedValue).Text;

            // Start the call to the web service asynchronously, calling CalcComplete
            // when each calculation is complete and passing the calc opcode via
            // IAsyncResult.AsyncState so we can know which proxy End method to call
            switch (opcode)
            {
                case "+":
                    client.BeginAdd(operand1, operand2, CalcComplete, opcode);
                    break;

                case "-":
                    client.BeginSubtract(operand1, operand2, CalcComplete, opcode);
                    break;

                case "*":
                    client.BeginMultiply(operand1, operand2, CalcComplete, opcode);
                    break;

                case "/":
                    client.BeginDivide(operand1, operand2, CalcComplete, opcode);
                    break;
            }

            // Let the user know by creating a phony calc result object
            // (which the GUI knows how to bind to) and setting the words
            CalcResult progress = new CalcResult();
            progress.Words = "(getting results)";
            this.DataContext = progress;
        }

        // Show the result when call to web service completes
        void CalcComplete(IAsyncResult ar)
        {
            //This call back is not coming on the ui thread, so we must use the
            //Dispatcher to invoke on the ui thread
            Dispatcher.Invoke(DispatcherPriority.Normal, new AsyncCallback(BindResults), ar);
        }

        void BindResults(IAsyncResult ar)
        {
            CalcResult result = null;
            string opcode = (string)ar.AsyncState;

            // Harvest the results
            switch (opcode)
            {
                case "+":
                    result = client.EndAdd(ar);
                    break;

                case "-":
                    result = client.EndSubtract(ar);
                    break;

                case "*":
                    result = client.EndMultiply(ar);
                    break;

                case "/":
                    result = client.EndDivide(ar);
                    break;
            }

            // Put the results into the data context, causing the
            // bound data to be updated in the TextBlock controls
            this.DataContext = result;
        }
    }
}
