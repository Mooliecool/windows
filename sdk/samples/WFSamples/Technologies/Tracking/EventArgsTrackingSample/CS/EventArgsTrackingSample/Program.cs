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
using System.IO;
using System.Data;
using System.Threading;
using System.Data.SqlClient;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;
using System.Workflow.Runtime.Hosting;
using System.Runtime.Serialization.Formatters.Binary;

namespace Microsoft.Samples.Workflow.EventArgsTrackingSample
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static string connectionString = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;";

        static void Main()
        {
            // Create WorkflowRuntime
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                try
                {
                    // Add SqlTrackingService
                    SqlTrackingService sqlTrackingService = new SqlTrackingService(connectionString);
                    sqlTrackingService.IsTransactional = false;
                    workflowRuntime.AddService(sqlTrackingService);

                    // Subscribe to Workflow Suspended WorkflowRuntime Event
                    workflowRuntime.WorkflowSuspended += OnWorkflowSuspended;
                    // Subscribe to Workflow Terminated WorkflowRuntime Event
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    // Start WorkflowRuntime
                    workflowRuntime.StartRuntime();

                    // Execute the ExceptionWorkflow Workflow
                    WriteTitle("Executing the exception workflow");
                    WorkflowInstance exceptionWorkflowInstance = workflowRuntime.CreateWorkflow(typeof(ExceptionWorkflow));
                    exceptionWorkflowInstance.Start();
                    waitHandle.WaitOne();
                    QueryAndWriteTrackingInformationToConsole(exceptionWorkflowInstance.InstanceId, TrackingWorkflowEvent.Exception);
                    QueryAndWriteTrackingInformationToConsole(exceptionWorkflowInstance.InstanceId, TrackingWorkflowEvent.Terminated);

                    // Execute the SuspendedWorkflow Workflow
                    WriteTitle("Executing the suspended workflow");
                    WorkflowInstance suspendedWorkflowInstance = workflowRuntime.CreateWorkflow(typeof(SuspendedWorkflow));
                    suspendedWorkflowInstance.Start();
                    waitHandle.WaitOne();
                    QueryAndWriteTrackingInformationToConsole(suspendedWorkflowInstance.InstanceId, TrackingWorkflowEvent.Suspended);

                    // Stop Runtime
                    workflowRuntime.StopRuntime();
                }
                catch (Exception e)
                {
                    Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1} ", e.Source, e.Message);
                }
                finally
                {
                    workflowRuntime.StopRuntime();
                }
            }
        }

        static void OnWorkflowSuspended(object sender, WorkflowSuspendedEventArgs e)
        {
            waitHandle.Set();
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

        // Reading Data from Tracking Database
        static void QueryAndWriteTrackingInformationToConsole(Guid instanceId, TrackingWorkflowEvent workflowEventToFind)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            if (sqlTrackingQuery.TryGetWorkflow(instanceId, out sqlTrackingWorkflowInstance))
            {
                foreach (WorkflowTrackingRecord workflowTrackingRecord in sqlTrackingWorkflowInstance.WorkflowEvents)
                {
                    if (workflowTrackingRecord.TrackingWorkflowEvent == workflowEventToFind)
                    {
                        WriteEventDescriptionAndArgs(workflowTrackingRecord.TrackingWorkflowEvent.ToString(), workflowTrackingRecord.EventArgs, workflowTrackingRecord.EventDateTime);
                        break;
                    }
                }
            }
        }

        // Manipulating and Writing Information to Console
        static void WriteEventDescriptionAndArgs(string eventDescription, object argData, DateTime eventDateTime)
        {
            // checking the type and the corresponding event
            if (argData is TrackingWorkflowSuspendedEventArgs)
            {
                WriteSuspendedEventArgs(eventDescription, (TrackingWorkflowSuspendedEventArgs)argData, eventDateTime);
            }
            if (argData is TrackingWorkflowTerminatedEventArgs)
            {
                WriteTerminatedEventArgs(eventDescription, (TrackingWorkflowTerminatedEventArgs)argData, eventDateTime);
            }
            if (argData is TrackingWorkflowExceptionEventArgs)
            {
                WriteExceptionEventArgs(eventDescription, (TrackingWorkflowExceptionEventArgs)argData, eventDateTime);
            }
        }

        static void WriteSuspendedEventArgs(string eventDescription, TrackingWorkflowSuspendedEventArgs suspendedEventArgs, DateTime eventDataTime)
        {
            Console.WriteLine("\nSuspended Event Arguments Read From Tracking Database:\n");
            Console.WriteLine("EventDataTime: " + eventDataTime.ToString());
            Console.WriteLine("EventDescription: " + eventDescription);
            Console.WriteLine("SuspendedEventArgs Info: " + suspendedEventArgs.Error);
        }

        static void WriteTerminatedEventArgs(string eventDescription, TrackingWorkflowTerminatedEventArgs terminatedEventArgs, DateTime eventDataTime)
        {
            Console.WriteLine("\nTerminated Event Arguments Read From Tracking Database:\n");
            Console.WriteLine("EventDataTime: " + eventDataTime.ToString());
            Console.WriteLine("EventDescription: " + eventDescription);
            if (null != terminatedEventArgs.Exception)
            {
                Console.WriteLine("TerminatedEventArgs Exception Message: " + terminatedEventArgs.Exception.Message.ToString());
            }
        }

        static void WriteExceptionEventArgs(string eventDescription, TrackingWorkflowExceptionEventArgs exceptionEventArgs, DateTime eventDataTime)
        {
            Console.WriteLine("\nException Event Arguments Read From Tracking Database:\n");
            Console.WriteLine("EventDataTime: " + eventDataTime.ToString());
            Console.WriteLine("EventDescription: " + eventDescription);
            if (null != exceptionEventArgs.Exception)
            {
                Console.WriteLine("ExceptionEventArgs Exception Message: " + exceptionEventArgs.Exception.Message.ToString());
            }
            Console.WriteLine("ExceptionEventArgs Original Activity Path: " + exceptionEventArgs.OriginalActivityPath.ToString());
        }

        static void WriteTitle(string title)
        {
            Console.WriteLine("\n***********************************************************");
            Console.WriteLine("\t" + title);
            Console.WriteLine("***********************************************************");
        }
    }
}
