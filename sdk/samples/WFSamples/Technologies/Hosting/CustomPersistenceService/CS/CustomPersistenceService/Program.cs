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
using System.Collections.Specialized;

namespace Microsoft.Samples.Workflow.CustomPersistenceService
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
                    // engine will unload workflow instance when it is idle
                    workflowRuntime.AddService(new FilePersistenceService(true));

                    workflowRuntime.WorkflowCreated += OnWorkflowCreated;
                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowIdled += OnWorkflowIdle;
                    workflowRuntime.WorkflowUnloaded += OnWorkflowUnload;
                    workflowRuntime.WorkflowLoaded += OnWorkflowLoad;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                    workflowRuntime.ServicesExceptionNotHandled += OnExceptionNotHandled;

                    workflowRuntime.CreateWorkflow(typeof(PersistenceServiceWorkflow)).Start();

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

        static void OnExceptionNotHandled(object sender, ServicesExceptionNotHandledEventArgs e)
        {
            Console.WriteLine("Unhandled Workflow Exception ");
            Console.WriteLine("  Type: " + e.GetType().ToString());
            Console.WriteLine("  Message: " + e.Exception.Message);
        }

        static void OnWorkflowCreated(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow created \n");
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("Workflow completed \n");
            waitHandle.Set();
        }

        static void OnWorkflowIdle(object sender, WorkflowEventArgs e)
        {   
            Console.WriteLine("Workflow idling \n");
        }

        static void OnWorkflowUnload(object sender, WorkflowEventArgs e)
        {  
            Console.WriteLine("Workflow unloaded \n");
        }

        static void OnWorkflowLoad(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow loaded \n");
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }
    }
}
