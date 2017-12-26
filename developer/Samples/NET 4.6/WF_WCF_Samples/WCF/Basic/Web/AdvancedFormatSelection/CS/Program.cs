//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Net;
using System.ServiceModel.Web;

namespace Microsoft.Samples.AdvancedFormatSelection
{
    class Program
    {
        static void Main(string[] args)
        {
            Uri baseAddress = new Uri("http://localhost:8000");
            Console.WriteLine("Service is hosted at: " + baseAddress.AbsoluteUri);
            Console.WriteLine("Service help page is at: " + baseAddress.AbsoluteUri + "help");

            using (WebServiceHost host = new WebServiceHost(typeof(Service), baseAddress))
            {
                //WebServiceHost will automatically create a default endpoint at the base address using the 
                //WebHttpBinding and the WebHttpBehavior, so there's no need to set that up explicitly
                host.Open();

                using (WebClient client = new WebClient())
                {
                    client.BaseAddress = baseAddress.AbsoluteUri;
                    client.QueryString["list"] = "apple,banana,grapes,orange";

                    Console.WriteLine();

                    // Specify response format for GET using 'format' query string parameter
                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and Accept header application/xml: ");
                    client.Headers[HttpRequestHeader.Accept] = "application/xml";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and Accept header application/json: ");
                    client.Headers[HttpRequestHeader.Accept] = "application/json";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and Accept header application/atom+xml: ");
                    client.Headers[HttpRequestHeader.Accept] = "application/atom+xml";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and Accept header application/xhtml+xml: ");
                    client.Headers[HttpRequestHeader.Accept] = "application/xhtml+xml";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("");

                    // Specify response format for GET using 'format' query string parameter
                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and format query string parameter set to xml: ");
                    client.QueryString["format"] = "xml";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and format query string parameter set to json: ");
                    client.QueryString["format"] = "json";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and format query string parameter set to atom: ");
                    client.QueryString["format"] = "atom";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Calling EchoListWithGet via HTTP GET and format query string parameter set to xhtml: ");
                    client.QueryString["format"] = "xhtml";
                    Console.WriteLine(client.DownloadString("EchoListWithGet"));
                    Console.WriteLine("");

                    Console.WriteLine("Browse to http://localhost:8000/EchoListWithGet?list=apple,banana,onion,pizza&format=jpeg to see the response as a jpeg.");
                    Console.WriteLine("");

                    Console.WriteLine("Press any key to terminate");
                    Console.ReadLine();
                }
            }
        }
    }
}
