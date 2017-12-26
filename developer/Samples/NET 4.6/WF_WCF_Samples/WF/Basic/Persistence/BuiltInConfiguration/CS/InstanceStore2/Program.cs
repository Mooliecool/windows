//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Activities;
using System.Activities.DurableInstancing;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Activities.Description;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.BuiltInConfiguration
{

    class Program
    {
        //const string connectionString = "Data Source=.\\SQLEXPRESS;Initial Catalog=InstanceStore;Integrated Security=True;Asynchronous Processing=True";
        const string hostBaseAddress = "http://localhost:8081/CountingService";


        [ServiceContract]
        public interface ICountingWorkflow
        {
            [OperationContract(IsOneWay = true)]
            void start();
        }


        static void Main(string[] args)
        {
            // Create service host.
            WorkflowServiceHost host = new WorkflowServiceHost(new CountingWorkflow2(), new Uri(hostBaseAddress));

            // Add service endpoint.
            host.AddServiceEndpoint("ICountingWorkflow", new BasicHttpBinding(), "");

            // Open service host.
            host.Open();

            // Create a client that sends a message to create an instance of the workflow.
            ICountingWorkflow client = ChannelFactory<ICountingWorkflow>.CreateChannel(new BasicHttpBinding(), new EndpointAddress(hostBaseAddress));
            client.start();

            Console.WriteLine("(Press [Enter] at any time to terminate host)");
            Console.ReadLine();
            host.Close();
        }
        
    }
}

