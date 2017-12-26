//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Threading;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.IfElseActivityWithRules
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            // Start the engine.
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                workflowRuntime.StartRuntime();

                // Load the workflow type.
                Type type = typeof(IfElseWorkflow);

                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                // The "OrderValueParameter" parameter is used to determine which branch of the IfElse should be executed
                // a value less than 10,000 will execute branch 1 - Get Manager Approval; any other value will execute branch 2 - Get VP Approval
                Dictionary<string, object> parameters = new Dictionary<string, object>();
                parameters.Add("OrderValue", 14000);
                WorkflowInstance workflow = workflowRuntime.CreateWorkflow(type, parameters);
                workflow.Start();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("\r\nWorkflow completed");
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
