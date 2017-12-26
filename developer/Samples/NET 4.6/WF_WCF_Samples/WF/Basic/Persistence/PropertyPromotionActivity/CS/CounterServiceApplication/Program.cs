//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Persistence;
using System.Activities.Statements;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Activities.Description;
using System.ServiceModel.Description;
using System.Xml.Linq;
using System.Xaml;

namespace Microsoft.Samples.PropertyPromotionActivity
{
    // This sample needs to be run with Admin privileges.
    // Otherwise the channel listener is not allowed to open ports.
    // See sample documentation for details.

    class Program
    {
        const string CounterServiceContractName = "ICountingWorkflow";
        const string ServiceEndpointAddress = "http://localhost:8080/CountingService";

        [ServiceContract(Name = CounterServiceContractName)]
        public interface ICountingWorkflow
        {
            [OperationContract(IsOneWay = true, Action = "Start")]
            void Start();
        }

        static void Main(string[] args)
        {
            // Xamlx file is a workflow that waits for a client to call its Start method.
            // Once called, the workflow counts from 0 to 29, incrementing the counter
            // every 2 seconds. After every counter increment the workflow persists,
            // updating the promoted properties in the [InstancePromotedProperties] view.
            object serviceImplementation = XamlServices.Load("CounterService.xamlx");

            using (WorkflowServiceHost host = new WorkflowServiceHost(serviceImplementation))
            {
                host.Open();

                // Create a client that sends a message to create an instance of the workflow.
                ICountingWorkflow client = ChannelFactory<ICountingWorkflow>.CreateChannel(new BasicHttpBinding(), new EndpointAddress(ServiceEndpointAddress));
                client.Start();

                Console.WriteLine("(Press [Enter] at any time to terminate host)");
                Console.ReadLine();

                host.Close();
            }
        }
    }
}

