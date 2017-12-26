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
using System.Security.Principal;

namespace Microsoft.Samples.Workflow.ActiveDirectoryRoles
{
    class Program
    {
        private static StartPurchaseOrder poImpl;
        private static Guid workflowInstanceId;
        private static AutoResetEvent waitHandle = new AutoResetEvent(false);

        private static void Main()
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                poImpl = new StartPurchaseOrder();
                ExternalDataExchangeService dataService = new ExternalDataExchangeService();
                workflowRuntime.AddService(dataService);
                dataService.AddService(poImpl);

                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                // Load the workflow type.
                Type type = typeof(PurchaseOrderWorkflow);
                WorkflowInstance instance = workflowRuntime.CreateWorkflow(type);
                workflowInstanceId = instance.InstanceId;

                // Start the workflow runtime engine
                workflowRuntime.StartRuntime();

                instance.Start();

                SendPORequestMessage();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }

        private static void SendPORequestMessage()
        {
            try
            {
                // Initialize the Purchase Order details
                int itemID = 191;
                float itemCost = 200.00F;
                string itemName = "Graphics Card";
              
                // Send the event to initiate the PO from the workflow participant
                // The current windows identity is used for simulating the Employee
                // who can initiate a PO

                WindowsIdentity authUserIdentity = WindowsIdentity.GetCurrent();

                poImpl.InvokePORequest(workflowInstanceId, itemID, itemCost, itemName, authUserIdentity);
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception message: {0}", e.ToString());
            }
        }

        private static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("Workflow has completed.");
            waitHandle.Set();
        }

        private static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine("Workflow has been terminated. Message: {0}", e.Exception.Message);
            waitHandle.Set();
        }
    }
}
