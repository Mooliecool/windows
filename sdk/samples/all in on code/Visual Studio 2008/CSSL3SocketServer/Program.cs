/****************************** Module Header ******************************\
* Module Name:              Program.cs
* Project:                  CSSL3SocketServer
* Copyright (c) Microsoft Corporation.
* 
* Socket Server application code file, can serve Silverlight and normal socket
* Client.
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
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.IO;
using CSSL3SocketServer;

namespace CSSL3SocketServer
{
    class Program
    {
        static byte[] policybytes;
        static void Main(string[] args)
        {
            // Read crossdomainpolicy.xml file to byte array.
            var filestream = new FileStream("policy.xml", FileMode.Open, FileAccess.Read);
            policybytes = new byte[filestream.Length];
            filestream.Read(policybytes, 0, (int)filestream.Length);
            filestream.Close();

            // Initialize policy socket listener
            var socketp = new SocketListener();
            socketp.ListenAsync(943, socketp_SocketConnected);

            // Initialize socket listener
            var socketp2 = new SocketListener();
            socketp2.ListenAsync(4502, socketp2_SocketConnected);

            Console.Read();
            
        }

        // Client connected
        static void socketp2_SocketConnected(Socket sock)
        {
            // Create new thread to handle client communication
            new Thread(
                new ThreadStart(delegate
            {
                // Initialize SocketClient
                var client = new SocketClient(sock);
                try
                {
                    client.MessageReceived += new EventHandler<SocketMessageEventArgs>(client_MessageReceived);
                    client.MessageSended += new EventHandler<SocketMessageEventArgs>(client_MessageSended);
                    // Ready to receive 
                    client.StartReceiving();
                    Console.WriteLine("Client connected.");
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Exception occured when start receiving message:\n" + ex.Message);
                    client.Close();
                }
            })).Start();
        }

        // Handle message sended event
        static void client_MessageSended(object sender, CSSL3SocketServer.SocketMessageEventArgs e)
        {
            if (e.Error != null)
            {
                Console.WriteLine("Message send failed: " + e.Error.Message);
                ((SocketClient)sender).Close();
                Console.WriteLine("Client disconnected.");
            }
            else
                Console.WriteLine("Message send successful");
        }

        // Handle message received event
        static void client_MessageReceived(object sender, CSSL3SocketServer.SocketMessageEventArgs e)
        {
            if (e.Error != null)
            {
                Console.WriteLine("Message receive failed: " + e.Error.Message);
                ((SocketClient)sender).Close();
                Console.WriteLine("Client disconnected.");
            }
            else
            {
                // Idle 1 second and send message back
                Console.WriteLine("Message Received: " + e.Data);
                Thread.Sleep(1000);
                SendMessage(sender as SocketClient,
                    "Handled: " + e.Data);
            }
        }

        // User SocketClient to send message
        static void SendMessage(SocketClient client, string data)
        {
            try
            {
                client.SendAsync(data);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Exception occured when sending message:\n" + ex.Message);
                client.Close();
                Console.WriteLine("Client disconnected.");
            }
        }

        // Before Silverlight socket client connected to socket server,
        // it will connect to server 943 port to request policy.
        static readonly string POLICY_REQUEST = "<policy-file-request/>";
        static void socketp_SocketConnected(Socket sock)
        {
            // Run at other thread.
            new Thread(
                new ThreadStart(delegate
                    {
                        try
                        {
                            Console.WriteLine("Policy client connected.");
                            byte[] receivebuffer = new byte[1000];
                            var receivedcount = sock.Receive(receivebuffer);
                            string requeststr = Encoding.UTF8.GetString(receivebuffer, 0, receivedcount);

                            // Check if client request server policy
                            if (requeststr == POLICY_REQUEST)
                            {
                                // Send policy
                                sock.Send(policybytes, 0, policybytes.Length, SocketFlags.None);
                            }
                        }
                        catch
                        {
                            Console.WriteLine("Policy socket client get an error.");
                        }
                        finally
                        {
                            sock.Close();
                            Console.WriteLine("Policy client disconnected.");
                        }
                    })).Start();
        }
    }
}
