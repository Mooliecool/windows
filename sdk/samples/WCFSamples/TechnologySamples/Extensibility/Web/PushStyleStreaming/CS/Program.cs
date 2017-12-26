//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Web;

namespace Microsoft.WebProgrammingModel.Samples
{
    class Program
    {
        static void Main(string[] args)
        {
            WebServiceHost host = new WebServiceHost(typeof(ImageGenerationService), new Uri("http://localhost:8000"));
            host.AddServiceEndpoint(typeof(ImageGenerationService), new WebHttpBinding(), "");
            host.Open();

            Console.WriteLine("This output of this sample is intended to be viewed in a web browser.");
            Console.WriteLine("To interact with this sample, navigate to the following URL's while this program is running: ");
            Console.WriteLine("    http://localhost:8000/images?text=Hello, world!");
            Console.WriteLine("    http://localhost:8000/text?text=Hello, world!");

            Console.WriteLine("Open!");
            Console.ReadLine();
        }
    }
}
