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

namespace Microsoft.Samples.Workflow.PersistenceServices
{
    //This sample demonstrates how to use SqllWorkflowPersistenceService and
    // how to manage workflow lifetime using the WorkflowRuntime events.  Once the workflow instance is
    // idle due to the delay1 activity, the Host unloads the instance from memory.  When the timer expires
    // the Workflow is automatically loaded and it continues to execute.
    class Program
    {
        //A workflow is always run asychronously; the main thread waits on this event so the program
        // does not exit before the workflow completes.
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            try
            {
                // Create the WorkflowRuntime
                using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
                {
                    // Add the SqlWorkflowPersistenceService service
                    WorkflowPersistenceService persistenceService =
                        new SqlWorkflowPersistenceService(
                        "Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;",
                        false, 
                        new TimeSpan(1, 0, 0), 
                        new TimeSpan(0, 0, 5));
                    workflowRuntime.AddService(persistenceService);

                    // Set up the WorkflowRuntime event handlers
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowIdled += OnWorkflowIdled;
                    workflowRuntime.WorkflowPersisted += OnWorkflowPersisted;
                    workflowRuntime.WorkflowUnloaded += OnWorkflowUnloaded;
                    workflowRuntime.WorkflowLoaded += OnWorkflowLoaded;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                    workflowRuntime.WorkflowAborted += OnWorkflowAborted;


                    // Load the workflow type
                    Type type = typeof(PersistenceServicesWorkflow);

                    // Create an instance of the workflow
                    Console.WriteLine("Workflow Started.");
                    workflowRuntime.CreateWorkflow(type).Start();                    

                    // Wait for the event to be signaled
                    waitHandle.WaitOne();

                    // Stop the runtime
                    workflowRuntime.StopRuntime();
                    Console.WriteLine("Program Complete.");
                }
            }
            catch (Exception exception)
            {
                Console.WriteLine("Application exception occurred: " + exception.Message);
            }
        }

        //It is good practice to provide a handler for the WorkflowTerminated event
        // so the host application can manage unexpected problems during workflow execution
        // such as database connectivity issues, networking issues, and so on.
        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

        //Called when the workflow is loaded back into memory - in this sample this occurs when the timer expires
        static void OnWorkflowLoaded(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow was loaded.");
        }

        //Called when the workflow is unloaded from memory - in this sample the workflow instance is unloaded by the application
        // in the UnloadInstance method below.
        static void OnWorkflowUnloaded(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow was unloaded.");
        }

        //Called when the workflow is persisted - in this sample when it is unloaded and completed
        static void OnWorkflowPersisted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow was persisted.");
        }

        //Called when the workflow is idle - in this sample this occurs when the workflow is waiting on the
        // delay1 activity to expire.
        static void OnWorkflowIdled(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow is idle.");
            e.WorkflowInstance.TryUnload();
        }
        
        // This method is called when a workflow instance is completed; because only a single instance is 
        // started, the event arguments are ignored and the waitHandle is signaled so the main thread can continue.
        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs instance)
        {
            waitHandle.Set();
        }

        static void OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow aborted: Please check database connectivity");
            waitHandle.Set();
        }
    }
}
