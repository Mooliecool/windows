//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.ServiceModel.Discovery;
using System.Xml.Linq;

namespace Microsoft.Samples.Discovery
{
    class Service
    {
        public static void Main()
        {
            Uri baseAddress = new Uri("net.tcp://localhost:8000/" + Guid.NewGuid().ToString());

            ServiceHost serviceHost = new ServiceHost(typeof(CalculatorService), baseAddress);
            try
            {
                ServiceEndpoint netTcpEndpoint = serviceHost.AddServiceEndpoint(typeof(ICalculatorService), new NetTcpBinding(), string.Empty);
                EndpointDiscoveryBehavior endpointDiscoveryBehavior = new EndpointDiscoveryBehavior();

                // Create XML metadata to add to the service endpoint
                XElement endpointMetadata = new XElement(
                    "Root",
                    new XElement("Information", "This endpoint is for a service of type: ICalculatorService."),
                    new XElement("Time", System.DateTime.Now.ToString("MM/dd/yyyy HH:mm")));

                // Add the XML metadata to the endpoint discovery behavior.
                endpointDiscoveryBehavior.Extensions.Add(endpointMetadata);

                // Add the discovery behavior to the endpoint.
                netTcpEndpoint.Behaviors.Add(endpointDiscoveryBehavior);

                // Make the service discoverable over UDP multicast
                serviceHost.Description.Behaviors.Add(new ServiceDiscoveryBehavior());
                serviceHost.AddServiceEndpoint(new UdpDiscoveryEndpoint()); 

                serviceHost.Open();

                Console.WriteLine("Calculator Service started at {0}", baseAddress);
                Console.WriteLine();
                Console.WriteLine("Added the following metadata to the endpoint: \n{0}", endpointMetadata);
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
