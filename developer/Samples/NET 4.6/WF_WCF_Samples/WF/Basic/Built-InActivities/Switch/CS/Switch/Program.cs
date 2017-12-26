//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.SwitchUsage
{
    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(new Sequence());

            Console.WriteLine("The workflow has completed");
            Console.WriteLine("Press <return> to exit...");
            Console.ReadLine();
        }
    }
}
