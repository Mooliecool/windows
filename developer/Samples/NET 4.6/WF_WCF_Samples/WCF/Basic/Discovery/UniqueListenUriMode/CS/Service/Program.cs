//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.ServiceModel.Discovery;

namespace Microsoft.Samples.Discovery
{
    class Program
    {
        public static void Main()
        {
            Uri baseAddress = new Uri("net.tcp://localhost/" + Guid.NewGuid().ToString() + "/");

            ServiceHost serviceHost = new ServiceHost(typeof(CalculatorService), baseAddress);

            try
            {
                ServiceEndpoint netTcpEndpoint = serviceHost.AddServiceEndpoint(typeof(ICalculatorService), new NetTcpBinding(), string.Empty);

                // Set the ListenUri mode to unique
                netTcpEndpoint.ListenUriMode = ListenUriMode.Unique;

                // Make the service discoverable over UDP multicast
                serviceHost.Description.Behaviors.Add(new ServiceDiscoveryBehavior());
                serviceHost.AddServiceEndpoint(new UdpDiscoveryEndpoint());

                serviceHost.Open();

                Console.WriteLine("Calculator Service started at {0}", baseAddress);
                Console.WriteLine();
                Console.WriteLine("Press <ENTER> to terminate the service.");
                Console.WriteLine();
                Console.ReadLine();

                serviceHost.Close();
            }
            catch (CommunicationException e)
            {
                Console.WriteLine(e.Message);
            }
            catch (TimeoutException e)
            {
                Console.WriteLine(e.Message);
            }

            if (serviceHost.State != CommunicationState.Closed)
            {
                Console.WriteLine("Aborting service...");
                serviceHost.Abort();
            }
        }
    }
}
