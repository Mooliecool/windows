//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Description;

namespace Microsoft.WebProgrammingModel.Samples
{
    class Program
    {
        static void Main(string[] args)
        {
            ServiceHost host = new ServiceHost(typeof(Service), new Uri("http://localhost:8000/FormTest"));

            ServiceEndpoint endpoint = host.AddServiceEndpoint(typeof(Service), new WebHttpBinding(), "");
            endpoint.Behaviors.Add(new FormProcessingBehavior());

            ServiceMetadataBehavior smb = host.Description.Behaviors.Find<ServiceMetadataBehavior>();

            if (smb != null)
            {
                smb.HttpGetEnabled = false;
                smb.HttpsGetEnabled = false;
            }

            ServiceDebugBehavior sdb = host.Description.Behaviors.Find<ServiceDebugBehavior>();
            if (sdb != null)
            {
                sdb.HttpHelpPageEnabled = false;
            }

            host.Open();

            Console.WriteLine("The service is open and listening. To interact with the service,");
            Console.WriteLine("navigate to http://localhost:8000/FormTest with a web browser.");

            Console.ReadLine();
        }
    }
}
