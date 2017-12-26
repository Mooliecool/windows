//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Reflection;
using System.ServiceModel;
using System.ServiceModel.Discovery;

namespace Microsoft.Samples.Discovery
{
    class Client
    {
        public static void Main()
        {
            // Create a DiscoveryEndpoint that points to the DiscoveryProxy
            Uri probeEndpointAddress = new Uri("net.tcp://localhost:8001/Probe");
            DiscoveryEndpoint discoveryEndpoint = new DiscoveryEndpoint(new NetTcpBinding(), new EndpointAddress(probeEndpointAddress));
            
            DiscoveryClient discoveryClient = new DiscoveryClient(discoveryEndpoint);

            Console.WriteLine("Finding ICalculatorService endpoints using the proxy at {0}", probeEndpointAddress);
            Console.WriteLine();

            try
            {
                // Find ICalculatorService endpoints            
                FindResponse findResponse = discoveryClient.Find(new FindCriteria(typeof(ICalculatorService)));

                Console.WriteLine("Found {0} ICalculatorService endpoint(s).", findResponse.Endpoints.Count);
                Console.WriteLine();

                // Check to see if endpoints were found, if so then invoke the service.
                if (findResponse.Endpoints.Count > 0)
                {
                    InvokeCalculatorService(findResponse.Endpoints[0].Address);
                }
            }
            catch (TargetInvocationException)
            {
                Console.WriteLine("This client was unable to connect to and query the proxy. Ensure that the proxy is up and running.");
            }

            Console.WriteLine("Press <ENTER> to exit.");
            Console.ReadLine();
        }

        static void InvokeCalculatorService(EndpointAddress endpointAddress)
        {
            // Create a client
            CalculatorServiceClient client = new CalculatorServiceClient(new NetTcpBinding(), endpointAddress);
            Console.WriteLine("Invoking CalculatorService at {0}", endpointAddress.Uri);
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
