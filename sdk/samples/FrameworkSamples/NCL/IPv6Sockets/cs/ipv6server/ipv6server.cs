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
    static class IPv6Server
    {
        static void Main()
        {
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, 5150);
            Socket serverSocket = 
                new Socket(
                    localEndPoint.AddressFamily, 
                    SocketType.Stream, 
                    ProtocolType.Tcp);
            try
            {
                serverSocket.Bind(localEndPoint);
                serverSocket.Listen(int.MaxValue);
                Console.WriteLine("Server started.");

                while (true)
                {
                    try
                    {
                        Socket clientSocket = serverSocket.Accept();
                        Console.WriteLine(
                            "Accepted connection from: {0}",
                            clientSocket.RemoteEndPoint.ToString());

                        StreamReader reader = null;
                        StreamWriter writer = null;
                        try
                        {
                            NetworkStream networkStream = 
                                new NetworkStream(clientSocket);
                            reader = new StreamReader(networkStream);
                            string clientMessage = reader.ReadLine();
                            Console.WriteLine(
                                "Server received message: {0}", clientMessage);

                            writer = new StreamWriter(networkStream);
                            string serverMessage = "Hello!";
                            writer.WriteLine(serverMessage);
                            writer.Flush();
                            Console.WriteLine(
                                "Server sent message: {0}", serverMessage);
                        }
                        catch (SocketException ex)
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
                    }
                    catch (SocketException ex)
                    {
                        Console.WriteLine(
                            "Server could not accept connection: {0}",
                            ex.Message);
                    }
                }
            }
            catch (SocketException ex)
            {
                Console.WriteLine("Failed to start server: {0}", ex.Message);
            }
            finally
            {
                if (serverSocket != null)
                    serverSocket.Close();
            }
        }
    }
}
