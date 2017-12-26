//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Routing;

namespace Microsoft.Samples.RoutingDynamicReconfiguration
{


    public class Router
    {

        // Host the service within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the CalculatorService type.
            using (ServiceHost serviceHost =
                new ServiceHost(typeof(RoutingService)))
            {

                // Open the ServiceHost to create listeners         
                // and start listening for messages.

                serviceHost.Description.Behaviors.Add(new RoutingBehavior(new RoutingConfiguration()));
                serviceHost.Description.Behaviors.Add(new UpdateBehavior());

                Console.WriteLine("The Routing Service configured, opening....");
                serviceHost.Open();

                Console.WriteLine("The Routing Service is now running.");
                Console.WriteLine("Press <ENTER> to terminate router.");

                // The service can now be accessed.
                Console.ReadLine();
            }
        }

    }
}
