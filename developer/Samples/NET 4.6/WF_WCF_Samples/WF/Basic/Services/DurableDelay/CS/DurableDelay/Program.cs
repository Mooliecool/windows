//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------
using System;
using System.ServiceModel;
using System.ServiceModel.Activities.Description;

namespace Microsoft.Samples.DurableDelay
{

    class Program
    {
        static void Main()
        {            
            System.ServiceModel.Activities.WorkflowServiceHost myServiceHost = new System.ServiceModel.Activities.WorkflowServiceHost(new Sequence1(), new Uri("http://localhost:8080/Client"));

            //Create an endpoint in the service host to enable comunication with the Receive activity inside the Workflow.
            myServiceHost.AddServiceEndpoint("IWorkflow", new BasicHttpBinding(), "IWorkflow");            

            //Set up SQL Instance Store
            string myConnectionString = "Data Source=localhost\\SQLEXPRESS;Initial Catalog=DefaultSampleStore;Integrated Security=True;Asynchronous Processing=True";
            SqlWorkflowInstanceStoreBehavior sqlWorkflowInstanceStoreBehavior = new SqlWorkflowInstanceStoreBehavior(myConnectionString);
            myServiceHost.Description.Behaviors.Add(sqlWorkflowInstanceStoreBehavior);

            //Set the TimeToUnload to 0 to force the WF to be unloaded. To have a durable delay, the WF needs to be unloaded otherwise it will be thread as an in-memory delay.
            WorkflowIdleBehavior workflowIdleBehavior = new WorkflowIdleBehavior()
            {
                TimeToUnload = TimeSpan.FromSeconds(0)
            };
            myServiceHost.Description.Behaviors.Add(workflowIdleBehavior);

            myServiceHost.Open();
            Console.WriteLine("WorkflowServiceHost started");            

            //To create an instance of the Workflow, we are sending a message to the receive in the Workflow. 
            IWorkflow proxy = ChannelFactory<IWorkflow>.CreateChannel(new BasicHttpBinding(), new EndpointAddress("http://localhost:8080/Client/IWorkflow"));
            proxy.Start();
            Console.WriteLine("Client started");

            Console.ReadLine();
            myServiceHost.Close(); 
        }
    }
}
