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
using System.Collections.ObjectModel;
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.CancelWorkflow
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static ExpenseReportServiceImpl expenseService = new ExpenseReportServiceImpl();

        static void Main()
        {
            string connectionString = "Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;";

            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                ExternalDataExchangeService dataService = new ExternalDataExchangeService();
                workflowRuntime.AddService(dataService);
                dataService.AddService(expenseService);

                workflowRuntime.AddService(new SqlWorkflowPersistenceService(connectionString));
                workflowRuntime.StartRuntime();

                workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                workflowRuntime.WorkflowIdled += OnWorkflowIdled;
                workflowRuntime.WorkflowAborted += OnWorkflowAborted;

                Type type = typeof(Microsoft.Samples.Workflow.CancelWorkflow.SampleWorkflow);
                WorkflowInstance workflowInstance = workflowRuntime.CreateWorkflow(type);
                workflowInstance.Start();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }

        static void OnWorkflowIdled(object sender, WorkflowEventArgs e)
        {
            WorkflowInstance workflow = e.WorkflowInstance;

            Console.WriteLine("\n...waiting for 3 seconds... \n");
            Thread.Sleep(3000);
            
            // what activity is blocking the workflow
            ReadOnlyCollection<WorkflowQueueInfo> wqi = workflow.GetWorkflowQueueData();
            foreach (WorkflowQueueInfo q in wqi)
            {
                EventQueueName eq = q.QueueName as EventQueueName;
                if (eq != null)
                {
                    // get activity that is waiting for event
                    ReadOnlyCollection<string> blockedActivity = q.SubscribedActivityNames;
                    Console.WriteLine("Host: Workflow is blocked on " + blockedActivity[0]);
                    
                    // this event is never going to arrive eg. employee left the company
                    // lets send an exception to this queue
                    // it will either be handled by exception handler that was modeled in workflow
                    // or the runtime will unwind running compensation handlers and exit the workflow
                    Console.WriteLine("Host: This event is not going to arrive");
                    Console.WriteLine("Host: Cancel workflow with unhandled exception");
                    workflow.EnqueueItem(q.QueueName, new Exception("ExitWorkflowException"), null, null);
                }
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

        static void OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow aborted: Please check database connectivity");
            waitHandle.Set();
        }

        // class that implements the abstract IExpenseReportService
        class ExpenseReportServiceImpl : IExpenseReportService
        {
            string expenseId;

            // Expense report is created in the system 
            // This method is invoked by the SubmitExpense activity
            public void SubmitExpense(string Id)
            {
                Console.WriteLine("Host: expense report sent");
                expenseId = Id;
            }

            // This method corresponds to the event being raised to approve the order
            public void ApprovalResult(WorkflowInstance workflow, string approval)
            {
                EventHandler<ExpenseReportEventArgs> expenseApproval = this.ExpenseApproval;
                if (expenseApproval != null)
                    expenseApproval(workflow, new ExpenseReportEventArgs(workflow.InstanceId, approval));
            }

            public event EventHandler<ExpenseReportEventArgs> ExpenseApproval;
        }

    }
}
