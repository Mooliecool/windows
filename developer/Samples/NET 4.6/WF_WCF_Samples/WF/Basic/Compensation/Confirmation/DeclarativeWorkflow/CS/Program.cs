//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;

namespace Microsoft.Samples.Compensation.ConfirmationSample
{

    class Program
    {
        static void Main()
        {
            //Executing a sequence of two CompensableActivities. After the second CA completes the first is
            //explicitly confirmed using the Confirm activity and the CompensationToken result of the first CA
            Console.WriteLine("\nExplicit confirmation:");
            new WorkflowInvoker(new ConfirmACompensableActivity()).Invoke();

            //Executing a sequence of two CompensableActivities. After the second CA completes the first is
            //explicitly compensated using the Compensate activity and the CompensationToken result of the first CA
            Console.WriteLine("\nExplicit compensation:");
            new WorkflowInvoker(new CompensateACompensableActivity()).Invoke();

            //Executing a CompensableActivity whose body is a sequence of two CompensableActivities. When the 
            //workflow completes it confirms the outer CA which has a confirmation handler defined. The confirmation
            //handler then compensates the first CA and confirms the second. Note that by default the second CA would
            //have been confirmed then the first.
            Console.WriteLine("\nCustom confirmation handler:");
            new WorkflowInvoker(new SpecifyCustomOrderInCompensationHandler()).Invoke();

            //Executing a sequence of two CompensableActivities. In the body of the first CA the two ints are summed and stored
            //in a variable which was added to the sequence's variables collection. The second CA then adds another int to the sum.
            //When the workflow completes default confirmation is invoked confirming CA2 then CA1. The first CA has a custom
            //confirmation handler which then subtracts an int from the sum. Notice that the value of the sum when the CA1 confirmation
            //handler runs is the value after both activities have modified the sum.
            Console.WriteLine("\nVariable access in a confirmation handler:");
            new WorkflowInvoker(new VariableAccessVignette()).Invoke();

            Console.WriteLine("\nPress ENTER to exit");
            Console.ReadLine();
        }
    }
}
