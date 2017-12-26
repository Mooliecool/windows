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
using System.Threading;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Globalization;


namespace Microsoft.Samples.Workflow.QueryUsingSqlTrackingService
{
    class Program
    {
        private static AutoResetEvent waitHandle;
        private const string connectionString = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;";
        private static Version version;

        static void Main()
        {
            try
            {
                waitHandle = new AutoResetEvent(false);
                version = GetTrackingProfileVersion(new Version("3.0.0.0"));
                CreateAndInsertTrackingProfile();
                using (WorkflowRuntime runtime = new WorkflowRuntime())
                {
                    SqlTrackingService trackingService = new SqlTrackingService(connectionString);
                    runtime.AddService(trackingService);
                    runtime.StartRuntime();
                    runtime.WorkflowCompleted += OnWorkflowCompleted;
                    runtime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                    {
                        Console.WriteLine(e.Exception.Message);
                        waitHandle.Set();
                    };

                    WorkflowInstance instance = runtime.CreateWorkflow(typeof(SimpleWorkflow));
                    instance.Start();
                    waitHandle.WaitOne();

                    runtime.StopRuntime();
                    OutputWorkflowTrackingEvents(instance.InstanceId);
                    OutputActivityTrackingEvents(instance.InstanceId);
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

        private static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs instance)
        {
            waitHandle.Set();
        }

        private static void CreateAndInsertTrackingProfile()
        {
            TrackingProfile profile = new TrackingProfile();
            ActivityTrackPoint activityTrack = new ActivityTrackPoint();
            ActivityTrackingLocation activityLocation = new ActivityTrackingLocation(typeof(Activity));
            activityLocation.MatchDerivedTypes = true;
            IEnumerable<ActivityExecutionStatus> statuses = Enum.GetValues(typeof(ActivityExecutionStatus)) as IEnumerable<ActivityExecutionStatus>;
            foreach (ActivityExecutionStatus status in statuses)
            {
                activityLocation.ExecutionStatusEvents.Add(status);
            }

            activityTrack.MatchingLocations.Add(activityLocation);
            profile.ActivityTrackPoints.Add(activityTrack);
            profile.Version = version;

            WorkflowTrackPoint workflowTrack = new WorkflowTrackPoint();
            WorkflowTrackingLocation workflowLocation = new WorkflowTrackingLocation();
            IEnumerable<TrackingWorkflowEvent> eventStatuses = Enum.GetValues(typeof(TrackingWorkflowEvent)) as IEnumerable<TrackingWorkflowEvent>;
            foreach (TrackingWorkflowEvent status in eventStatuses)
            {
                workflowLocation.Events.Add(status);
            }
            
            workflowTrack.MatchingLocation = workflowLocation;
            profile.WorkflowTrackPoints.Add(workflowTrack);

            TrackingProfileSerializer serializer = new TrackingProfileSerializer();
            StringWriter writer = new StringWriter(new StringBuilder(),CultureInfo.InvariantCulture);
            serializer.Serialize(writer, profile);
            String trackingprofile = writer.ToString();
            InsertTrackingProfile(trackingprofile);
        }

        private static void InsertTrackingProfile(string profile)
        {
            using (SqlCommand command = new SqlCommand())
            {

                command.CommandType = CommandType.StoredProcedure;
                command.CommandText = "dbo.UpdateTrackingProfile";
                command.Connection = new SqlConnection(connectionString);
                try
                {
                    SqlParameter typeFullName = new SqlParameter();
                    typeFullName.ParameterName = "@TypeFullName";
                    typeFullName.SqlDbType = SqlDbType.NVarChar;
                    typeFullName.SqlValue = typeof(SimpleWorkflow).ToString();
                    command.Parameters.Add(typeFullName);

                    SqlParameter assemblyFullName = new SqlParameter();
                    assemblyFullName.ParameterName = "@AssemblyFullName";
                    assemblyFullName.SqlDbType = SqlDbType.NVarChar;
                    assemblyFullName.SqlValue = typeof(SimpleWorkflow).Assembly.FullName;
                    command.Parameters.Add(assemblyFullName);

                    SqlParameter versionId = new SqlParameter();
                    versionId.ParameterName = "@Version";
                    versionId.SqlDbType = SqlDbType.VarChar;
                    versionId.SqlValue = version.ToString();
                    command.Parameters.Add(versionId);

                    SqlParameter trackingProfile = new SqlParameter();
                    trackingProfile.ParameterName = "@TrackingProfileXml";
                    trackingProfile.SqlDbType = SqlDbType.NVarChar;
                    trackingProfile.SqlValue = profile;
                    command.Parameters.Add(trackingProfile);

                    command.Connection.Open();
                    command.ExecuteNonQuery();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.Message);
                    Console.WriteLine("The tracking profile was not inserted. If you wish to add a new tracking profile, please increase the tracking profile's version number.");
                }
            }
        }

        private static Version GetTrackingProfileVersion(Version version)
        {
            TrackingProfile profile = null;
            SqlDataReader reader = null;
            using (SqlCommand command = new SqlCommand())
            {
                command.CommandType = CommandType.StoredProcedure;
                command.CommandText = "dbo.GetTrackingProfile";
                command.Connection = new SqlConnection(connectionString);
                SqlParameter typeFullName = new SqlParameter();
                typeFullName.ParameterName = "@TypeFullName";
                typeFullName.SqlDbType = SqlDbType.NVarChar;
                typeFullName.SqlValue = typeof(SimpleWorkflow).FullName;
                command.Parameters.Add(typeFullName);

                SqlParameter assemblyFullName = new SqlParameter();
                assemblyFullName.ParameterName = "@AssemblyFullName";
                assemblyFullName.SqlDbType = SqlDbType.NVarChar;
                assemblyFullName.SqlValue = typeof(SimpleWorkflow).Assembly.FullName;
                command.Parameters.Add(assemblyFullName);

                SqlParameter versionId = new SqlParameter();
                versionId.ParameterName = "@Version";
                versionId.SqlDbType = SqlDbType.VarChar;
                command.Parameters.Add(versionId);

                SqlParameter createDefault = new SqlParameter();
                createDefault.ParameterName = "@CreateDefault";
                createDefault.SqlDbType = SqlDbType.Bit;
                createDefault.SqlValue = 0;
                command.Parameters.Add(createDefault);

                try
                {
                    command.Connection.Open();
                    reader = command.ExecuteReader();
                    if (reader.Read())
                    {
                        string profileXml = reader[0] as string;
                        if (null != profileXml)
                        {
                            TrackingProfileSerializer serializer = new TrackingProfileSerializer();
                            StringReader stringReader = null;
                            try
                            {
                                stringReader = new StringReader(profileXml);
                                profile = serializer.Deserialize(stringReader);
                            }
                            finally
                            {
                                if (stringReader != null)
                                    stringReader.Close();
                            }
                        }
                    }
                }
                finally
                {
                    if (reader != null && !reader.IsClosed)
                        reader.Close();
                }
            }

            if (profile != null)
                return new Version(profile.Version.Major, profile.Version.MajorRevision, profile.Version.Minor, profile.Version.MinorRevision + 1);
            else
                return new Version(version.Major, version.MajorRevision, version.Minor, version.MinorRevision + 1);
        }

        private static void OutputWorkflowTrackingEvents(Guid instanceId)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            if (sqlTrackingQuery.TryGetWorkflow(instanceId, out sqlTrackingWorkflowInstance))
            {
                Console.WriteLine("\nInstance Level Events:\n");

                foreach (WorkflowTrackingRecord workflowTrackingRecord in sqlTrackingWorkflowInstance.WorkflowEvents)
                {
                    Console.WriteLine("EventDescription : {0}  DateTime : {1}", workflowTrackingRecord.TrackingWorkflowEvent, workflowTrackingRecord.EventDateTime);
                }
            }
        }

        private static void OutputActivityTrackingEvents(Guid instanceId)
        {
            SqlTrackingQuery sqlTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            if (sqlTrackingQuery.TryGetWorkflow(instanceId, out sqlTrackingWorkflowInstance))
            {
                Console.WriteLine("\nActivity Tracking Events:\n");

                foreach (ActivityTrackingRecord activityTrackingRecord in sqlTrackingWorkflowInstance.ActivityEvents)
                {
                    Console.WriteLine("StatusDescription : {0}  DateTime : {1} Activity Qualified Name : {2}", activityTrackingRecord.ExecutionStatus, activityTrackingRecord.EventDateTime, activityTrackingRecord.QualifiedName);
                }
            }
        }
    }
}