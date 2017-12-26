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
using System.Net;
using System.IO;
using System.Text;
using System.Globalization;

// The HttpRequestListener example demonstrates how to add the ability to 
// process HTTP requests from within a client application.
namespace Microsoft.Samples.HttpListener
{
    static class HttpRequestListener
    {
        public static void Main()
        {
            string[] prefixes = new string[1];
            prefixes[0] = "http://*:8080/HttpListener/";
            ProcessRequests(prefixes);
        }


        private static void ProcessRequests(string[] prefixes)
        {
            if (!System.Net.HttpListener.IsSupported)
            {
                Console.WriteLine(
                    "Windows XP SP2, Server 2003, or higher is required to " +
                    "use the HttpListener class.");
                return;
            }
            // URI prefixes are required,
            if (prefixes == null || prefixes.Length == 0)
                throw new ArgumentException("prefixes");

            // Create a listener and add the prefixes.
            System.Net.HttpListener listener = new System.Net.HttpListener();
            foreach (string s in prefixes)
            {
                listener.Prefixes.Add(s);
            }

            try
            {
                // Start the listener to begin listening for requests.
                listener.Start();
                Console.WriteLine("Listening...");

                // Set the number of requests this application will handle.
                int numRequestsToBeHandled = 10;

                for (int i = 0; i < numRequestsToBeHandled; i++)
                {
                    HttpListenerResponse response = null;
                    try
                    {
                        // Note: GetContext blocks while waiting for a request.
                        HttpListenerContext context = listener.GetContext();

                        // Create the response.
                        response = context.Response;
                        string responseString =
                            "<HTML><BODY>The time is currently " +
                            DateTime.Now.ToString(
                            DateTimeFormatInfo.CurrentInfo) +
                            "</BODY></HTML>";
                        byte[] buffer =
                            System.Text.Encoding.UTF8.GetBytes(responseString);
                        response.ContentLength64 = buffer.Length;
                        System.IO.Stream output = response.OutputStream;
                        output.Write(buffer, 0, buffer.Length);
                    }
                    catch (HttpListenerException ex)
                    {
                        Console.WriteLine(ex.Message);
                    }
                    finally
                    {
                        if (response != null)
                            response.Close();
                    }
                }
            }
            catch (HttpListenerException ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                // Stop listening for requests.
                listener.Close();
                Console.WriteLine("Done Listening.");
            }
        }
    }
}