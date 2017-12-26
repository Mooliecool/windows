//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Windows.Forms;

namespace Microsoft.Samples.WCF.UptimeMonitor.Client {
    public partial class MainForm : Form {
        public MainForm() {
            InitializeComponent();
        }

        void updateButton_Click(object sender, EventArgs e) {
            // Make sure a server name was provided.
            if (hostBox.Text.Trim().Length == 0) {
                MessageBox.Show("Specify a server name first.");
                return;
            }
            // Since creating a factory and sending a message can take
            // a noticeable amount of time, let the user know we're working
            // by setting the wait cursor.
            Cursor.Current = Cursors.WaitCursor;
            // Make sure the wait cursor has a chance to show up by calling
            // DoEvents.
            Application.DoEvents();
            try {
                // Construct our channel factory, which we'll use to send
                // uptime requests.
                // In a production app, we would normally construct the 
                // factory just once for the app, and reuse it to send 
                // every message.
                var binding = new BasicHttpBinding();
                var parameters = new BindingParameterCollection();
                var factory =
                    binding.BuildChannelFactory<IRequestChannel>(parameters);
                factory.Open();
                // We send the message in a try block to guarantee that we
                // close the channel and factory when we're done.
                try {
                    // Open the channel to the server the user provided,
                    // on the hard-coded port number.
                    var addr = new EndpointAddress("http://" + hostBox.Text +
                        ":" + UptimeRequest.ListeningPort);
                    var channel = factory.CreateChannel(addr);
                    channel.Open();
                    try {
                        // Formulate our request, which is really simple.
                        var request = new UptimeRequest();
                        // Serialize that request into a WCF message.
                        Message requestMessage = Message.CreateMessage(
                            binding.MessageVersion, "urn:UptimeRequest",
                            new UptimeRequest(),
                            MessageSerializer.RequestSerializer
                        );
                        // Send the request and block waiting for a response.
                        var responseMessage = channel.Request(requestMessage);
                        // Deserialize the response.
                        var response = responseMessage.GetBody<UptimeResponse>
                            (MessageSerializer.ResponseSerializer);
                        // Read the response and show results to user.
                        uptimeLabel.Text = response.Uptime.ToString();
                        timestampLabel.Text = response.Timestamp.ToString();
                    } finally {
                        channel.Close();
                    }
                } finally {
                    factory.Close();
                }
            } catch (Exception ex) {
                // don't catch fatal exceptions
                if (ex is OutOfMemoryException || ex is StackOverflowException)
                    throw;
                // Display the error to the user to decide what to do.
                if (MessageBox.Show(ex.Message, "Error querying server uptime",
                    MessageBoxButtons.RetryCancel, MessageBoxIcon.Exclamation,
                    MessageBoxDefaultButton.Button1) == DialogResult.Retry) {
                    updateButton_Click(sender, e); // try again
                }
            } finally {
                // Reset wait cursor in finally block to ensure it goes away
                // even in case of failure.
                Cursor.Current = Cursors.Default;
            }
        }
    }
}