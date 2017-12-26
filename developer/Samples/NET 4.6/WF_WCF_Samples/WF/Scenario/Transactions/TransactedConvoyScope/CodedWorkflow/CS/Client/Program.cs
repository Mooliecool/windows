//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Transaction.TransactedConvoy.Client
{
    class Program
    {
        static void Main()
        {
            Console.WriteLine("Press enter when the service is running.");
            Console.ReadLine();

            //Start the client
            Console.WriteLine("Starting the client.");
            new WorkflowInvoker(new ClientWorkflow()).Invoke();
            
            Console.WriteLine("Client complete. Press enter to exit.");
            Console.ReadLine();
        }
    }
}
