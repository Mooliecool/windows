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
using System.Collections.Specialized;

namespace Microsoft.Samples.Workflow.RaiseEventToLoadWorkflow
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static DocumentApprovalService documentApprover = new DocumentApprovalService();

        static void Main()
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                try
                {
                    // Set up the runtime to unload the workflow instance from memory to file using FilePersistenceService
                    workflowRuntime.AddService(new FilePersistenceService(true));

                    // Add document approval service
                    ExternalDataExchangeService dataService = new ExternalDataExchangeService();
                    workflowRuntime.AddService(dataService);

                    dataService.AddService(documentApprover);

                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowIdled += OnWorkflowIdled;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                    workflowRuntime.ServicesExceptionNotHandled += OnExceptionNotHandled;

                    // Start the engine.
                    workflowRuntime.StartRuntime();

                    // Load the workflow type.
                    Type type = typeof(DocumentApprovalWorkflow);
                    workflowRuntime.CreateWorkflow(type).Start();

                    // waitHandle blocks so that the program does not exit till workflow completes
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

        static void OnWorkflowIdled(object sender, WorkflowEventArgs e)
        {
            // Raise the DocumentApproved event
            Console.WriteLine("\nHost:  RaiseDocumentApproval event to load workflow");
            documentApprover.RaiseDocumentApproved(e.WorkflowInstance.InstanceId, "Approver1");
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            // Signal the waitHandle because workflow completed
            waitHandle.Set();
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

        // Local service
        class DocumentApprovalService : IDocumentApproval
        {
            private Guid documentId;
            private string approver;

            // Send the document for approval
            public void RequestDocumentApproval(Guid documentId, string approver)
            {
                this.documentId = documentId;
                this.approver = approver;

                Console.WriteLine("Host:  Sending document for approval");
            }

            // Method to raise a DocumentApproved event to workflow
            public void RaiseDocumentApproved(Guid documentId, String approver)
            {
                this.documentId = documentId;
                this.approver = approver;

                ThreadPool.QueueUserWorkItem(new WaitCallback(RaiseDocumentApprovalEvent), (DocumentApprovalService)this);
            }

            private void RaiseDocumentApprovalEvent(object stateInfo)
            {
                Console.WriteLine("\nHost:  Loading workflow due to event firing...");
                DocumentApprovalService documentApprovalService = stateInfo as DocumentApprovalService;
                EventHandler<DocumentEventArgs> documentApproved = this.DocumentApproved;
                if (documentApproved != null)
                    documentApproved(documentApprovalService.approver, new DocumentEventArgs(documentApprovalService.documentId));
            }

            // Received the document approval
            public event EventHandler<DocumentEventArgs> DocumentApproved;
        }
    }
}
