//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Microsoft.Samples.CorrelatedCalculator.CalculatorClient
{
    /// <summary>
    /// Interaction logic for CalculatorWindow.xaml
    /// </summary>
    
    public partial class CalculatorWindow : Window
    {
        int CalculatorId;
        Microsoft.Samples.CorrelatedCalculator.CalculatorClient.ServiceReference1.CalculatorClient client;

        Brush onBrush = Brushes.LightSteelBlue;
        Brush offBrush = Brushes.GhostWhite;

        bool displayNew = true;
        string previousOp = "+";

        Microsoft.Samples.CorrelatedCalculator.CalculatorClient.ServiceReference1.CalculatorClient Client
        {
            get
            {
                if (client == null)
                {
                    this.CalculatorId++;
                    client = new Microsoft.Samples.CorrelatedCalculator.CalculatorClient.ServiceReference1.CalculatorClient();
                }
                return client;
            }
        }

        public CalculatorWindow()
        {
            InitializeComponent();

            this.CalculatorId = new Random(DateTime.Now.Millisecond).Next();
            client = new Microsoft.Samples.CorrelatedCalculator.CalculatorClient.ServiceReference1.CalculatorClient();
        }

        void UpdateDisplay(object sender, RoutedEventArgs e)
        {
            string input = ((Button)sender).Content.ToString();
            if (displayNew)
            {
                Display.Text = input;
                Display.Background = offBrush;
            }
            else
                Display.Text += input;
            displayNew = false;
        }

        void DoOperation(object sender, RoutedEventArgs e)
        {
            double value = 0;
            string operation = ((Button)sender).Content.ToString();
            if (!previousOp.StartsWith("=") && !double.TryParse(Display.Text,out value))
            {
                MessageBox.Show("Invalid input! Try again.");
                Display.Text = String.Empty;
                return;
            }
                        
            try
            {
                if (!displayNew)
                {
                    switch (previousOp)
                    {
                        case "=": { break; }
                        case "+": { Client.Add(value, CalculatorId.ToString()); break; }
                        case "-": { Client.Subtract(value, CalculatorId.ToString()); break; }
                        case "x": { Client.Multiply(value, CalculatorId.ToString()); break; } 
                        case "/":
                            {
                                if (value == 0)
                                {
                                    MessageBox.Show("Divide By Zero is not allowed");
                                    value = Client.Equals(CalculatorId.ToString());
                                    Display.Text = value.ToString();
                                    break;
                                }
                                else
                                {
                                    Client.Divide(value, CalculatorId.ToString());
                                    break;
                                }
                            }
                    }
                }
                if (operation.Equals("="))
                {
                    value = Client.Equals(CalculatorId.ToString());
                    Display.Text = value.ToString();
                }
            }
            catch (CommunicationException ce)
            {
                MessageBox.Show(ce.Message);
                Client.Abort();
                client = null;
            }

            previousOp = operation;            

            Display.Background = onBrush;            
            displayNew = true;
        }

        void Reset(object sender, RoutedEventArgs e)
        {
            try
            {
                Client.Reset(CalculatorId.ToString());
                Client.Close();
            }
            catch (CommunicationException ce)
            {
                MessageBox.Show(ce.Message);
                Client.Abort();
            }
            
            client = null;
            previousOp = "+";

            Display.Background = onBrush;
            Display.Text = String.Empty;
            displayNew = true;
        }        

        void OnExit(object sender, RoutedEventArgs e)
        {
            this.Close();
            Application.Current.Shutdown();
        }    

    }


}
