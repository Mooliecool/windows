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

namespace Microsoft.Samples.Workflow.FileWatcher
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            using (WorkflowRuntime runtime = new WorkflowRuntime())
            {
                runtime.AddService(new FileWatcherService(runtime));

                runtime.WorkflowCompleted += OnWorkflowCompleted;
                runtime.WorkflowTerminated += OnWorkflowTerminated;

                runtime.StartRuntime();
                Type type = typeof(Microsoft.Samples.Workflow.FileWatcher.FileWatcherWorkflow);
                runtime.CreateWorkflow(type).Start();

                waitHandle.WaitOne();
                runtime.StopRuntime();

                Console.WriteLine("The workflow is complete.");
            }
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            waitHandle.Set();
        }
    }
}
