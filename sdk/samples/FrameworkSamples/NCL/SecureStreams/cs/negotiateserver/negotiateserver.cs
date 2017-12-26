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
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Net.Security;
using System.Security.Authentication;
using System.Security.Principal;

namespace Microsoft.Samples.SecureStreams
{
    static class NegotiateServer
    {
        public static void Main()
        {
            NegotiateStream negotiateStream = null;
            try
            {
                // Set the TcpListener on port 13000.
                Int32 port = 13000;
                IPAddress localAddr = IPAddress.Loopback;

                // TcpListener server = new TcpListener(port);
                TcpListener server = new TcpListener(localAddr, port);

                // Start listening for client requests.
                server.Start();

                // Buffer for reading data
                Byte[] bytes = new Byte[256];
                String data = null;

                // Enter the listening loop.
                while (true)
                {
                    Console.Write("Waiting for a connection... ");

                    // Perform a blocking call to accept requests.
                    // You could also user server.AcceptSocket() here.
                    TcpClient client = server.AcceptTcpClient();
                    Console.WriteLine("Connected!");

                    data = null;

                    // Get a stream object for reading and writing
                    // Wrap it in a NegotiateStream.
                    negotiateStream = new NegotiateStream(client.GetStream());
                    negotiateStream.AuthenticateAsServer();

                    if (negotiateStream.IsAuthenticated)
                    {
                        Console.WriteLine(
                            "IsAuthenticated: {0}",
                            negotiateStream.IsAuthenticated);
                        Console.WriteLine(
                            "IsMutuallyAuthenticated: {0}",
                            negotiateStream.IsMutuallyAuthenticated);
                        Console.WriteLine(
                            "IsEncrypted: {0}",
                            negotiateStream.IsEncrypted);
                        Console.WriteLine(
                            "IsSigned: {0}",
                            negotiateStream.IsSigned);
                        Console.WriteLine(
                            "IsServer: {0}",
                            negotiateStream.IsServer);
                        IIdentity remoteIdentity =
                            negotiateStream.RemoteIdentity;
                        Console.WriteLine(
                            "Client identity: {0}",
                            remoteIdentity.Name);
                        Console.WriteLine(
                            "Authentication Type: {0}",
                            remoteIdentity.AuthenticationType);
                    }

                    int i;

                    // Loop to receive all the data sent by the client.
                    while (
                        (i = negotiateStream.Read(bytes, 0, bytes.Length)) 
                        != 0)
                    {
                        // Translate data bytes to a string.
						// The encoding used is application specific.
                        data = 
                            System.Text.Encoding.ASCII.GetString(bytes, 0, i);
                        Console.WriteLine("Received: {0}", data);

                        // Process the data sent by the client.
                        data = data.ToUpper(
                            System.Globalization.CultureInfo.CurrentCulture);

                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(data);

                        // Send back a response.
                        negotiateStream.Write(msg, 0, msg.Length);
                        Console.WriteLine("Sent: {0}", data);
                    }
                }
            }
            catch (AuthenticationException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (SocketException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (IOException ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                if (negotiateStream != null)
                    negotiateStream.Close();
            }

            Console.WriteLine("\nHit enter to continue...");
            Console.Read();
        }
    }

}
