//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Description;
using System.Xml.Linq;

namespace Microsoft.Samples.Scenario
{
    class Program
    {
        static void Main(string[] args)
        {
            string baseAddr = "http://localhost:8080/OpScope";
            XName serviceContractName = XName.Get("IBankService", "http://bank.org");

            WorkflowService svc = new WorkflowService
            {
                Name = serviceContractName,
                Body = new BankService()
            };

            using (WorkflowServiceHost host = new WorkflowServiceHost(svc, new Uri(baseAddr)))
            {
                host.AddServiceEndpoint(serviceContractName, new BasicHttpContextBinding(), "");
                host.Description.Behaviors.Add(new ServiceMetadataBehavior() { HttpGetEnabled = true });
                Console.WriteLine("Starting ...");
                host.Open();

                Console.WriteLine("Service is waiting at: " + baseAddr);
                Console.WriteLine("Press [Enter] to exit");
                Console.ReadLine();
                host.Close();
            }
        }

    }
}
