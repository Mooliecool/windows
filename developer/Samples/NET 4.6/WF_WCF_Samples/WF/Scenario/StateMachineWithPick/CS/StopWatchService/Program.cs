//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Description;

namespace Microsoft.Samples.StopWatchService
{

    class Program
    {
        static void Main(string[] args)
        {
            string baseAddress = "http://localhost:8081/StopWatchService";

            try
            {

                using (WorkflowServiceHost host = new WorkflowServiceHost(new StopWatchWorkflow(), new Uri(baseAddress)))
                {
                    host.Description.Behaviors.Add(new ServiceMetadataBehavior() { HttpGetEnabled = true });

                    Console.WriteLine("Opening StopWatchService...");
                    host.Open();

                    Console.WriteLine("StopWatchService waiting at: " + baseAddress);
                    Console.WriteLine("Press [ENTER] to exit");
                    Console.ReadLine();
                    host.Close();
                }
            }
            catch (AddressAlreadyInUseException)
            {
                Console.WriteLine("Error - An error occurred while opening the service. Please ensure that there are no other instances of this service running.");
                Console.WriteLine("Press [ENTER] to exit");
                Console.ReadLine();
            }
            catch (CommunicationObjectFaultedException)
            {
                Console.WriteLine("Error - An error occurred while opening the service. Please ensure that there are no other instances of this service running.");
                Console.WriteLine("Press [ENTER] to exit");
                Console.ReadLine();
            }
        }
    }

}
