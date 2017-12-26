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
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Drawing;

namespace Microsoft.Samples.Workflow.WorkflowThreading
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static AutoResetEvent readyHandle = new AutoResetEvent(false);
        static WorkflowInstance workflowInstance;
        static WorkflowRuntime workflowRuntime;

        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage WorkflowThreading.exe [Single | Multi] [Delay | WaitForMessage]");
                return;
            }

            if (!args[0].Equals("Single", StringComparison.OrdinalIgnoreCase) && !args[0].Equals("Multi", StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine("Specify Single or Multi as a first command line parameter");
                return;
            }

            if (!args[1].Equals("Delay", StringComparison.OrdinalIgnoreCase) && !args[1].Equals("WaitForMessage", StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine("Specify Delay or WaitForMessage as a second command line parameter");
                return;
            }

            ThreadMonitor.Enlist(Thread.CurrentThread, "Host");
            Console.ForegroundColor = ConsoleColor.White;

            // Start the engine
            using (workflowRuntime = new WorkflowRuntime())
            {
                ManualWorkflowSchedulerService scheduler = null;
                if (args[0].ToString().Equals("Single", StringComparison.OrdinalIgnoreCase))
                {
                    scheduler = new ManualWorkflowSchedulerService();
                    workflowRuntime.AddService(scheduler);
                }

                workflowRuntime.StartRuntime();

                // Set up the workflow runtime event handlers
                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                workflowRuntime.WorkflowIdled += OnWorkflowIdled;
                workflowRuntime.WorkflowCreated += OnWorkflowCreated;

                // Load the workflow type
                Type type = typeof(ThreadingWorkflow);
                Dictionary<string, object> workflowParameters = new Dictionary<string, object>();
                workflowParameters.Add("BranchFlag", args[1]);

                Console.WriteLine("\n--- Before Starting Workflow ---\n");

                // Create an instance of the workflow
                workflowInstance = workflowRuntime.CreateWorkflow(type, workflowParameters);
                workflowInstance.Start();

                Console.WriteLine("\n--- After Starting Workflow ---\n");

                if (scheduler != null)
                    scheduler.RunWorkflow(workflowInstance.InstanceId);
                readyHandle.WaitOne();

                if (args[1].Equals("WaitForMessage", StringComparison.OrdinalIgnoreCase))
                {
                    // Send message to WaitForMessageActivity's queue
                    workflowInstance.EnqueueItem("WaitForMessageActivityQueue", "Hello", null, null);
                }

                if (scheduler != null)
                    scheduler.RunWorkflow(workflowInstance.InstanceId);
                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }

        static void OnWorkflowCreated(object sender, WorkflowEventArgs e)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "Host", "Host: Processed WorkflowCreated Event");
        }

        static void OnWorkflowIdled(object sender, WorkflowEventArgs e)
        {
            if (workflowRuntime.GetService<ManualWorkflowSchedulerService>() != null)
            {
                // Set a system timer to reload this workflow when its next timer expires
                SetReloadWorkflowTimer();
            }
            else
            {
                readyHandle.Set();
            }

            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "Host", "Host: Processed WorkflowIdle Event");

            Console.WriteLine("\n--- Workflow Idle ---\n");
        }

        static void SetReloadWorkflowTimer()
        {
            DateTime reloadTime = workflowInstance.GetWorkflowNextTimerExpiration();
            if (reloadTime == DateTime.MaxValue)
            {
                // WaitForMessage case
                readyHandle.Set();
            }
            else
            {
                TimeSpan timeDifference = 
                    reloadTime - 
                    DateTime.UtcNow + 
                    // account for the race in this code
                    // the timer is set later than the timespan was measured
                    new TimeSpan(0, 0, 0, 0, 1);

                Timer timer = new System.Threading.Timer(
                    new TimerCallback(ReloadWorkflow),
                    null,
                    timeDifference < TimeSpan.Zero ? TimeSpan.Zero : timeDifference,
                    new TimeSpan(-1));
            }
        }

        static void ReloadWorkflow(object state)
        {
            if (workflowInstance.GetWorkflowNextTimerExpiration() > DateTime.UtcNow)
            {
                SetReloadWorkflowTimer();
            }
            else
            {
                readyHandle.Set();
            }
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "Host", "Host: Processed WorkflowCompleted Event");

            waitHandle.Set();

            Console.WriteLine("\n--- Workflow Done ---\n");
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

    }
}
