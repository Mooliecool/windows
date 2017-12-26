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
using System.Threading;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.SimpleSequentialWorkflow
{
    class Program
    {
        //A workflow is always run asychronously; the main thread waits on this event so the program
        // doesn't exit before the workflow completes
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            // Create the WorkflowRuntime
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                try
                {
                    Console.WriteLine("Runtime Started.");

                    // Load the workflow type
                    Type type = typeof(SequentialWorkflow);

                    // Listen for the workflow events
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    // Create an instance of the workflow
                    workflowRuntime.CreateWorkflow(type).Start();
                    Console.WriteLine("Workflow Started.");

                    // Wait for the event to be signaled
                    waitHandle.WaitOne();

                    // Stop the runtime
                    workflowRuntime.StopRuntime();

                    Console.WriteLine("Program Complete.");
                }
                catch (Exception e)
                {
                    Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1} ", e.Source, e.Message);
                }
                finally
                {
                    if (workflowRuntime != null)
                        workflowRuntime.StopRuntime();
                    Console.WriteLine("Done running the workflow.");
                }
            }
        }

        // This method will be called when a workflow instance is completed; since we have started only a single
        // instance we are ignoring the event args and signaling the waitHandle so the main thread can continue
        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            waitHandle.Set();
        }

        // This method is called when the workflow terminates and does not complete
        // This should not occur in this sample; however, it is good practice to include a
        // handler for this event so the host application can manage workflows that are
        // unexpectedly terminated (e.g. unhandled workflow exception).
        // waitHandle is set so the main thread can continue
        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
