//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Activities;
using System.ServiceModel;

namespace Microsoft.Samples.StopWatchClient
{

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                WorkflowInvoker.Invoke(new StopWatchClientWorkflow());
            }
            catch (EndpointNotFoundException)
            {
                Console.WriteLine("\r\nError - The StopWatch service is not started or is not responding. Please start the StopWatchService and run the client again.");                
            }

            Console.WriteLine("");
            Console.WriteLine("Press enter to exit...");
            Console.ReadLine();
        }
    }
}
