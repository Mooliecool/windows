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
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Workflow.Runtime;

using System.Workflow.ComponentModel;


namespace Microsoft.Samples.Workflow.DynamicUpdateFromHost
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
                    // Start the engine
                    workflowRuntime.StartRuntime();

                    // Subscribe to events
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    workflowRuntime.WorkflowIdled += OnWorkflowIdled;
                    workflowRuntime.ServicesExceptionNotHandled += OnServicesExceptionNotHandled;

                    // Start PO approval workflow with purchase less than $1000
                    System.Int32 poAmount = 750;
                    Type workflowType = typeof(Microsoft.Samples.Workflow.DynamicUpdateFromHost.DynamicUpdateWorkflow);
                    Dictionary<string, object> inputParameters = new Dictionary<string, object>();
                    inputParameters.Add("Amount", poAmount);
                    workflowRuntime.CreateWorkflow(workflowType, inputParameters).Start();
                    waitHandle.WaitOne();
                }
                catch (Exception e)
                {
                    Console.WriteLine("Exception \n\t Source: {0} \n\t Message: {1}", e.Source, e.Message);
                }
                finally
                {
                    workflowRuntime.StopRuntime();
                    Console.WriteLine("Workflow runtime stopped, program exiting... \n");
                }
            }
        }

        static void OnServicesExceptionNotHandled(object sender, ServicesExceptionNotHandledEventArgs e)
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

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("  Workflow instance " + e.WorkflowInstance.InstanceId + " completed\n");
            waitHandle.Set();
        }

        static void OnWorkflowIdled(object sender, WorkflowEventArgs e)
        {
            WorkflowInstance workflowInstance = e.WorkflowInstance;
            Activity wRoot = workflowInstance.GetWorkflowDefinition();

            //
            // use WorkflowChanges class to author dynamic change
            //
            WorkflowChanges changes = new WorkflowChanges(wRoot);
            Console.WriteLine("  Host is denying all PO requests - Removing POCreated step");
            //
            // remove POCreated activity
            //
            changes.TransientWorkflow.Activities.Remove(changes.TransientWorkflow.Activities["POCreated"]);
            //
            // apply transient changes to instance
            //
            workflowInstance.ApplyWorkflowChanges(changes);
        }
    }
}
