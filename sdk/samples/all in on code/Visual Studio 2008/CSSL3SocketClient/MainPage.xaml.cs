/****************************** Module Header ******************************\
* Module Name:                 MainPage.xaml.cs
* Project:                     CSSL3SocketClient
* Copyright (c) Microsoft Corporation.
* 
* Silverlight socket client codebehind file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using CSSL3SocketClient;

namespace CSSL3SocketClient
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
        }

        ~MainPage()
        {
            if (_client != null)
                _client.Close();
        }

        SocketClient _client;

        // Handle "Connect" button click event.
        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            btnConnect.IsEnabled = false;
            OpenSocketClientAsync(tboxServerAddress.Text);
        }

        // Handle "Send" button click event.
        private void btnSend_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (_client != null)
                {
                    _client.SendAsync(tboxMessage.Text);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception occured when sending message:" + ex.Message);
                CloseSocketClient();
                return;
            }
        }

        // Close SocketClient
        void CloseSocketClient()
        {
            if (_client != null)
            {
                _client.Close();
                _client = null;
            }

            // Update UI
            btnConnect.IsEnabled = true;
            btnSend.IsEnabled = false;
        }

        // Create SocketClient and connect to server
        //
        // For convience, we use 127.0.0.1 address to connect socket server
        // at local. To let client access socket server at differnt machine
        // on network, input server ip address in textbox and click 'connect'.
        // To let socket server accessible from network, please see the 
        // comment in CCSL3SocketServer.
        bool OpenSocketClientAsync(string ip)
        {
            try
            {
                var endpoint = new IPEndPoint(
                    IPAddress.Parse(ip),
                    4502);

                _client = new SocketClient(
                    endpoint.AddressFamily,
                    SocketType.Stream,
                    ProtocolType.Tcp);

                // Register event
                _client.ClientConnected += new EventHandler<SocketMessageEventArgs>(_client_ClientConnected);
                _client.MessageReceived += new EventHandler<SocketMessageEventArgs>(_client_MessageReceived);
                _client.MessageSended += new EventHandler<SocketMessageEventArgs>(_client_MessageSended);

                _client.ConnectAsync(endpoint);
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception occured when Connecting Socket: " + ex.Message);
                CloseSocketClient();
                return false;
            }
        }

        void _client_ClientConnected(object sender, SocketMessageEventArgs e)
        {
            Dispatcher.BeginInvoke(delegate
            {
                // If connect successful, start receiving message
                if (e.Error == null)
                {
                    try
                    {
                        _client.StartReceiving();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Exception occured when creating SocketClient:" + ex.Message);
                        CloseSocketClient();
                        return;
                    }
                    // Update UI
                    btnConnect.IsEnabled = false;
                    btnSend.IsEnabled = true;
                    tbSocketStatus.Text = "Connected";
                }
                else
                {
                    _client.Close();
                    btnConnect.IsEnabled = true;
                    tbSocketStatus.Text = "Connect failed: " + e.Error.Message;
                }
            });
        }

        // Handle message received event
        void _client_MessageSended(object sender, SocketMessageEventArgs e)
        {
            Dispatcher.BeginInvoke(delegate
            {
                if (e.Error == null)
                    tbSocketStatus.Text = "Sended";
                else
                {
                    tbSocketStatus.Text = "Send failed: " + e.Error.Message;
                    CloseSocketClient();
                }
            });
        }

        // Handle message sended event
        void _client_MessageReceived(object sender, SocketMessageEventArgs e)
        {
            Dispatcher.BeginInvoke(delegate
            {
                if (e.Error == null)
                {
                    tbSocketStatus.Text = "Received";
                    lb1.Items.Insert(0, e.Data);
                }
                else
                {
                    tbSocketStatus.Text = "Receive failed: " + e.Error.Message;
                    CloseSocketClient();
                }
            });
        }
    }
}
