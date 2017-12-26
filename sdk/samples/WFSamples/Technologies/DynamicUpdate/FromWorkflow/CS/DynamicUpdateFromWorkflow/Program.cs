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
using System.Configuration;
using System.Collections.Generic;
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;

namespace Microsoft.Samples.Workflow.DynamicUpdateFromWorkflow
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        const String amountParameter = "Amount";

        static void Main()
        {
            // Instanciate and configure workflow runtime
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                workflowRuntime.AddService(
                    new SqlWorkflowPersistenceService(
                        ConfigurationManager.AppSettings["ConnectionString"], true, new TimeSpan(0, 10, 0), new TimeSpan(0, 0, 5)));

                // Subscribe to workflow events
                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowIdled += OnWorkflowIdle;
                workflowRuntime.ServicesExceptionNotHandled += OnExceptionNotHandled;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                workflowRuntime.WorkflowAborted += OnWorkflowAborted;

                // Start Workflow Runtime
                workflowRuntime.StartRuntime();

                //
                // start PO approval workflow with purchase order amount less than $1000
                //
                Console.WriteLine("Workflow 1:");

                Int32 poAmount = 750;
                Type workflowType = typeof(DynamicUpdateWorkflow);
                Dictionary<string, object> workflow1Parameters = new Dictionary<string, object>();
                workflow1Parameters.Add(amountParameter, poAmount);
                workflowRuntime.CreateWorkflow(workflowType, workflow1Parameters).Start();
                waitHandle.WaitOne();

                //
                // start PO approval workflow with purchase order amount greater than $1000
                //
                Console.WriteLine("Workflow 2:");

                poAmount = 1200;
                Dictionary<string, object> workflow2Parameters = new Dictionary<string, object>();
                workflow2Parameters.Add(amountParameter, poAmount);
                workflowRuntime.CreateWorkflow(workflowType, workflow2Parameters).Start();
                waitHandle.WaitOne();

                //Wait for dynamically created workflow to finish
                waitHandle.WaitOne();

                // After workflows have completed, stop runtime and report to command line
                workflowRuntime.StopRuntime();
                Console.WriteLine("Workflow runtime stopped, program exiting... \n");
            }
        }

        static void OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow aborted: Please check database connectivity");
            waitHandle.Set();
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("  Workflow instance " + e.WorkflowInstance.InstanceId + " completed\n");
            waitHandle.Set();
        }

        static void OnWorkflowIdle(object sender, EventArgs e)
        {
            Console.WriteLine("  Workflow is in idle state");
        }

        static void OnExceptionNotHandled(object sender, ServicesExceptionNotHandledEventArgs e)
        {
            Console.WriteLine("Unhandled Workflow Exception ");
            Console.WriteLine("  Type: " + e.GetType().ToString());
            Console.WriteLine("  Message: " + e.Exception.Message);
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
