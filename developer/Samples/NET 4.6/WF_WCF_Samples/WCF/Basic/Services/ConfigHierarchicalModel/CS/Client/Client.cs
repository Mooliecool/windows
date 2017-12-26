
//-----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;

namespace Microsoft.Samples.ConfigHierarchicalModel
{

    class Client
    {
        public static void Main()
        {
            // Service 1
            ServiceReference1.DescClient service1 = new ServiceReference1.DescClient("BasicHttpBinding_IDesc1");
            Console.WriteLine("Service 1:");
            foreach (string endpoint in service1.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service1.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service1.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 2
            ServiceReference2.DescClient service2 = new ServiceReference2.DescClient("BasicHttpBinding_IDesc2");
            Console.WriteLine("Service 2:");
            foreach (string endpoint in service2.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service2.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service2.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 3
            ServiceReference3.DescClient service3 = new ServiceReference3.DescClient("BasicHttpBinding_IDesc3");
            Console.WriteLine("Service 3:");
            foreach (string endpoint in service3.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service3.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service3.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 4
            ServiceReference4.DescClient service4 = new ServiceReference4.DescClient("BasicHttpBinding_IDesc4");
            Console.WriteLine("Service 4:");
            foreach (string endpoint in service4.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service4.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service4.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 5
            ServiceReference5.DescClient service5 = new ServiceReference5.DescClient("BasicHttpBinding_IDesc5");
            Console.WriteLine("Service 5:");
            foreach (string endpoint in service5.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service5.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service5.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 6
            ServiceReference6.DescClient service6 = new ServiceReference6.DescClient("BasicHttpBinding_IDesc6");
            Console.WriteLine("Service 6:");
            foreach (string endpoint in service6.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service6.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service6.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 7
            ServiceReference7.DescClient service7 = new ServiceReference7.DescClient("BasicHttpBinding_IDesc7");
            Console.WriteLine("Service 7:");
            foreach (string endpoint in service7.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service7.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service7.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 10
            ServiceReference10.DescClient service10 = new ServiceReference10.DescClient("BasicHttpBinding_IDesc10");
            Console.WriteLine("Service 10:");
            foreach (string endpoint in service10.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service10.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service10.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();

            // Service 11
            ServiceReference11.DescClient service11 = new ServiceReference11.DescClient("BasicHttpBinding_IDesc11");
            Console.WriteLine("Service 11:");
            foreach (string endpoint in service11.ListEndpoints())
            {
                Console.WriteLine("  Endpoint: " + endpoint);
            }
            foreach (string serviceBehavior in service11.ListServiceBehaviors())
            {
                Console.WriteLine("  Service behavior: " + serviceBehavior);
            }
            foreach (string endpointBehavior in service11.ListEndpointBehaviors())
            {
                Console.WriteLine("  Endpoint behavior: " + endpointBehavior);
            }
            Console.WriteLine();
        }
    }
}
