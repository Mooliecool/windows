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

namespace Microsoft.Samples.Workflow.WebWorkflow
{
    class Program
    {
        static StartPurchaseOrder poImpl;
        static Guid workflowInstanceId;
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            CreateRoles();

            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                workflowRuntime.StartRuntime();

                // Load the workflow type.
                Type type = typeof(PurchaseOrderWorkflow);

                ExternalDataExchangeService dataService = new ExternalDataExchangeService();
                workflowRuntime.AddService(dataService);

                poImpl = new StartPurchaseOrder();
                dataService.AddService(poImpl);

                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                WorkflowInstance instance = workflowRuntime.CreateWorkflow(type);
                workflowInstanceId = instance.InstanceId;
                instance.Start();

                SendPORequestMessage();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }

        static void SendPORequestMessage()
        {
            try
            {
                // Initialize the Purchase Order details
                int itemId = 191;
                float itemCost = 200.00F;
                string itemName = "Graphics Card";
              
                // Send the event to initiate the PO from the workflow participant
                GenericIdentity genIdentity = new GenericIdentity("Amanda");

                poImpl.InvokePORequest(workflowInstanceId, itemId, itemCost, itemName, genIdentity);
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception message: {0}", e.ToString());
            }
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

        static void CreateRoles()
        {
            if (!System.Web.Security.Roles.RoleExists("Clerk"))
            {
                System.Web.Security.Roles.CreateRole("Clerk");

                string[] users = { "Amanda", "Jones", "Simpson", "Peter" };
                string[] ClerkRole = { "Clerk" };

                System.Web.Security.Roles.AddUsersToRoles(users, ClerkRole);
            }

            if (!System.Web.Security.Roles.RoleExists("ProductManager"))
            {
                System.Web.Security.Roles.CreateRole("ProductManager");

                string[] users1 = { "Betty", "Chris", "Anil", "Derek" };
                string[] ProductManagerRole = { "ProductManager" };

                System.Web.Security.Roles.AddUsersToRoles(users1, ProductManagerRole);
            }
        }
    }
}
