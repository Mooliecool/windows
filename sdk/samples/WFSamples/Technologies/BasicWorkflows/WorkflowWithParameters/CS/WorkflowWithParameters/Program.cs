//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.SequentialWorkflowWithParameters
{
    class Program
    {
        //A workflow is always run asychronously; the main thread waits on this event so the program
        // does not exit before the workflow completes.
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main(string[] args)
        {
            try
            {
                // The program passes the one and only argument into the workflow
                //  as the order amount.
                if (args.Length < 1)
                {
                    Console.WriteLine("Usage: SequentialWorkflowWithParameters [amount]");
                    return;
                }

                // Create the WorkflowRuntime
                using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
                {
                    Console.WriteLine("Runtime Started.");

                    // Listen for the workflow events
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    // Set up the parameters
                    // "amount" is an "in" parameter and specifies the order amount.
                    //  If the amount is < 500 the status is "approved"; "rejected" otherwise.
                    Dictionary<string, object> parameters = new Dictionary<string, object>();
                    parameters.Add("Amount", Convert.ToInt32(args[0], CultureInfo.InvariantCulture));

                    // Get the workflow type
                    Type type = typeof(SequentialWorkflow);

                    // Create and start an instance of the workflow
                    workflowRuntime.CreateWorkflow(type, parameters).Start();
                    Console.WriteLine("Workflow Started.");

                    // Wait for the event to be signaled
                    waitHandle.WaitOne();

                    workflowRuntime.StopRuntime();
                    Console.WriteLine("Program Complete.");
                }
            }
            catch (Exception exception)
            {
                Console.WriteLine("Exception occurred: " + exception.Message);
            }
        }

        // This method is called when a workflow instance is completed; because only a single
        // thread can continue instance is started, the event arguments are ignored and the waitHandle  
        // is signaled so the main and exit the program.
        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            //The order status is stored in the "status" "out" parameter.
            string orderStatus = e.OutputParameters["Status"].ToString();
            Console.WriteLine("Order was " + orderStatus);
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
