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
using System.Workflow.Runtime.Tracking;

namespace Microsoft.Samples.Workflow.SimpleTrackingSample
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        const string connectionString = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;";

        static void Main()
        {
            try
            {
                using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
                {
                    workflowRuntime.AddService(new SqlTrackingService(connectionString));

                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    WorkflowInstance workflowInstance = workflowRuntime.CreateWorkflow(typeof(SimpleTrackingWorkflow));
                    Guid instanceId = workflowInstance.InstanceId;

                    workflowRuntime.StartRuntime();

                    workflowInstance.Start();

                    waitHandle.WaitOne();

                    workflowRuntime.StopRuntime();

                    GetInstanceTrackingEvents(instanceId);
                    GetActivityTrackingEvents(instanceId);

                    Console.WriteLine("\nDone running the workflow.");
                }
            }
            catch (Exception ex)
            {
                if (ex.InnerException != null)
                    Console.WriteLine(ex.InnerException.Message);
                else
                    Console.WriteLine(ex.Message);
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

        static void GetInstanceTrackingEvents(Guid instanceId)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            sqlTrackingQuery.TryGetWorkflow(instanceId, out sqlTrackingWorkflowInstance);
            if (sqlTrackingWorkflowInstance != null)
            {
                Console.WriteLine("\nInstance Level Events:\n");

                foreach (WorkflowTrackingRecord workflowTrackingRecord in sqlTrackingWorkflowInstance.WorkflowEvents)
                {
                    Console.WriteLine("EventDescription : {0}  DateTime : {1}", workflowTrackingRecord.TrackingWorkflowEvent, workflowTrackingRecord.EventDateTime);
                }
            }
        }

        static void GetActivityTrackingEvents(Guid instanceId)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            sqlTrackingQuery.TryGetWorkflow(instanceId, out sqlTrackingWorkflowInstance);
            if (sqlTrackingWorkflowInstance != null)
            {
                Console.WriteLine("\nActivity Tracking Events:\n");

                foreach (ActivityTrackingRecord activityTrackingRecord in sqlTrackingWorkflowInstance.ActivityEvents)
                {
                    Console.WriteLine("StatusDescription : {0}  DateTime : {1} Activity Qualified ID : {2}", activityTrackingRecord.ExecutionStatus, activityTrackingRecord.EventDateTime, activityTrackingRecord.QualifiedName);
                }
            }
        }
    }
}