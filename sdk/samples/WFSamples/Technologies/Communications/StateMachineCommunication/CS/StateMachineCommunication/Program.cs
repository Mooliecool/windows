//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Threading;
using System.Workflow.Activities;
using System.Workflow.Runtime; 

namespace Microsoft.Samples.Workflow.StateMachineCommunication
{
    class Program
    {
        private static EventService service;
        private static Guid instanceId;

        static void Main(string[] args)
        {
            using(WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                AutoResetEvent waitHandle = new AutoResetEvent(false);
                ExternalDataExchangeService dataExchange = new ExternalDataExchangeService();
                workflowRuntime.AddService(dataExchange);
                service = new EventService();
                dataExchange.AddService(service);
                
                workflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) 
                {
                    Console.WriteLine("Workflow completed.");
                    waitHandle.Set();
                };
                workflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    waitHandle.Set();
                };
                workflowRuntime.WorkflowIdled += delegate(object sender, WorkflowEventArgs e)
                {
                    Console.WriteLine("Workflow idled.");
                    Console.WriteLine("Sending event to workflow.");
                    service.RaiseSetStateEvent(instanceId);
                };

                WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(StateMachineCommunication.SimpleStateMachine));
                instanceId = instance.InstanceId;
                Console.WriteLine("Starting workflow.");
                instance.Start();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }
    }
}
