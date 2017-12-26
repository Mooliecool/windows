/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    RouterService
* File Name:      Program.cs
* Copyright (c) Microsoft Corporation
*
* This console application exposes a Routing Service through Service Bus 
* https://{namespace}.servicebus.windows.net/MyService.
* The Routing Service routs WCF requests to net.tcp://localhost:9090/MyService
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.ServiceModel;
using System.ServiceModel.Routing;
using Microsoft.ServiceBus;
using System.ServiceModel.Description;
using Common;

namespace RouterService
{
    class Program
    {
        static void Main(string[] args)
        {
            // Create the service URI based on the service namespace.
            Uri address = ServiceBusEnvironment.CreateServiceUri("https",
                            Settings.ServiceNamespace, "");

            // Create the credential object for the endpoint.
            TransportClientEndpointBehavior sharedSecretServiceBusCredential = new TransportClientEndpointBehavior();
            sharedSecretServiceBusCredential.CredentialType = TransportClientCredentialType.SharedSecret;
            sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerName = Settings.IssuerName;
            sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerSecret = Settings.IssuerSecret;

            Console.WriteLine("Starting service.");
            try
            {
                using (ServiceHost host = new ServiceHost(typeof(RoutingService), address))
                {
                    // Add the Service Bus credentials to all endpoints specified in configuration.
                    foreach (ServiceEndpoint endpoint in host.Description.Endpoints)
                    {
                        endpoint.Behaviors.Add(sharedSecretServiceBusCredential);
                    }

                    host.Open();

                    Console.WriteLine("Service is ready at {0}",
                        host.Description.Endpoints[0].Address.ToString());

                    Console.WriteLine("Please press [Enter] to exit.");
                    Console.ReadLine();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine("Please press [Enter] to exit.");
                Console.ReadLine();
            }
        }
    }
}
