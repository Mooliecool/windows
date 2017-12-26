//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Data;
using System.Data.SqlClient;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;

namespace Microsoft.Workflow.Samples
{
    class Program
    {
        private static AutoResetEvent waitHandle;
        private const string connectionString = "Initial Catalog=Tracking;Data Source=localhost\\SQLEXPRESS;Integrated Security=SSPI;";

        static void Main()
        {
            try
            {
                waitHandle = new AutoResetEvent(false);
                CreateAndInsertTrackingProfile();
                using (WorkflowRuntime runtime = new WorkflowRuntime())
                {
                    SqlTrackingService trackingService = new SqlTrackingService(connectionString);
                    runtime.AddService(trackingService);
                    runtime.StartRuntime();
                    runtime.WorkflowCompleted += OnWorkflowCompleted;
                    runtime.WorkflowTerminated += OnWorkflowTerminated;
                    runtime.WorkflowAborted += OnWorkflowAborted;

                    WorkflowInstance instance = runtime.CreateWorkflow(typeof(BankMachineWorkflow));
                    instance.Start();
                    waitHandle.WaitOne();

                    runtime.StopRuntime();
                    OutputTrackedData(instance.InstanceId);
                    
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

        private static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }


        private static void OnWorkflowAborted(object sender, WorkflowEventArgs e)
        {
            Console.WriteLine("Workflow has been aborted.");
            waitHandle.Set();
        }

        private static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs instance)
        {
            Console.WriteLine("\nYour transaction has finished. Thank you for your service.");
            waitHandle.Set();
        }

        private static void CreateAndInsertTrackingProfile()
        {
            TrackingProfile profile = new TrackingProfile();

            // Create an activity track point, used for tracking data from Code Activities.
            ActivityTrackPoint codeActivityTrackPoint = new ActivityTrackPoint();

            // Create an ActivityTrackingLocation to be added to the track point.
            ActivityTrackingLocation codeActivityTrackingLocation = new ActivityTrackingLocation("CodeActivity");
            codeActivityTrackingLocation.MatchDerivedTypes = true;
            // Add the location "Closed" event to track.
            codeActivityTrackingLocation.ExecutionStatusEvents.Add(ActivityExecutionStatus.Closed);
            codeActivityTrackPoint.MatchingLocations.Add(codeActivityTrackingLocation);

            // Create a WorkflowDataTrackingExtract for extracting data from the Balance property. 
            WorkflowDataTrackingExtract balanceWorkflowTrackingExtract = new WorkflowDataTrackingExtract();
            balanceWorkflowTrackingExtract.Member = "Balance";

            // Create an activity track point, used for tracking data in the custom activity "ServiceCharge".
            ActivityTrackPoint customActivityTrackPoint = new ActivityTrackPoint();
            ActivityTrackingLocation customActivityTrackingLocation = new ActivityTrackingLocation("ServiceCharge");
            // Create an ActivityTrackingLocation to be added to the track point
            customActivityTrackingLocation.ExecutionStatusEvents.Add(ActivityExecutionStatus.Closed);
            customActivityTrackPoint.MatchingLocations.Add(customActivityTrackingLocation);

            // Create an ActivityDataTrackingExtract for extracting Fee property data from the ServiceCharge activity.
            ActivityDataTrackingExtract feeActivityTrackingExtract = new ActivityDataTrackingExtract();
            feeActivityTrackingExtract.Member = "Fee";
            
            // Add extracts to the activity tracking points.
            codeActivityTrackPoint.Extracts.Add(balanceWorkflowTrackingExtract);
            customActivityTrackPoint.Extracts.Add(feeActivityTrackingExtract);

            profile.ActivityTrackPoints.Add(codeActivityTrackPoint);
            profile.ActivityTrackPoints.Add(customActivityTrackPoint);
            profile.Version = new Version("3.0.0.0");

            // Serialize the profile.
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
                using (command.Connection = new SqlConnection(connectionString))
                {
                    try
                    {
                        SqlParameter typeFullName = new SqlParameter();
                        typeFullName.ParameterName = "@TypeFullName";
                        typeFullName.SqlDbType = SqlDbType.NVarChar;
                        typeFullName.SqlValue = typeof(BankMachineWorkflow).ToString();
                        command.Parameters.Add(typeFullName);

                        SqlParameter assemblyFullName = new SqlParameter();
                        assemblyFullName.ParameterName = "@AssemblyFullName";
                        assemblyFullName.SqlDbType = SqlDbType.NVarChar;
                        assemblyFullName.SqlValue = typeof(BankMachineWorkflow).Assembly.FullName;
                        command.Parameters.Add(assemblyFullName);

                        SqlParameter versionId = new SqlParameter();
                        versionId.ParameterName = "@Version";
                        versionId.SqlDbType = SqlDbType.VarChar;
                        versionId.SqlValue = "3.0.0.0";
                        command.Parameters.Add(versionId);

                        SqlParameter trackingProfile = new SqlParameter();
                        trackingProfile.ParameterName = "@TrackingProfileXml";
                        trackingProfile.SqlDbType = SqlDbType.NVarChar;
                        trackingProfile.SqlValue = profile;
                        command.Parameters.Add(trackingProfile);

                        command.Connection.Open();
                        command.ExecuteNonQuery();
                        command.Connection.Close();
                    }
                    catch (SqlException e)
                    {
                        Console.WriteLine(e.Message);
                        Console.WriteLine("The tracking profile was not inserted. If you wish to add a new tracking profile, please increase the tracking profile's version number.");
                    }
                }
            }
        }


        private static void OutputTrackedData(Guid instanceId)
        {
            SqlTrackingQuery sqlDataTrackingQuery = new SqlTrackingQuery(connectionString);

            SqlTrackingWorkflowInstance sqlTrackingWorkflowInstance;
            if (sqlDataTrackingQuery.TryGetWorkflow(instanceId, out sqlTrackingWorkflowInstance))
            {
                Console.WriteLine("\nOutputting data tracked from the workflow:\n");

                foreach (ActivityTrackingRecord activityDataTrackingRecord in sqlTrackingWorkflowInstance.ActivityEvents)
                {
                    foreach (TrackingDataItem dataItem in activityDataTrackingRecord.Body)
                    {
                        // Output data queried from TrackingDataItem table in the database.
                        Console.WriteLine("At the {0} event for the {1}:\n{2} = {3}",
                        activityDataTrackingRecord.ExecutionStatus, activityDataTrackingRecord.QualifiedName,
                        dataItem.FieldName, dataItem.Data);
                    }
                }
            }
            else
            {
                Console.WriteLine("Could not retrieve data for workflow with instance id {0}", instanceId);
            }
        }
    }
}
