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
using System.Collections.Specialized;
using System.Diagnostics;
using System.Security;
using System.Threading;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.TerminationTrackingServiceSample
{
    class Program
    {
        const string eventSource = "TerminationTrackingService";
        static void Main(string[] args)
        {
            try
            {
                //Set up event log source if it doesn't already exist
                if (!EventLog.SourceExists(eventSource))
                    EventLog.CreateEventSource(eventSource, "Application");
            }
            catch (SecurityException)
            {
                // Administrator privileges are needed to create an event log source.
                Console.WriteLine("Please run this application once with Administrator privileges to create event source.");
                return;
            }

            using(WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                AutoResetEvent waitHandle = new AutoResetEvent(false);
                NameValueCollection parameters = new NameValueCollection();
                parameters.Add("EventSource", eventSource);

                workflowRuntime.AddService(new TerminationTrackingService(parameters));
                workflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) {waitHandle.Set();};
                workflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    waitHandle.Set();
                };

                WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(SampleWorkflow));
                instance.Start();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }
    }
}
