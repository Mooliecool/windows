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
using System.Collections.Generic;
using System.Globalization;
using System.Threading;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;

namespace Microsoft.Samples.Workflow.UserTrackPoints
{
    class Program
    {
        private const string connectionString = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;";

        private static AutoResetEvent waitHandle = new AutoResetEvent(false);
        

        static void Main()
        {
            try
            {
                // Create the tracking profile to track user track points
                CreateAndInsertTrackingProfile();

                using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
                {
                    // Add the SQL tracking service to the run time
                    workflowRuntime.AddService(new SqlTrackingService(connectionString));

                    workflowRuntime.StartRuntime();

                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;

                    WorkflowInstance workflowInstance = workflowRuntime.CreateWorkflow(typeof(SimpleWorkflow));
                    workflowInstance.Start();


                    Guid workflowInstanceId = workflowInstance.InstanceId;

                    // Wait for the workflow to complete
                    waitHandle.WaitOne();

                    workflowRuntime.StopRuntime();

                    // Get the tracking events from the database
                    GetUserTrackingEvents(workflowInstanceId);
                    Console.WriteLine("Done Running The workflow.");
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

        private static void CreateAndInsertTrackingProfile()
        {
            TrackingProfile profile = new TrackingProfile();

            ActivityTrackPoint trackPoint = new ActivityTrackPoint();
            ActivityTrackingLocation location = new ActivityTrackingLocation(typeof(Activity));
            location.MatchDerivedTypes = true;

            IEnumerable<ActivityExecutionStatus> statuses = Enum.GetValues(typeof(ActivityExecutionStatus)) as IEnumerable<ActivityExecutionStatus>;
            foreach (ActivityExecutionStatus status in statuses)
            {
                location.ExecutionStatusEvents.Add(status);
            }

            trackPoint.MatchingLocations.Add(location);
            profile.ActivityTrackPoints.Add(trackPoint);
            profile.Version = new Version("3.0.0.0");

            
            // Adding a user track point to the tracking profile
            UserTrackPoint utp = new UserTrackPoint();
            
            // Adding a user location to the track point 
            UserTrackingLocation ul = new UserTrackingLocation(typeof(string), typeof(CodeActivity));
            ul.MatchDerivedActivityTypes = true;
            utp.MatchingLocations.Add(ul);
            profile.UserTrackPoints.Add(utp);
            
            
            // Serialize the profile
            TrackingProfileSerializer serializer = new TrackingProfileSerializer();
            StringWriter writer = new StringWriter(new StringBuilder(), CultureInfo.InvariantCulture);
            serializer.Serialize(writer, profile);
            string trackingprofile = writer.ToString();
            InsertTrackingProfile(trackingprofile);
        }

        private static void InsertTrackingProfile(string profile)
        {
            {
                SqlCommand cmd = new SqlCommand();

                cmd.CommandType = CommandType.StoredProcedure;
                cmd.CommandText = "dbo.UpdateTrackingProfile";
                cmd.Connection = new SqlConnection(connectionString);
                try
                {
                    cmd.Parameters.Clear();

                    SqlParameter param1 = new SqlParameter();
                    param1.ParameterName = "@TypeFullName";
                    param1.SqlDbType = SqlDbType.NVarChar;
                    param1.SqlValue = typeof(SimpleWorkflow).ToString();
                    cmd.Parameters.Add(param1);

                    SqlParameter param2 = new SqlParameter();
                    param2.ParameterName = "@AssemblyFullName";
                    param2.SqlDbType = SqlDbType.NVarChar;
                    param2.SqlValue = typeof(SimpleWorkflow).Assembly.FullName;
                    cmd.Parameters.Add(param2);


                    SqlParameter param3 = new SqlParameter();
                    param3.ParameterName = "@Version";
                    param3.SqlDbType = SqlDbType.VarChar;
                    param3.SqlValue = "3.0.0.0";
                    cmd.Parameters.Add(param3);

                    SqlParameter param4 = new SqlParameter();
                    param4.ParameterName = "@TrackingProfileXml";
                    param4.SqlDbType = SqlDbType.NText;
                    param4.SqlValue = profile;
                    cmd.Parameters.Add(param4);

                    cmd.Connection.Open();
                    cmd.ExecuteNonQuery();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                    Console.WriteLine("The Tracking Profile Was not Inserted. If You want to add a new one then please increase the version Number\n");
                }
                finally
                {
                    if ((null != cmd) && (null != cmd.Connection) && (ConnectionState.Closed != cmd.Connection.State))
                        cmd.Connection.Close();
                }
            }
        }

        private static void GetUserTrackingEvents(Guid workflowInstanceId)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            if (sqlTrackingQuery.TryGetWorkflow(workflowInstanceId, out sqlTrackingWorkflowInstance))
            {
                foreach (UserTrackingRecord userTrackingRecord in sqlTrackingWorkflowInstance.UserEvents)
                {
                    Console.WriteLine("\nUser Tracking Event : Event Date Time : {0}, Event Data : {1}\n", userTrackingRecord.EventDateTime.ToString(), userTrackingRecord.UserData.ToString());
                }
            }
            else
            {
                throw new Exception("\nFailed to retrieve workflow instance\n");
            }
        }
    }
}