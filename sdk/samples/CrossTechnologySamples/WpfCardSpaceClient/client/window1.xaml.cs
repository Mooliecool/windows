using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.IdentityModel.Selectors;
using System.ServiceModel.Security;
using System.Windows.Threading;
using Microsoft.ServiceModel.Samples;

namespace CardSpaceWpfCallingWcf
{
    public partial class Window1 : Window
    {
        bool identityRetireved = false;

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
        SecureCalculatorClient client = new SecureCalculatorClient();

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            // Clean up client when Window closes
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

            // Let the user know
            _numberTextBlock.Text = "(getting result)";
        }

        // Show the result when call to web service completes
        void CalcComplete(IAsyncResult ar)
        {
            //This call back is not coming on the ui thread, so we must use the
            //Dispatcher to invoke on the ui thread
            Dispatcher.Invoke(DispatcherPriority.Normal, new AsyncCallback(UpdateNumberText), ar);
        }

        void IdentityCallback(IAsyncResult ar)
        {

            //This call back is not coming on the ui thread, so we must use the
            //Dispatcher to invoke on the ui thread
            Dispatcher.Invoke(DispatcherPriority.Normal, new AsyncCallback(UpdateIdentityText), ar);
        }

        delegate void ShowResultsDelegate(string result);

        void UpdateNumberText(IAsyncResult ar)
        {
            double result = 0;
            string opcode = (string)ar.AsyncState;

            try
            {
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

                // Show the result
                _numberTextBlock.Text = result.ToString();

                lock (this)
                {
                    // Get the identity
                    if (!identityRetireved)
                    {
                        client.BeginGetIdentity(IdentityCallback, null);
                        identityRetireved = true;
                    }
                }
            }
            catch (UserCancellationException ex)
            {
                _numberTextBlock.Text = "User Cancelation: " + ex.Message;
            }
            catch (UntrustedRecipientException ex)
            {
                _numberTextBlock.Text = "Untrusted Recipient: " + ex.Message;
            }
            catch (ServiceNotStartedException ex)
            {
                _numberTextBlock.Text = "CardSpace Service Not Started: " + ex.Message;
            }
            catch (SecurityNegotiationException ex)
            {
                _numberTextBlock.Text = "Security Negotiation: " + ex.Message;
            }
            catch (IdentityValidationException ex)
            {
                _numberTextBlock.Text = "Identity Validation: " + ex.Message;
            }
            catch (Exception ex)
            {
                _numberTextBlock.Text = "Exception: " + ex.Message;
            }
        }

        void UpdateIdentityText(IAsyncResult ar)
        {
            // Show the identity
            _identityTextBlock.Text = client.EndGetIdentity(ar); ;
        }
    }
}
