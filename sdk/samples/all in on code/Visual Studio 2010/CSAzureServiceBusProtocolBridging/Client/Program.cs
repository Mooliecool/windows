/****************************** Module Header ******************************\
* Project Name:   CSAzureServiceBusProtocolBridging
* Module Name:    Client
* File Name:      Program.cs
* Copyright (c) Microsoft Corporation
*
* This console application consumes the WCF service through Service Bus
* https://{namespace}.servicebus.windows.net/MyService.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using Common;

namespace Client
{
    class Program
    {
        static void Main(string[] args)
        {
            Random random = new Random();
            int number1 = random.Next(100);
            int number2 = random.Next(100);

            try
            {
                Console.WriteLine("Initializing proxy.");
                using (MyChannelFactory channelFactory = new MyChannelFactory("ServiceBusRouterService", Settings.ServiceEndpoint))
                {
                    Console.WriteLine("Calling Add({0}, {1}) via {2}", number1, number2, channelFactory.EndpointAddress);
                    int result = channelFactory.MyService.Add(number1, number2);

                    Console.WriteLine("Result: {0}", result);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            Console.WriteLine("Please press [Enter] to exit.");
            Console.ReadLine();
        }
    }
}
