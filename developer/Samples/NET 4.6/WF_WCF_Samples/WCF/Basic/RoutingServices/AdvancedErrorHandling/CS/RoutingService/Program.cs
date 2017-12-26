//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Routing;

namespace Microsoft.Samples.RoutingAdvancedErrorHandling
{

    
    class RoutingServiceHost
    {

        private static string primaryServiceQueue = "net.msmq://localhost/private/primaryServiceQueue";
        private static string backupServiceQueue = "net.msmq://localhost/private/backupServiceQueue";
        private static string primaryLoggingQueue = "net.msmq://localhost/private/primaryLoggingQueue";
        private static string backupLoggingQueue = "net.msmq://localhost/private/backupLoggingQueue";
        private static string inboundQ = "net.msmq://localhost/private/inQ";
        private static ContractDescription cd = ContractDescription.GetContract(typeof(ISimplexDatagramRouter));

        private static string inQpath = Environment.MachineName + "\\private$\\inQ";
        private static string primaryServicePath = Environment.MachineName + "\\private$\\primaryServiceQueue";
        private static string backupServicePath = Environment.MachineName + "\\private$\\backupServiceQueue";
        private static string primaryLoggingPath = Environment.MachineName + "\\private$\\primaryLoggingQueue";
        private static string backupLoggingPath = Environment.MachineName + "\\private$\\backupLoggingQueue";

        //main method to run the four test cases
        static void Main(string[] args)
        {

            CreateAllQueues();

            using (ServiceHost host = new ServiceHost(typeof(RoutingService)))
            {
                //uncomment this call and rename the provided App.config to something
                //else like App.config.example to run a Router configured via code
                //ConfigureRouterViaCode(host);
                host.Open();

                System.Console.WriteLine("Routing Service Configured and Opened");
                System.Console.WriteLine("Press Enter to Run Case 1");
                System.Console.ReadLine();
                RunMulticastErrorHandlingCase1();
            }

            using (ServiceHost host = new ServiceHost(typeof(RoutingService)))
            {
                //uncomment this call and rename the provided App.config to something
                //else like App.config.example to run a Router configured via code
                //ConfigureRouterViaCode(host);
                host.Open();

                System.Console.WriteLine("Press Enter to Run Case 2");
                System.Console.ReadLine();
                RunMulticastErrorHandlingCase2();

            }

            using (ServiceHost host = new ServiceHost(typeof(RoutingService)))
            {
                //uncomment this call and rename the provided App.config to something
                //else like App.config.example to run a Router configured via code
                //ConfigureRouterViaCode(host);
                host.Open();

                System.Console.WriteLine("Press Enter to Run Case 3");
                System.Console.ReadLine();
                RunMulticastErrorHandlingCase3();

            }


            using (ServiceHost host = new ServiceHost(typeof(RoutingService)))
            {
                //uncomment this call and rename the provided App.config to something
                //else like App.config.example to run a Router configured via code
                //ConfigureRouterViaCode(host);
                host.Open();

                System.Console.WriteLine("Press Enter to Run Case 4");
                System.Console.ReadLine();
                RunMulticastErrorHandlingCase4();

            }

            System.Console.WriteLine("The System Dead Letter queue has {0} messages.", QueueUtils.GetMessageCount("FormatName:Direct=OS:.\\System$;DEADXACT"));
            System.Console.WriteLine("Press <Enter> to Quit");
            System.Console.ReadLine();

            EmptyQueues();
            QueueUtils.DeleteQueue(Environment.MachineName + "\\private$\\backupServiceQueue");
            QueueUtils.DeleteQueue(Environment.MachineName + "\\private$\\inQ");
        }


