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
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;

namespace Microsoft.Samples.SecureStreams
{
    static class SslServer
    {
        public static void Main()
        {
            SslStream sslStream = null;
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
                    sslStream = new SslStream(client.GetStream());

                    // The exportedcertificate.cer file would contain a 
                    // certificate with public key that came from a certificate
                    // in the local machine store.
                    X509Certificate certificate =
                           X509Certificate.CreateFromCertFile(
                           "exportedcertificate.cer");
                    sslStream.AuthenticateAsServer(certificate);

                    if (sslStream.IsAuthenticated)
                    {
                        Console.WriteLine(
                            "IsAuthenticated: {0}",
                            sslStream.IsAuthenticated);
                        Console.WriteLine(
                            "IsMutuallyAuthenticated: {0}",
                            sslStream.IsMutuallyAuthenticated);
                        Console.WriteLine(
                            "IsEncrypted: {0}",
                            sslStream.IsEncrypted);
                        Console.WriteLine(
                            "IsSigned: {0}",
                            sslStream.IsSigned);
                        Console.WriteLine(
                            "IsServer: {0}",
                            sslStream.IsServer);
                    }

                    int i;

                    // Loop to receive all the data sent by the client.
                    while ((i = sslStream.Read(bytes, 0, bytes.Length)) != 0)
                    {
						// Translate data bytes to a string.
						// The encoding used is application specific.
						data =
							System.Text.Encoding.ASCII.GetString(bytes, 0, i);
                        Console.WriteLine("Received: {0}", data);

                        // Process the data sent by the client.
                        data = data.ToUpper(
                            System.Globalization.CultureInfo.CurrentCulture);

                        byte[] msg =
                            System.Text.Encoding.ASCII.GetBytes(data);

                        // Send back a response.
                        sslStream.Write(msg, 0, msg.Length);
                        Console.WriteLine("Sent: {0}", data);
                    }
                }
            }
            catch (CryptographicException ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(
                    "An error has occurred while using or authenticating " +
                    "this connection.  Check to make sure you've exported " +
                    "your certificate and that it is correctly referenced " +
                    "in CreateFromCertFile above.");
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
                if (sslStream != null)
                    sslStream.Close();
            }

            Console.WriteLine("\nHit enter to continue...");
            Console.Read();
        }
    }

}
