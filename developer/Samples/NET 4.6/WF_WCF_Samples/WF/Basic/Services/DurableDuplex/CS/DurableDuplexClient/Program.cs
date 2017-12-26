//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System.Activities;

namespace Microsoft.Samples.DurableDuplex
{

    class Program
    {
        static void Main(string[] args)
        {
            WorkflowInvoker.Invoke(new DurableDuplexClient());
        }
    }
}
