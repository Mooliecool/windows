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
using System.Data;
using System.Data.SqlClient;
using System.IO;
using System.Globalization;
using System.Text;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime.Tracking;

namespace Microsoft.Samples.Workflow.SqlDataMaintenance
{
    public static class DataAccess
    {
        private static Version version = GetTrackingProfileVersion(new Version("3.0.0.0"));
        internal const string connectionString = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;";

        internal static void CreateAndInsertTrackingProfile()
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
            StringWriter writer = new StringWriter(new StringBuilder(), CultureInfo.InvariantCulture);
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
            if (profile != null)
                return new Version(profile.Version.Major, profile.Version.MajorRevision, profile.Version.Minor, profile.Version.MinorRevision + 1);
            else
                return new Version(version.Major, version.MajorRevision, version.Minor, version.MinorRevision + 1);
        }

        internal static void GetWorkflowTrackingEvents(Guid instanceId)
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

        internal static void SetPartitionInterval(Char interval)
        {
            // Valid values are 'd' (daily), 'm' (monthly), and 'y' (yearly).  The default is 'm'.
            using (SqlCommand command = new SqlCommand("dbo.SetPartitionInterval"))
            {
                command.CommandType = CommandType.StoredProcedure;
                command.Connection = new SqlConnection(connectionString);

                SqlParameter intervalParameter = new SqlParameter("@Interval", SqlDbType.Char);
                intervalParameter.SqlValue = interval;
                command.Parameters.Add(intervalParameter);
                command.Connection.Open();
                command.ExecuteNonQuery();
           }
        }
        internal static void ShowTrackingPartitionInformation()
        {
            //Show the contents of the TrackingPartitionName table

            using (SqlCommand command = new SqlCommand("SELECT * FROM vw_TrackingPartitionSetName"))
            {
                SqlDataReader reader = null;
                command.CommandType = CommandType.Text;
                command.Connection = new SqlConnection(connectionString);

                try
                {
                    command.Connection.Open();
                    reader = command.ExecuteReader();
                    if (reader.Read())
                    {
                        Console.WriteLine();
                        Console.WriteLine("***************************");
                        Console.WriteLine("Partition information: ");
                        Console.WriteLine("PartitionId: {0}", reader[0]);
                        Console.WriteLine("Name: {0}", reader[1]);
                        Console.WriteLine("Created: {0}", reader[2]);
                        Console.WriteLine("End: {0}", reader[3] is System.DBNull ? "NULL" : reader[3]);
                        Console.WriteLine("Partition Interval: {0}", reader[4]);
                        Console.WriteLine("***************************");
                    }
                    else
                    {
                        Console.WriteLine();
                        Console.WriteLine("No partition information present.");
                    }
                }
                finally
                {
                    if (reader != null && !reader.IsClosed)
                        reader.Close();
                }
            }
        }
        internal static void ShowPartitionTableInformation()
        {
            //List the names of the partition tables created

            using (SqlCommand command = new SqlCommand(
                "declare @trackingName varchar(255) select @trackingName = Name from TrackingPartitionSetName " +
                "select name from sysobjects where name like '%' + @trackingName"))
            {

                Console.WriteLine();
                Console.WriteLine("***************************");
                Console.WriteLine("Partition tables: ");
                SqlDataReader reader = null;
                command.CommandType = CommandType.Text;
                command.Connection = new SqlConnection(connectionString);
                try
                {
                    command.Connection.Open();
                    reader = command.ExecuteReader();
                    while (reader.Read())
                    {
                        Console.WriteLine(reader[0]);
                    }
                }
                finally
                {
                    if (reader != null && !reader.IsClosed)
                        reader.Close();
                }


                Console.WriteLine("***************************");
            }
        }
    }
}
