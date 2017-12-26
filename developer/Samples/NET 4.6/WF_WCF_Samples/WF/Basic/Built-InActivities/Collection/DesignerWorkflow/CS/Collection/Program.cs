//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Collection
{
    public class Program
    {
        public static void Main(string[] args)
        {
            Activity wf = new Sequence1();
            WorkflowInvoker.Invoke(wf);

            Console.WriteLine("Press <return> to continue...");
            Console.Read();
        }
    }
}
