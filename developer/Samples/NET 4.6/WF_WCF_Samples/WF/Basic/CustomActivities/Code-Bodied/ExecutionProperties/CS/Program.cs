//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.XamlIntegration;

namespace Microsoft.Samples.ExecutionProperties
{

    class Program
    {
        static void Main()
        {
            Activity workflow = ActivityXamlServices.Load("ThreeColors.xaml");
            WorkflowInvoker.Invoke(workflow);

            Console.WriteLine("Press <enter> to exit");
            Console.ReadLine();
        }
    }
}