        private static void ConfigureRouterViaCode(ServiceHost host)
        {
                //configure a MSMQ Binding that is not very tolerant of failures.
                MsmqTransportBindingElement msmqbe = new MsmqTransportBindingElement();

                //Set the auth mode and protection levels to none so that we can run without
                //Active Directory using Private queues.
                msmqbe.MsmqTransportSecurity.MsmqAuthenticationMode = MsmqAuthenticationMode.None;
                msmqbe.MsmqTransportSecurity.MsmqProtectionLevel = System.Net.Security.ProtectionLevel.None;

                //when the Routing Service receives a message and is unable to process it, we should reject it.
                //This will result in the message being placed in the "inbound" queue's System Dead Letter Queue.
                msmqbe.ReceiveErrorHandling = ReceiveErrorHandling.Reject;
                msmqbe.DeadLetterQueue = DeadLetterQueue.System;
                
                //Set the retry count, retry cycles, and delay to very unreasonable values
                //If the message cannot be delivered to the destination Queue on the first try
                //report an error. This error will be noticed by the Routing Service's error
                //handling code, which will automatically move the message to the backup endpoints
                //defined, if available.
                msmqbe.ReceiveRetryCount = 0;
                msmqbe.MaxRetryCycles = 0;
                msmqbe.RetryCycleDelay = TimeSpan.FromMilliseconds(1);
                
                //create the MSMQ Binding with our custom settings
                Binding QueueBinding = new CustomBinding(new BinaryMessageEncodingBindingElement(), msmqbe);

                //add a service endpoint to the host.  This is the endpoint on the Routing Service
                //that reads from the queue.
                host.AddServiceEndpoint(typeof(ISimplexDatagramRouter), QueueBinding, inboundQ);

                //create a new RoutingConfiguration object.
                RoutingConfiguration rc = new RoutingConfiguration();

                //create the service endpoints that the Routing Service will communicate with
                ServiceEndpoint primaryService = new ServiceEndpoint(cd, QueueBinding, new EndpointAddress(primaryServiceQueue));
                ServiceEndpoint backupService = new ServiceEndpoint(cd, QueueBinding, new EndpointAddress(backupServiceQueue));
                ServiceEndpoint primaryLogging = new ServiceEndpoint(cd, QueueBinding, new EndpointAddress(primaryLoggingQueue));
                ServiceEndpoint backupLogging = new ServiceEndpoint(cd, QueueBinding, new EndpointAddress(backupLoggingQueue));

                //we always want the message to go to both a service endpoint and a logging endpoint 
                //the way we achieve this is "multicasting" the message to both destinations at the same time
                //additionally, since we're both the inbound and outbound queues are transactional, if either none of the service queues 
                //or none of the logging queues are able to Transactionally recieve the message, then the Routing Service 
                //will report to the originating system that the message was unable to be delivered, at which point the 
                //originating queue will put the message in it's system Dead Letter Queue.

                //add the primary service queue and the backup service queue endpoint
                rc.FilterTable.Add(new MatchAllMessageFilter(), new List<ServiceEndpoint> { primaryService, backupService });

                //add the primary logging queue and the backup logging queue endpoint
                rc.FilterTable.Add(new MatchAllMessageFilter(), new List<ServiceEndpoint> { primaryLogging, backupLogging });

                //add the RoutingBehavior to the Service
                host.Description.Behaviors.Add(new RoutingBehavior(rc));
        }


        private static void RunMulticastErrorHandlingCase1()
        {
            
            EmptyQueues();
            QueueUtils.InsertMessageIntoQueue("net.msmq://localhost/private/inQ");
            System.Console.Write("In this case, we should see one message show up in the primary service queue and one message show up in the primary logging queue.");
            System.Console.Write(" This is because the Routing Service is receiving one message from the inbound queue and multicasting it to the two endpoint");
            System.Console.Write(" lists we configured before.  Since the primary endpoints are responding, both of them accept the message and the Routing Service ");
            System.Console.Write("is able to complete the transaction, successfully moving the message from the inbound queue to the primary service queue and the ");
            System.Console.WriteLine("primary logging queue.");
            System.Console.WriteLine("Press <Enter> to begin routing messages");
            System.Console.ReadLine();
            PrintQueueStatus();
            System.Console.WriteLine("Press <Enter> to continue");
            System.Console.ReadLine();

        }

        private static void RunMulticastErrorHandlingCase2()
        {
            EmptyQueues();
            System.Console.Write("In this example, we are going to delete the primary service queue, simulating a situation in which the system is unavailable ");
            System.Console.Write("for some reason.  In this case, we should see one message show up in the backup service queue, and one message show up in the ");
            System.Console.Write("primary logging queue. This is beacuse the Routing Service is failing to contact the primary service queue (it doesn't exist), ");
            System.Console.Write("and so automatically fails that transaction and creates another to wrap the sends that occur to the backup service queue and the ");
            System.Console.Write("primary logging queue. Since the backup service queue and the primary logging queue both respond, the Routing Service is able ");
            System.Console.Write("to complete the transaction, successfully moving the message from the inbound queue to the backup service queue and the primary ");
            System.Console.WriteLine("logging queue");
            System.Console.WriteLine("Deleting the primary service queue");
            QueueUtils.DeleteQueue(primaryServicePath);
            QueueUtils.InsertMessageIntoQueue("net.msmq://localhost/private/inQ");
            System.Console.WriteLine("Press <Enter> to begin routing messages");
            System.Console.ReadLine();
            PrintQueueStatus();
            System.Console.WriteLine("Press <Enter> to continue");
            System.Console.ReadLine();


        }

