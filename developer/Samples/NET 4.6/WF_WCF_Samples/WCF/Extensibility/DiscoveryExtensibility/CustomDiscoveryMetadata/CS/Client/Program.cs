//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Discovery;
using System.Xml.Linq;

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

            // Find ICalculatorService endpoints            
            FindCriteria findCriteria = new FindCriteria(typeof(ICalculatorService));
            findCriteria.Duration = TimeSpan.FromSeconds(5);

            FindResponse findResponse = discoveryClient.Find(findCriteria);

            Console.WriteLine("Found {0} ICalculatorService endpoint(s).", findResponse.Endpoints.Count);
            Console.WriteLine();

            // Check to see if endpoints were found & print the XML metadata in them.
            if (findResponse.Endpoints.Count > 0)
            {
                foreach (XElement xElement in findResponse.Endpoints[0].Extensions)
                {
                    Console.WriteLine("Printing Metadata from ServiceEndpoint:");
                    Console.WriteLine("Endpoint Information: " + xElement.Element("Information").Value);
                    Console.WriteLine("Endpoint Started at Time: " + xElement.Element("Time").Value);
                    Console.WriteLine();
                }
                InvokeCalculatorService(findResponse.Endpoints[0].Address);
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
