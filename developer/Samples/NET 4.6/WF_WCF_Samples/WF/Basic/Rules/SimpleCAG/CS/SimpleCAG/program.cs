//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Threading;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.SimpleConditionedActivityGroup
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            // Fire up the engine.
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                workflowRuntime.StartRuntime();

                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                // Load the workflow type.
                Type type = typeof(SimpleConditionedActivityGroupWorkflow);
                workflowRuntime.CreateWorkflow(type).Start();

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