        private static void RunMulticastErrorHandlingCase3()
        {
            EmptyQueues();
            System.Console.Write("In this example, we also delete the primary logging queue, forcing the Routing Service to send the message to both the backup ");
            System.Console.Write("service queue and the backup logging queue.  Since both primary queues are unavailable, but the backup queues are present, we should ");
            System.Console.Write("see that the Routing Service is able to complete the transaction, successfully moving the message from the inbound queue to both the ");
            System.Console.WriteLine("backup service queue and the backup logging queue.");
            System.Console.WriteLine("Deleting the primary logging queue");
            QueueUtils.DeleteQueue(primaryLoggingPath);
            QueueUtils.InsertMessageIntoQueue("net.msmq://localhost/private/inQ");
            System.Console.WriteLine("Press <Enter> to begin routing messages");
            System.Console.ReadLine();
            PrintQueueStatus();
            System.Console.WriteLine("Press <Enter> to continue");
            System.Console.ReadLine();

        }

        private static void RunMulticastErrorHandlingCase4()
        {
            EmptyQueues();
            System.Console.Write("In the final example, we also delete the backup logging queue. Now the Routing Service will be unable to transactionally deliver the ");
            System.Console.Write("message to both a service queue and a backup queue, so it shouldn't move the message at all.  At the end of this, we should see that ");
            System.Console.Write("because the Routing Service was unable to transactionally deliver the message to both a service queue and a logging queue, MSMQ marks ");
            System.Console.WriteLine("the message undeliverable. Based on our MSMQ settings, this should place it in the system Transactional DLQ. ");
            System.Console.WriteLine("Deleting the backup logging queue");
            QueueUtils.DeleteQueue(backupLoggingPath);
            QueueUtils.InsertMessageIntoQueue("net.msmq://localhost/private/inQ");
            System.Console.WriteLine("Press <Enter> to begin routing messages");
            System.Console.ReadLine();
            PrintQueueStatus();
        }

        //use QueueUtil to create the queues that we'll use
        public static void CreateAllQueues()
        {
            QueueUtils.CreateQueue(inQpath, true);
            QueueUtils.CreateQueue(primaryServicePath, true);
            QueueUtils.CreateQueue(backupServicePath, true);
            QueueUtils.CreateQueue(primaryLoggingPath, true);
            QueueUtils.CreateQueue(backupLoggingPath, true);
        }

        //after each exercise, clear out all the queues for the next example
        private static void EmptyQueues()
        {

            QueueUtils.PurgeQueue(inQpath);
            QueueUtils.PurgeQueue(primaryServicePath);
            QueueUtils.PurgeQueue(backupServicePath);
            QueueUtils.PurgeQueue(primaryLoggingPath);
            QueueUtils.PurgeQueue(backupLoggingPath);
            QueueUtils.PurgeQueue("FormatName:Direct=OS:.\\System$;DEADXACT");

        }

        //go to each queue and print out the number of messages it contains
        private static void PrintQueueStatus()
        {
            System.Threading.Thread.Sleep(5000);
            System.Console.WriteLine("The inbound queue has {0} messages.", QueueUtils.GetMessageCount(inQpath));

            if(QueueUtils.QueueExists(primaryServicePath))
            {
                Console.WriteLine("The primary service queue has {0} messages.", QueueUtils.GetMessageCount(primaryServicePath));
            }
            else
            {
                Console.WriteLine("The primary service queue does not exist.");
            }
            
            if(QueueUtils.QueueExists(backupServicePath))
            {
                Console.WriteLine("The backup service queue has {0} messages.", QueueUtils.GetMessageCount(backupServicePath));
            }
            else
            {
                Console.WriteLine("The backup service queue does not exist.");
            }

            if(QueueUtils.QueueExists(primaryLoggingPath))
            {
                Console.WriteLine("The primary logging queue has {0} messages.", QueueUtils.GetMessageCount(primaryLoggingPath));
            }
            else
            {
               Console.WriteLine("The primary logging queue does not exist.");
            }

            if (QueueUtils.QueueExists(backupLoggingPath))
            {
                Console.WriteLine("The backup logging queue has {0} messages.", QueueUtils.GetMessageCount(backupLoggingPath));
            }
            else
            {
                Console.WriteLine("The backup logging queue does not exist.");
            }
        }
    }
}
