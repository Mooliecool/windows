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


namespace Microsoft.Samples.Workflow.SuspendAndTerminate
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static Boolean workflowSuspended = false;

        static void Main()
        {
            // Start the engine.
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                workflowRuntime.StartRuntime();

                // Load the workflow type.
                Type type = typeof(SuspendAndTerminateWorkflow);

                workflowRuntime.WorkflowCompleted += OnWorkflowCompletion;
                workflowRuntime.WorkflowSuspended += OnWorkflowSuspend;
                workflowRuntime.WorkflowResumed += OnWorkflowResume;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminate;

                WorkflowInstance workflowInstance = workflowRuntime.CreateWorkflow(type);
                workflowInstance.Start();

                waitHandle.WaitOne();

                if (workflowSuspended)
                {
                    Console.WriteLine("\r\nResuming Workflow Instance");
                    workflowInstance.Resume();
                    waitHandle.WaitOne();
                }

                workflowRuntime.StopRuntime();
            }

        }

        static void OnWorkflowCompletion(object sender, WorkflowCompletedEventArgs instance)
        {
            Console.WriteLine("\r\nWorkflow Instance Completed");
            waitHandle.Set();
        }

        static void OnWorkflowSuspend(object sender, WorkflowSuspendedEventArgs instance)
        {
            workflowSuspended = true;
            Console.WriteLine("\nWorkflow Suspend event raised");
            waitHandle.Set();
        }
        static void OnWorkflowResume(object sender, WorkflowEventArgs instance)
        {
            Console.WriteLine("\nWorkflow Resumed event raised");
        }

        static void OnWorkflowTerminate(object sender, WorkflowTerminatedEventArgs instance)
        {
            Console.WriteLine("\nWorkflow Terminated event raised");
            waitHandle.Set();
        }
    }
}
