//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Discovery;
using System.ServiceModel.Description;

namespace Microsoft.Samples.Discovery
{
    class Client
    {
        public static void Main()
        {
            // Create DiscoveryClient
            DiscoveryClient discoveryClient = new DiscoveryClient(new UdpDiscoveryEndpoint());

            Console.WriteLine("Finding ICalculatorService endpoints...");
            Console.WriteLine();

            FindCriteria findCriteria = new FindCriteria(typeof(ICalculatorService));
            findCriteria.Duration = TimeSpan.FromSeconds(5);

            // Find ICalculatorService endpoints            
            FindResponse findResponse = discoveryClient.Find(findCriteria);

            Console.WriteLine("Found {0} ICalculatorService endpoint(s).", findResponse.Endpoints.Count);
            Console.WriteLine();

            // Check to see if endpoints were found
            if (findResponse.Endpoints.Count > 0)
            {
                EndpointDiscoveryMetadata discoveredEndpoint = findResponse.Endpoints[0];

                // Check to see if the endpoint has a listenUri and if it differs from the Address URI
                if (discoveredEndpoint.ListenUris.Count > 0 && discoveredEndpoint.Address.Uri != discoveredEndpoint.ListenUris[0])
                {
                    // Since the service is using a unique ListenUri, it needs to be invoked at the correct ListenUri 
                    InvokeCalculatorService(discoveredEndpoint.Address, discoveredEndpoint.ListenUris[0]);
                }
                else
                {
                    // Endpoint was found, however it doesn't have a unique ListenUri, hence invoke the service with only the Address URI
                    InvokeCalculatorService(discoveredEndpoint.Address, null);
                }
            }

            Console.WriteLine("Press <ENTER> to exit.");
            Console.ReadLine();
        }

        static void InvokeCalculatorService(EndpointAddress endpointAddress, Uri viaUri)
        {
            // Create a client
            CalculatorServiceClient client = new CalculatorServiceClient(new NetTcpBinding(), endpointAddress);
            Console.WriteLine("Invoking CalculatorService at {0}", endpointAddress.Uri);

            // if viaUri is not null then add the approprate ClientViaBehavior.
            if (viaUri != null)
            {
                client.Endpoint.Behaviors.Add(new ClientViaBehavior(viaUri));
                Console.WriteLine("Using the viaUri {0}", viaUri);
            }

            Console.WriteLine();

            double value1 = 100.00D;
            double value2 = 15.99D;

            // Call the Add service operation.
            double result = client.Add(value1, value2);
            Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result);

            // Call the Subtract service operation.
            result = client.Subtract(value1, value2);
            Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result);

            // Call the Multiply service operation.
            result = client.Multiply(value1, value2);
            Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result);

            // Call the Divide service operation.
            result = client.Divide(value1, value2);
            Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result);
            Console.WriteLine();

            // Closing the client gracefully closes the connection and cleans up resources
            client.Close();
        }
    }
}
