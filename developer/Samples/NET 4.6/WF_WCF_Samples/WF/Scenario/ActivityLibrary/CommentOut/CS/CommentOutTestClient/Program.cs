//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Activities.Statements
{

    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(new Sequence1());
            Console.WriteLine("Press <return> to continue...");
            Console.ReadLine();
        }
    }
}
