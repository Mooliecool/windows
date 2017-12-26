//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Messaging;
using System.ServiceModel;
using System.ServiceModel.Activities;

namespace Microsoft.Samples.TransactedQueue
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Setting up queue");

            try
            {
                // Setup the queue
                CreateMessageQueue();
            }
            catch (InvalidOperationException)
            {
                Console.WriteLine("MSMQ is not installed or is not configured properly to run the sample. See readme for more info.");

                return;
            }

            Console.WriteLine("\nStarting client to queue messages");

            // Run the client to enqueue some messages for the service to process later
            WorkflowInvoker.Invoke(Client.Create());

            Console.WriteLine("Client finished");

            // Create the service and host it to start processing messages
            WorkflowService service = new WorkflowService
            {
                Name = "RewardsPointsWorkflowService",
                Body = Service.Create()
            };

            using (WorkflowServiceHost host = new WorkflowServiceHost(service))
            {
                // Specify the Contract, Binding and Address to service
                host.AddServiceEndpoint(Shared.Contract, Shared.Binding, Shared.Address);

                Console.WriteLine("\nStarting service to process messages");

                // Start service and begin processing messages
                host.Open();

                Console.WriteLine("Hit enter to quit...");

                Console.ReadLine();
            }

            // Cleanup queue
            DeleteMessageQueue();
        }

        // Basic MSMQ helpers
        private static void CreateMessageQueue()
        {
            if (MessageQueue.Exists(Shared.QueuePath))
            {
                DeleteMessageQueue();
            }

            MessageQueue.Create(Shared.QueuePath, true);
        }

        private static void DeleteMessageQueue()
        {
            MessageQueue.Delete(Shared.QueuePath);
        }
    }
}
