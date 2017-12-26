/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    Service
* File Name:      Program.cs
* Copyright (c) Microsoft Corporation
*
* This console application publishes a WCF service to net.tcp://localhost:9090/MyService
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.ServiceModel;

namespace Service
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Starting service.");
            using (ServiceHost host = new ServiceHost(new MyService()))
            {
                host.Open();

                Console.WriteLine("Service is ready at {0}",
                    host.Description.Endpoints[0].Address.ToString());

                Console.WriteLine("Please press [Enter] to exit.");
                Console.ReadLine();
            }
        }
    }
}
