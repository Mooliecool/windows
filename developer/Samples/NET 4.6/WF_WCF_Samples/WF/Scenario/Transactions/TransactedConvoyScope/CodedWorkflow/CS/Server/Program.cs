//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel.Activities;
using System.Threading;

namespace Microsoft.Samples.Transaction.TransactedConvoy.Server
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Starting the service.");
            ManualResetEvent serverResetEvent = new ManualResetEvent(false);
           
            using (WorkflowServiceHost host = new WorkflowServiceHost(new ServiceWorkflow()))
            {
                host.Open();

                Console.WriteLine("Service is running, press enter to close.");
                Console.ReadLine();
            };
        }
    }
}
