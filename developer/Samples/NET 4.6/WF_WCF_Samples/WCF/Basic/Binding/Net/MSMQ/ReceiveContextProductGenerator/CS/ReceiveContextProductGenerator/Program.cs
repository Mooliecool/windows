//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Messaging;
using System.ServiceModel;

namespace Microsoft.Samples.Queues
{
    class Program
    {
        private const string queueLocation = "net.msmq://localhost/private/";
        private const string queueName = "calculatorservice";
        private const string queuePrefix = ".\\Private$\\";

        static void Main(string[] args)
        {
            CreateMessageQueue(queuePrefix + queueName);

            NetMsmqBinding binding = new NetMsmqBinding(NetMsmqSecurityMode.None);
            Uri address = new Uri(queueLocation + queueName);

            // Specify the behavior of the service host - the ProductCalculator methods.
            ServiceHost serviceHost = new ServiceHost(typeof(ProductCalculator));
            // Specify the Contract, Binding and Address to service.
            serviceHost.AddServiceEndpoint(typeof(IProductCalculator), binding, address);

            try
            {
                // Begin processing messages.
                serviceHost.Open();

            		SendTwoRandomNumbers.SendNumbers(address, 10);

            		// Wait for the service hosts to process the messages.
            		Console.WriteLine("Press enter when messages have been received.");
            		Console.ReadLine();

                // Cease processing messages.
            		serviceHost.Close();

            		DeleteMessageQueue(queuePrefix + queueName);
            }
            catch (AddressAccessDeniedException)
            {
                Console.WriteLine("Ensure this process is running with elevated"
                        + " privileges.");
            }
            catch (TimeoutException)
            {
                Console.WriteLine("ServiceHost timed out during the open or close"
                        + " operation.");
            }
            catch (CommunicationException)
            {
                Console.WriteLine("ServiceHost was unable to open or close properly.");
            }
        }

        private static void CreateMessageQueue(string queuePath)
        {
            if (MessageQueue.Exists(queuePath))
            {
                DeleteMessageQueue(queuePath);
            }
            MessageQueue.Create(queuePath, true);
        }

        private static void DeleteMessageQueue(string queuePath)
        {
            MessageQueue.Delete(queuePath);
        }
    }
}
