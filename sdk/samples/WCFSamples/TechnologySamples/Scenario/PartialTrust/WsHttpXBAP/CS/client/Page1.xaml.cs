//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Deployment.Application;
using System.Security.Policy;
using System.ServiceModel;
using System.Windows;
using System.Windows.Controls;
using Microsoft.ServiceModel.Samples;

namespace Microsoft.PartialTrust.Samples
{
    public partial class Page1 : Page
    {

        //Default Service URI.
        static readonly string DEFAULT_SERVICE_URI = "http://localhost/servicemodelsamples/service.svc";

        // Generated with Svcutil.exe to enable partial trust access.
        // Keep proxy for lifetime of the Window.
        CalcPlusClient proxy;

        //Endpoint address of host service.
        EndpointAddress serviceAddress;

        public Page1()
        {
            InitializeComponent();

            // Keep the page alive between navigations
            // to keep the proxy alive between navigations.
            KeepAlive = true;
        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            // Handle button click.
            _equalsButton.Click += _equalsButton_Click;

            // In partial trust, only allowed to call back into the originating server.
            string serviceUri = GetUpdateLocationUrl();
            serviceAddress = new EndpointAddress(serviceUri);
            proxy = new CalcPlusClient(new WSHttpBinding(SecurityMode.None), serviceAddress);

            _numberTextBlock.Text = serviceUri;
        }

        // Update a URL to use the host of the ClickOnce update location to enable
        // partial-trust clients to call back to their originating server
        // (the only place they are allowed to call by default). For example, if this application is launched
        // from "http://deployedMachine/app.application" and the URL baked into the proxy is for localhost,
        // then "http://localhost/foo.svc" becomes "http://deployedMachine/foo.svc"
        static string GetUpdateLocationUrl()
        {
            // If not launched using ClickOnce, return the original URL.
            if (!ApplicationDeployment.IsNetworkDeployed)
            {
                return DEFAULT_SERVICE_URI;
            }

            // Extract the host from the update location.
            Uri updateLocation = ApplicationDeployment.CurrentDeployment.UpdateLocation;
            string host = Site.CreateFromUrl(updateLocation.AbsoluteUri).Name;

            // Update service URL to use update location host.
            UriBuilder updatedUrl = new UriBuilder(DEFAULT_SERVICE_URI);
            updatedUrl.Host = host;
            return updatedUrl.ToString();
        }

        void _equalsButton_Click(object sender, RoutedEventArgs e)
        {
            double operand1 = double.Parse(_operand1.Text, CultureInfo.InvariantCulture);
            double operand2 = double.Parse(_operand2.Text, CultureInfo.InvariantCulture);
            string opcode = ((TextBlock) _op.SelectedValue).Text;
            CalcResult result = null;
            // Start the call to the web service
            switch (opcode)
            {
                case "+":
                    result = proxy.Add(operand1, operand2);
                    break;

                case "-":
                    result = proxy.Subtract(operand1, operand2);
                    break;

                case "*":
                    result = proxy.Multiply(operand1, operand2);
                    break;

                case "/":
                    result = proxy.Divide(operand1, operand2);
                    break;
            }

            if (null == result)
            {
                _numberTextBlock.Text = "";
                _wordsTextBlock.Text = "(Failed to execute operation)";
            }
            else
            {
                _numberTextBlock.Text = result.Number.ToString();
                _wordsTextBlock.Text = result.Words;
            }
        }

        void Page1Parent_Closed(object sender, EventArgs e)
        {
            // Clean up proxy when Window closes.
            ((IClientChannel) proxy).Close();
            proxy.ChannelFactory.Close();
        }
    }
}
