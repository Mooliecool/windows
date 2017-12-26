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

namespace Microsoft.Samples.Workflow.ThrowSample
{
    class Program
    {
        //A workflow is always run asychronously; the main thread waits on this event so the program
        // doesn't exit before the workflow completes
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main()
        {
            // Create the WorkflowRuntime
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {

                // Set up the WorkflowRuntime events so that the host gets notified when the workflow
                // completes and terminates
                workflowRuntime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(OnWorkflowCompleted);
                workflowRuntime.WorkflowTerminated += new EventHandler<WorkflowTerminatedEventArgs>(OnWorkflowTerminated);

                // Load the workflow type
                Type type = typeof(ThrowWorkflow);

                //Start the workflow and wait for it to complete
                workflowRuntime.CreateWorkflow(type).Start();

                Console.WriteLine("Workflow Started.");
                waitHandle.WaitOne();

                Console.WriteLine("Workflow Completed.");

                workflowRuntime.StopRuntime();
            }
        }

        //This method is called when the workflow is terminated
        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine("Workflow Terminated.");
            Console.WriteLine("   Exception - Reason: " + e.Exception.Message);
            waitHandle.Set();
        }

        //This method is called when the workflow succesfully completes - in this sample this will never be called
        // since the workflow throws an unhandled exception
        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs instance)
        {
            waitHandle.Set();
        }
    }
}
