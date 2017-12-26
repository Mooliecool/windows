//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Activities
{

    class Program
    {
        static void Main()
        {
            WorkflowInvoker.Invoke(new Rhyme());
            
            Console.WriteLine("Press [Enter] to exit.");
            Console.ReadLine();
        }
    }
}
