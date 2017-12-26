//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.TestInteropInDesigner
{

    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(new Sequence1());

            // wait for user input
            Console.WriteLine("\nPress enter to exit");
            Console.ReadLine();
        }
    }
}
