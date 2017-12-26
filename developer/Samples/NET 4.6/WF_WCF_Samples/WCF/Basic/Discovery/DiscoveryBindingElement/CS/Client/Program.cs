//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Discovery;

namespace Microsoft.Samples.Discovery
{
    class Program
    {
        public static void Main()
        {
            try
            {
                // Instantiate the client with a CustomBinding which has DiscoveryClientBindingElement
                CalculatorServiceClient client = new CalculatorServiceClient(
                        CreateCustomBindingWithDiscoveryElement(), 
                        DiscoveryClientBindingElement.DiscoveryEndpointAddress);

                Console.WriteLine("Discovering and invoking CalculatorService.");

                double value1 = 1023;
                double value2 = 1534;
                double value3 = 2342;

                // Call the Add service operation.
                double result = client.Add(value1, value2);
                Console.WriteLine("Adding({0}, {1}) = {2}", value1, value2, result);

                // Call the Subtract service operation.
                result = client.Subtract(value3, value2);
                Console.WriteLine("Subtracting ({0}, {1}) = {2}", value3, value2, result);

                // Closing the client gracefully closes the connection and cleans up resources
                client.Close();
            }
            catch (EndpointNotFoundException)
            {
                Console.WriteLine("Unable to connect to the calculator service because a valid endpoint was not found.");
            }

            Console.WriteLine("Press <ENTER> to exit.");
            Console.ReadLine();
        }

        static CustomBinding CreateCustomBindingWithDiscoveryElement()
        {
            DiscoveryClientBindingElement discoveryBindingElement = new DiscoveryClientBindingElement();
            
            // Provide the search criteria and the endpoint over which the probe will be sent
            discoveryBindingElement.FindCriteria = new FindCriteria(typeof(ICalculatorService));
            discoveryBindingElement.DiscoveryEndpointProvider = new UdpDiscoveryEndpointProvider();

            CustomBinding customBinding = new CustomBinding(new NetTcpBinding());
            // Insert DiscoveryClientBindingElement at the top of the BindingElement stack.
            // An exception will be thrown if this binding element is not at the top
            customBinding.Elements.Insert(0, discoveryBindingElement);

            return customBinding;
        }
    }

    // Extend DiscoveryEndpointProvider class to change the default DiscoveryEndpoint
    // to the DiscoveryClientBindingElement. The Discovery ClientChannel will
    // use this endpoint to send the Probe message.
    public class UdpDiscoveryEndpointProvider : DiscoveryEndpointProvider
    {
        public override DiscoveryEndpoint GetDiscoveryEndpoint()
        {
            return new UdpDiscoveryEndpoint(DiscoveryVersion.WSDiscoveryApril2005);
        }
    }
}
