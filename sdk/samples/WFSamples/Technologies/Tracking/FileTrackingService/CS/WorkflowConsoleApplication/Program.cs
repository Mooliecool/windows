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

namespace Microsoft.Samples.Workflow.FileTrackingServiceAndQuerySample
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            try
            {
                // Create WorkflowRuntime
                using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
                {
                    // Add SimpleFileTrackingService
                    workflowRuntime.AddService(new SimpleFileTrackingService());

                    // Subscribe to Workflow Completed, Suspended, and Terminated WorkflowRuntime Event
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowSuspended += OnWorkflowSuspended;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    // Start WorkflowRuntime
                    workflowRuntime.StartRuntime();

                    // Execute the SimpleWorkflow Workflow
                    Console.WriteLine("Executing the SimpleWorkflow...");
                    workflowRuntime.StartRuntime();
                    WorkflowInstance simpleWorkflowInstance = workflowRuntime.CreateWorkflow(typeof(SimpleWorkflow));
                    simpleWorkflowInstance.Start();
                    // Wait for the Workflow Completion
                    waitHandle.WaitOne();

                    // Execute the SuspendedWorkflow Workflow
                    Console.WriteLine("Executing the SuspendedWorkflow...");
                    workflowRuntime.StartRuntime();
                    WorkflowInstance suspendedWorkflowInstance = workflowRuntime.CreateWorkflow(typeof(SuspendedWorkflow));
                    suspendedWorkflowInstance.Start();
                    // Wait for the Workflow Suspension
                    waitHandle.WaitOne();

                    // Execute the ExceptionWorkflow Workflow
                    Console.WriteLine("Executing the ExceptionWorkflow...");
                    workflowRuntime.StartRuntime();
                    WorkflowInstance exceptionWorkflowInstance = workflowRuntime.CreateWorkflow(typeof(ExceptionWorkflow));
                    exceptionWorkflowInstance.Start();
                    // Wait for the Workflow Termination
                    waitHandle.WaitOne();

                    // Stop Runtime
                    workflowRuntime.StopRuntime();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("\nException:\n\tMessage: {0}\n\tSource: {1}", ex.Message, ex.Source);
            }
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("Workflow Completed");
            waitHandle.Set();
        }

        static void OnWorkflowSuspended(object sender, WorkflowSuspendedEventArgs e)
        {
            Console.WriteLine("Workflow Suspended");
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine("Workflow Terminated");
            waitHandle.Set();
        }
    }
}
