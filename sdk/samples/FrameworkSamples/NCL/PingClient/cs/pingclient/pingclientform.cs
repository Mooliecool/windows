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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.Net.NetworkInformation;
using System.Globalization;

namespace Microsoft.Samples.PingClient
{
    partial class PingClientForm : Form
    {
        Ping pingClient = new Ping();

        public PingClientForm()
        {
            InitializeComponent();
            pingClient.PingCompleted +=
                new PingCompletedEventHandler(pingClient_PingCompleted);
        }

        private void pingClient_PingCompleted(object sender, PingCompletedEventArgs e)
        {
            // Check to see if an error occurred.  If no error, then display 
            // the address used and the ping time in milliseconds.
            if (e.Error == null)
            {
                if (e.Cancelled)
                {
                    pingDetailsTextBox.Text += "  Ping cancelled. \r\n";
                }
                else
                {
                    if (e.Reply.Status == IPStatus.Success)
                    {
                        pingDetailsTextBox.Text +=
                            "  " + e.Reply.Address.ToString() + " " +
                            e.Reply.RoundtripTime.ToString(
                            NumberFormatInfo.CurrentInfo) + "ms" + "\r\n";
                    }
                    else
                    {
                        pingDetailsTextBox.Text +=
                            "  " + GetStatusString(e.Reply.Status) + "\r\n";
                    }
                }
            }
            else
            {
                // Otherwise display the error.
                pingDetailsTextBox.Text += "  Ping error.\r\n";
                MessageBox.Show(
                    "An error occurred while sending this ping. " +
                    e.Error.InnerException.Message);
            }
            sendButton.Enabled = true;
        }

        private string GetStatusString(IPStatus status)
        {
            switch (status)
            {
                case IPStatus.Success:
                    return "Success.";
                case IPStatus.DestinationHostUnreachable:
                    return "Destination host unreachable.";
                case IPStatus.DestinationNetworkUnreachable:
                    return "Destination network unreachable.";
                case IPStatus.DestinationPortUnreachable:
                    return "Destination port unreachable.";
                case IPStatus.DestinationProtocolUnreachable:
                    return "Destination protocol unreachable.";
                case IPStatus.PacketTooBig:
                    return "Packet too big.";
                case IPStatus.TtlExpired:
                    return "TTL expired.";
                case IPStatus.ParameterProblem:
                    return "Parameter problem.";
                case IPStatus.SourceQuench:
                    return "Source quench.";
                case IPStatus.TimedOut:
                    return "Timed out.";
                default:
                    return "Ping failed.";
            }
        }

        private void sendButton_Click(object sender, EventArgs e)
        {
            // Select all the text in the address box.
            addressTextBox.SelectAll();

            if (addressTextBox.Text.Length != 0)
            {
                // Disable the Send button.
                sendButton.Enabled = false;

                pingDetailsTextBox.Text +=
                    "Pinging " + addressTextBox.Text + " . . .\r\n";

                // Send ping request.
                pingClient.SendAsync(addressTextBox.Text, null);
            }
            else
            {
                MessageBox.Show("Please enter an IP address or host name.");
            }

        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            // Cancel any pending pings.
            pingClient.SendAsyncCancel();
        }
    }
}