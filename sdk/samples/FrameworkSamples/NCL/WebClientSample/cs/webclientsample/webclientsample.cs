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

#region Using directives

using System;
using System.Text;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
#endregion

namespace Microsoft.Samples.Networking.WebClientSample
{
    static class Program
    {
        private static void Main(string[] args)
        {
            string serverName;
            // This example assumes the site uses anonymous logon.
            NetworkCredential credentials = 
                new NetworkCredential("anonymous", "");
            if (args.Length != 1 || 
                args[0].Equals("/?") || 
                args[0].Equals("/help"))
            {
                PrintUsage();
                return;
            }
            else
            {
                serverName = args[0];
            }

            DemonstrateWebClient(serverName, credentials);
        }

        private static void PrintUsage()
        {
            Console.WriteLine("To run the sample:");
            Console.WriteLine("    WebClientSample.exe <FTP server name>");
            Console.WriteLine("To display usage:");
            Console.WriteLine("    WebClientSample.exe [/? | /help]");
        }

        // Read the name of the file which needs to be uploaded or downloaded.
        private static string ReadFileName(string operation)
        {
            // Operation will be download or upload.
            Console.Write(
                "Enter the name of the file to " + 
                operation + 
                " relative to the logon directory: ");

            // Returns the name of the file read.
            return Console.ReadLine();
        }

        // Read in some text data to upload.
        private static string ReadData()
        {
            Console.WriteLine("Enter some text data to upload:");
            return Console.ReadLine();
        }

        // Demonstrates how to download and upload a file using WebClient.
        // Uses UploadFile as well as the UploadData methods.
        private static void DemonstrateWebClient(
            string serverName, NetworkCredential credentials)
        {
            string serverAddress = "ftp://" + serverName + "/";

            // Create a new WebClient object and set the credentials.
            WebClient webClient = new WebClient();
            webClient.Credentials = credentials;

            // Download file.
            try
            {
                string fileName = ReadFileName("download");
                string downloadAddress = serverAddress + fileName;
                Console.WriteLine("Downloading file " + downloadAddress);
                
                // The string passed in should include the URI path that has 
                // the file name to be downloaded.
                webClient.DownloadFile(downloadAddress, fileName);
            }
            catch (WebException ex)
            {
                Console.WriteLine(ex.Message);
            }

            // Upload file.
            try
            {
                string fileName = ReadFileName("upload");
                string uploadAddress = serverAddress + fileName;
                Console.WriteLine("Uploading file " + uploadAddress);
                webClient.UploadFile(uploadAddress, fileName);
            }
            catch (WebException ex)
            {
                Console.WriteLine(ex.Message);
            }

            // Upload file as data.
            try
            {
                string fileName = ReadFileName("upload to");
                string data = ReadData();
                string uploadAddress = serverAddress + fileName;
                
                // Apply ASCII Encoding to obtain the string as a byte array.
                byte[] byteArray = Encoding.UTF8.GetBytes(data);
                Console.WriteLine("Uploading data to " + uploadAddress);
                
                // Upload the data, and using the specified verb. 
                // The STOR verb is an FTP command creates a file on the 
                // server using the specified data.
                byte[] responseArray = 
                    webClient.UploadData(
                        uploadAddress, 
                        WebRequestMethods.Ftp.UploadFile, 
                        byteArray);
                
                string response;
                if (responseArray.Length == 0) 
                {
                    response = "<nothing>";
                }
                else 
                {
                    // Decode and display the response.
                    response = Encoding.UTF8.GetString(responseArray);
                }

                Console.WriteLine();
                Console.WriteLine("Response received was {0}", response);
            }
            catch (WebException ex)
            {
                Console.WriteLine(ex.Message);
            }

            webClient.Dispose();
        }
    }
}
