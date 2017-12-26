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
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace Microsoft.Samples.IPv6Sockets
{
    static class IPv6Client
    {
        static void Main(string[] args)
        {
            if (args.Length < 1) {
                DisplayUsage();
                return;
            }

            string serverDnsName = args[0];
            try
            {
                IPHostEntry resolvedServer = Dns.GetHostEntry(serverDnsName);
                for (int i = 0; i < resolvedServer.AddressList.Length; i++)
                {
                    IPAddress address = resolvedServer.AddressList[i];
                    IPEndPoint serverEndPoint = new IPEndPoint(address, 5150);
                    Socket tcpSocket = 
                        new Socket(
                            address.AddressFamily, 
                            SocketType.Stream, 
                            ProtocolType.Tcp);
                    try
                    {
                        tcpSocket.Connect(serverEndPoint);
                        StreamWriter writer = null;
                        StreamReader reader = null;
                        try
                        {
                            NetworkStream networkStream = 
                                new NetworkStream(tcpSocket);
                            writer = new StreamWriter(networkStream);
                            string clientMessage = "Hi there!";
                            writer.WriteLine(clientMessage);
                            writer.Flush();
                            Console.WriteLine(
                                "Client sent message: {0}", clientMessage);

                            reader = new StreamReader(networkStream);
                            string serverMessage = reader.ReadLine();
                            Console.WriteLine(
                                "Client received message: {0}", serverMessage);
                        }
                        catch (SocketException ex)
                        {
                            Console.WriteLine(
                                "Message exchange failed: {0}", ex.Message);
                        }
                        catch (IOException ex)
                        {
                            Console.WriteLine(
                                "Message exchange failed: {0}", ex.Message);
                        }
                        finally
                        {
                            if (reader != null)
                                reader.Close();
                            if (writer != null)
                                writer.Close();
                        }
                        break;
                    }
                    catch (SocketException)
                    {
                        if (tcpSocket != null)
                            tcpSocket.Close();
                        if (i == resolvedServer.AddressList.Length - 1)
                            Console.WriteLine(
                                "Failed to connect to the server.");
                    }
                }

            }
            catch (SocketException ex)
            {
                Console.WriteLine(
                    "Could not resolve server DNS name: {0}", ex.Message);
            }
        }

        private static void DisplayUsage()
        {
            Console.WriteLine("IPv6Client server_name");
        }
    }
}
