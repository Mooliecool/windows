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
using System.Security.Principal;
using System.Security.Authentication;

namespace Microsoft.Samples.SecureStreams
{
    static class NegotiateClient
    {
        static void Main()
        {
            // You should substitute in the SPN of the server you want to
            // authenticate to using Kerberos.  Otherwise, NTLM authentication
            // will be used.
            Connect("localhost", "howdy", "domain\\user");
        }

        static void Connect(
            String server, String message, string servicePrincipalName)
        {
            NegotiateStream negotiateStream = null;
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

                // Get a client stream for reading and writing.
                // Wrap it in a NegotiateStream.
                negotiateStream = new NegotiateStream(client.GetStream());

                // This example uses the SPN which is required for Kerberos.  
                // If you don't know your service principal name, you can do 
                // NTLM authentication by commenting out the line below
                negotiateStream.AuthenticateAsClient(
                    CredentialCache.DefaultNetworkCredentials,
                    servicePrincipalName,
                    ProtectionLevel.EncryptAndSign,
                    TokenImpersonationLevel.Impersonation);
                // And then uncomment this line
                // authenticatedStream.AuthenticateAsClient();

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
                }

                // Send the message to the connected TcpServer. 
                negotiateStream.Write(data, 0, data.Length);

                Console.WriteLine("Sent: {0}", message);

                // Receive the TcpServer.response:
                // Buffer to store the response bytes.
                data = new Byte[256];

                // String to store the response ASCII representation.
                String responseData = String.Empty;

                // Read the first batch of the TcpServer response bytes.
                Int32 bytes = negotiateStream.Read(data, 0, data.Length);
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
                if (negotiateStream != null)
                    negotiateStream.Close();
            }

            Console.WriteLine("\n Press Enter to continue...");
            Console.Read();
        }

    }
}
