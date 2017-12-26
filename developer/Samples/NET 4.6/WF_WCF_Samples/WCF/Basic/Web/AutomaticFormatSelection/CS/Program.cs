//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Net;
using System.ServiceModel.Web;

namespace Microsoft.Samples.AutomaticFormatSelection
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
                    client.QueryString["s"] = "hello";

                    Console.WriteLine("");

                    // Specify response format for GET using Accept header
                    Console.WriteLine("Calling EchoWithGet via HTTP GET and Accept header application/xml: ");
                    client.Headers[HttpRequestHeader.Accept] = "application/xml";
                    Console.WriteLine(client.DownloadString("EchoWithGet"));
                    Console.WriteLine("Calling EchoWithGet via HTTP GET and Accept header application/json: ");
                    client.Headers[HttpRequestHeader.Accept] = "application/json";
                    Console.WriteLine(client.DownloadString("EchoWithGet"));

                    Console.WriteLine("");

                    // Specify response format for GET using 'format' query string parameter
                    Console.WriteLine("Calling EchoWithGet via HTTP GET and format query string parameter set to xml: ");
                    client.QueryString["format"] = "xml";
                    Console.WriteLine(client.DownloadString("EchoWithGet"));
                    Console.WriteLine("Calling EchoWithGet via HTTP GET and format query string parameter set to json: ");
                    client.QueryString["format"] = "json";
                    Console.WriteLine(client.DownloadString("EchoWithGet"));

                    client.Headers[HttpRequestHeader.Accept] = null;

                    // Do POST in XML and JSON and get the response in the same format as the request
                    Console.WriteLine("Calling EchoWithPost via HTTP POST and request in XML format: ");
                    client.Headers[HttpRequestHeader.ContentType] = "application/xml";
                    Console.WriteLine(client.UploadString("EchoWithPost", "<string xmlns=\"http://schemas.microsoft.com/2003/10/Serialization/\">bye</string>"));
                    Console.WriteLine("Calling EchoWithPost via HTTP POST and request in JSON format: ");
                    client.Headers[HttpRequestHeader.ContentType] = "application/json";
                    Console.WriteLine(client.UploadString("EchoWithPost", "\"bye\""));

                    Console.WriteLine("Press any key to terminate");
                    Console.ReadLine();
                }
            }
        }
    }
}
