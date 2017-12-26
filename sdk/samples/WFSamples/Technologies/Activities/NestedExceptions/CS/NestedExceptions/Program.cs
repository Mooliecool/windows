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
using System.Workflow.Runtime.Hosting;

namespace Microsoft.Samples.Workflow.NestedExceptions
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                try
                {
                    // A workflow is always run asychronously; the main thread waits on this event so the program
                    // doesn't exit before the workflow completes
                    workflowRuntime.AddService(new SqlWorkflowPersistenceService("Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;"));

                    // Listen for the workflow events
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                    workflowRuntime.WorkflowAborted += OnWorkflowAborted;

                    // Create an instance of the workflow
                    Type type = typeof(NestedExceptionsWorkflow);
                    workflowRuntime.CreateWorkflow(type).Start();
                    Console.WriteLine("Workflow Started.\n");

                    // Wait for the event to be signaled
                    waitHandle.WaitOne();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Source: {0}\nMessage: {1}", ex.Source, ex.Message);
                }
                finally
                {
                    workflowRuntime.StopRuntime();
                    Console.WriteLine("\nWorkflow Complete.");
                }
            }
        }

        // This method is called when the workflow terminates and does not complete
        // This should not occur in this sample; however, it is good practice to include a
        // handler for this event so the host application can manage workflows that are
        // unexpectedly terminated (e.g. unhandled workflow exception).
        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

        // This method will be called when a workflow instance is completed
        // waitHandle is set so the main thread can continue
        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            waitHandle.Set();
        }

        // This method will be called when a workflow is aborted, such as by a failed database connection.
        static void OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow aborted: Please check database connectivity");
            waitHandle.Set();
        }
     }
}
