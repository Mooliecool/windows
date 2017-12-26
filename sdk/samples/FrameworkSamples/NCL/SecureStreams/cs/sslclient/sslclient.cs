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
using System.Security.Cryptography.X509Certificates;

namespace Microsoft.Samples.SecureStreams
{
    static class SslClient
    {
        static void Main()
        {
            Connect("localhost", "howdy");
        }

        static void Connect(String server, String message)
        {
            SslStream sslStream = null;
            try
            {
                // Create a TcpClient.
                // Note, for this client to work you need to have a TcpServer 
                // connected to the same address as specified by the server, 
                // port combination.
                Int32 port = 13000;
                TcpClient client = new TcpClient(server, port);

				// Translate the message into a byte array.
				// The encoding used is application specific.
				Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

				RemoteCertificateValidationCallback callback =
                    new RemoteCertificateValidationCallback(
                    OnCertificateValidation);

                // Get a client stream for reading and writing.
                // Wrap it in an SslStream. 
                sslStream = new SslStream(client.GetStream(), false, callback);
                sslStream.AuthenticateAsClient("localhost");

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

                // Send the message to the connected TcpServer. 
                sslStream.Write(data, 0, data.Length);

                Console.WriteLine("Sent: {0}", message);

                // Receive the TcpServer.response.

                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                Int32 bytes = sslStream.Read(data, 0, data.Length);
                responseData =
                    System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                Console.WriteLine("Received: {0}", responseData);
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


            Console.WriteLine("\n Press Enter to continue...");
            Console.Read();
        }

        private static bool OnCertificateValidation(
            object sender,
            X509Certificate certificate,
            X509Chain chain,
            SslPolicyErrors sslPolicyErrors)
        {
            // This sample returns true always so that you can see how
            // it works.  You should change logic in the validator to only
            // return true if you've inspected the certificate and approve it.
            return true;
        }

    }
}
