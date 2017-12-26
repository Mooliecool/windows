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
using System.Workflow.Activities;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.Listen
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static OrderServiceImpl orderService;

        static void Main()
        {
            orderService = new OrderServiceImpl();

            // Start the workflow runtime engine.
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                ExternalDataExchangeService dataService = new ExternalDataExchangeService();
                workflowRuntime.AddService(dataService);
                dataService.AddService(orderService);

                workflowRuntime.StartRuntime();

                // Listen for the workflow events
                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                workflowRuntime.WorkflowIdled += OnWorkflowIdled;

                // Start the workflow and wait for it to complete
                Type type = typeof(PurchaseOrderWorkflow);
                workflowRuntime.CreateWorkflow(type).Start();

                waitHandle.WaitOne();

                // Stop the workflow runtime engine.
                workflowRuntime.StopRuntime();
            }
        }

        // This method will be called when a workflow instance is idled
        static void OnWorkflowIdled(object sender, WorkflowEventArgs e)
        {
            orderService.instanceId = e.WorkflowInstance;

            // Randomly approve, reject or timeout purchase orders
            Random randGen = new Random();

            int pick = randGen.Next(1, 100) % 3;
            switch (pick)
            {
                case 0:
                    orderService.ApproveOrder();
                    break;
                case 1:
                    orderService.RejectOrder();
                    break;
                case 2:
                    // timeout
                    break;
            }
        }

        // This method will be called when a workflow instance is completed
        // waitHandle is set so the main thread can continue
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
