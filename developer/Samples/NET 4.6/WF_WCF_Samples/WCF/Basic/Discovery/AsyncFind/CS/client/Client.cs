//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Discovery;

namespace Microsoft.Samples.Discovery
{
    class Client
    {
        public static void Main()
        {
            FindCalculatorServiceAddress();

            Console.WriteLine("Press <ENTER> to exit.\n");
            Console.ReadLine();
        }

        static void FindCalculatorServiceAddress()
        {
            DiscoveryClient discoveryClient = new DiscoveryClient(new UdpDiscoveryEndpoint());

            discoveryClient.FindCompleted += new EventHandler<FindCompletedEventArgs>(ClientFindCompleted);
            discoveryClient.FindProgressChanged += new EventHandler<FindProgressChangedEventArgs>(ClientFindProgressChanged);

            Console.WriteLine("Finding ICalculatorServices endpoints asynchronously...\n");
            FindCriteria findCriteria = new FindCriteria(typeof(ICalculatorService));
            findCriteria.Duration = TimeSpan.FromSeconds(5);

            // Find ICalculatorService endpoint asynchronously. The results are returned asynchronously via events            
            discoveryClient.FindAsync(findCriteria);
        }

        static void ClientFindProgressChanged(object sender, FindProgressChangedEventArgs e)
        {
            Console.WriteLine("Found ICalculatorService endpoint at {0}\n", e.EndpointDiscoveryMetadata.Address.Uri);
            InvokeCalculatorService(e.EndpointDiscoveryMetadata.Address);
        }

        static void ClientFindCompleted(object sender, FindCompletedEventArgs e)
        {
            // Implement this method to access the FindResponse object through e.Result, which includes all the endpoints found
        }

        static void InvokeCalculatorService(EndpointAddress endpointAddress)
        {
            // Create a client
            CalculatorServiceClient client = new CalculatorServiceClient();

            // Connect to the discovered service endpoint
            client.Endpoint.Address = endpointAddress;

            Console.WriteLine("Invoking CalculatorService at {0}\n", endpointAddress);

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
            Console.WriteLine("Divide({0},{1}) = {2}\n", value1, value2, result);

            //Closing the client gracefully closes the connection and cleans up resources
            client.Close();
        }
    }
}

