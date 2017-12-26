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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.Communication.HostCommunication
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        [STAThread]
        static void Main()
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {

                // Create our local service and add it to the workflow runtime's list of services
                ExternalDataExchangeService dataService = new ExternalDataExchangeService();
                workflowRuntime.AddService(dataService);
                VotingServiceImpl votingService = new VotingServiceImpl();
                dataService.AddService(votingService);

                // Start up the runtime and hook the creation and completion events
                workflowRuntime.StartRuntime();

                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                workflowRuntime.WorkflowStarted += OnWorkflowStarted;

                // Create the workflow's parameters collection
                Dictionary<string, object> parameters = new Dictionary<string, object>();
                parameters.Add("Alias", "Jim");
                // Create and start the workflow
                Type type = typeof(HostCommunication.VotingServiceWorkflow);
                workflowRuntime.CreateWorkflow(type, parameters).Start();

                waitHandle.WaitOne();

                // Cleanly stop the runtime and all services
                workflowRuntime.StopRuntime();
            }
        }

        // Called when a workflow is started
        static void OnWorkflowStarted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow started.\n");
        }

        // Called when a workflow is completed - sets the waitHandle event which signals the main thread
        // it can continue and exit the host application
        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("\nWorkflow completed.");
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
