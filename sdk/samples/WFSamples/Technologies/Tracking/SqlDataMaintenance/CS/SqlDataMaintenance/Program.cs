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

namespace Microsoft.Samples.Workflow.SqlDataMaintenance
{
    class Program
    {
        private static AutoResetEvent waitHandle;
        
        static void Main()
        {
            try
            {
                waitHandle = new AutoResetEvent(false);
                
                DataAccess.CreateAndInsertTrackingProfile();
                using (WorkflowRuntime runtime = new WorkflowRuntime())
                {
                    SqlTrackingService trackingService = new SqlTrackingService(DataAccess.connectionString);

                    /*
                     *  Set partitioning settings on Sql Tracking Service and database
                     */

                    //Turn on PartitionOnCompletion setting-- Default is false
                    trackingService.PartitionOnCompletion = true;

                    //Set partition interval-- Default is 'm' (monthly)
                    DataAccess.SetPartitionInterval('d');

                    runtime.AddService(trackingService);
                    runtime.StartRuntime();

                    runtime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e)
                    {
                       waitHandle.Set();
                    };
                    
                    runtime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                    {
                        Console.WriteLine(e.Exception.Message);
                        waitHandle.Set();
                    };

                    WorkflowInstance instance = runtime.CreateWorkflow(typeof(SimpleWorkflow));
                    instance.Start();
                    waitHandle.WaitOne();

                    runtime.StopRuntime();
                    DataAccess.GetWorkflowTrackingEvents(instance.InstanceId);
                    Console.WriteLine("\nDone running the workflow.");

                    /*
                     *  Show tracking partition information and tables
                     */

                    DataAccess.ShowTrackingPartitionInformation();
                    DataAccess.ShowPartitionTableInformation();
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
    }
}