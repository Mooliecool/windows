//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.InvokeMethodUsage
{

    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(new Sequence1());

            // Wait for confirmation to exit             
            Console.WriteLine("Press <return> to continue...");
            Console.ReadLine();
        }
    }
}
