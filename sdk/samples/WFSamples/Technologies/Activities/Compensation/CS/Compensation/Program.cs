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
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;

namespace Microsoft.Samples.Workflow.Compensation
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
                    const string connectString = "Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;";

                    workflowRuntime.AddService(new SqlWorkflowPersistenceService(connectString));

                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                    workflowRuntime.WorkflowAborted += OnWorkflowAborted;

                    workflowRuntime.StartRuntime();
                    Type type = typeof(Compensation.PurchaseOrder);
                    workflowRuntime.CreateWorkflow(type).Start();

                    waitHandle.WaitOne();
                }
                catch (Exception ex)
                {
                    if (ex.InnerException != null)
                        Console.WriteLine(ex.InnerException.Message);
                    else
                        Console.WriteLine(ex.Message);
                }
                finally
                {
                    workflowRuntime.StopRuntime();
                }
            }
        }

        static void OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow aborted: Please check database connectivity");
            waitHandle.Set();
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("Ending workflow...");
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
