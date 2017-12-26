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
using System.Workflow.Runtime.Tracking;
using System.Xml.Schema;

namespace Microsoft.Samples.Workflow.FileTrackingServiceAndQuerySample
{
    public class SimpleTrackingChannel : TrackingChannel
    {
        public static readonly string TrackingDataFilePrefix = Environment.GetEnvironmentVariable("TEMP") + "\\Tracking_";

        private TrackingParameters trackingParameters = null;
        private static string trackingDataFile = "";
        private StreamWriter streamWriter = null;

        protected SimpleTrackingChannel()
        {
        }

        public SimpleTrackingChannel(TrackingParameters parameters)
        {
            trackingParameters = parameters;
            trackingDataFile = TrackingDataFilePrefix + parameters.InstanceId.ToString() + ".txt";
            Console.WriteLine("Saving tracking information to " + trackingDataFile);
            streamWriter = File.CreateText(trackingDataFile);
            streamWriter.AutoFlush = true;
        }

        // Send() is called by Tracking runtime to send various tracking records
        protected override void Send(TrackingRecord record)
        {
            //filter on record type
            if (record is WorkflowTrackingRecord)
            {
                WriteWorkflowTrackingRecord((WorkflowTrackingRecord)record);
            }
            if (record is ActivityTrackingRecord)
            {
                WriteActivityTrackingRecord((ActivityTrackingRecord)record);
            }
            if (record is UserTrackingRecord)
            {
                WriteUserTrackingRecord((UserTrackingRecord)record);
            }
        }

        // InstanceCompletedOrTerminated() is called by Tracking runtime to indicate that the Workflow instance finished running
        protected override void InstanceCompletedOrTerminated()
        {
            WriteTitle("Workflow Instance Completed or Terminated");
        }

        private void WriteTitle (string title)
        {
            WriteToFile("**** " + title + " ****");
        }

        private void WriteWorkflowTrackingRecord(WorkflowTrackingRecord workflowTrackingRecord)
        {
            WriteToFile("Workflow: " + workflowTrackingRecord.TrackingWorkflowEvent.ToString());        
        }

        private void WriteActivityTrackingRecord(ActivityTrackingRecord activityTrackingRecord)
        {
            WriteToFile("Activity: " + activityTrackingRecord.QualifiedName.ToString() + " " + activityTrackingRecord.ExecutionStatus.ToString());
        }

        private void WriteUserTrackingRecord(UserTrackingRecord userTrackingRecord)
        {
            WriteToFile("User Data: " + userTrackingRecord.UserData.ToString());
        }

        private void WriteToFile(string toWrite)
        {
            try
            {
                if (null != streamWriter)
                {
                    streamWriter.WriteLine(toWrite);
                }
                else
                {
                    throw new Exception("trackingDataFile " + trackingDataFile + " doesn't exist");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1} ", e.Source, e.Message);
            }
        }
    }

    public class SimpleFileTrackingService : TrackingService
    {
        public SimpleFileTrackingService()
        {
        }

        protected override bool TryGetProfile(Type workflowType, out TrackingProfile profile)
        {
            //Depending on the workflowType, service can return different tracking profiles
            //In this sample we're returning the same profile for all running types
            profile = GetProfile();
            return true;
        }

        protected override TrackingProfile GetProfile(Guid workflowInstanceId)
        {
            // Does not support reloading/instance profiles
            throw new NotImplementedException("The method or operation is not implemented.");
        }

        protected override TrackingProfile GetProfile(Type workflowType, Version profileVersionId)
        {
            // Return the version of the tracking profile that runtime requests (profileVersionId)
            return GetProfile();
        }

        protected override bool TryReloadProfile(Type workflowType, Guid workflowInstanceId, out TrackingProfile profile)
        {       
            // Returning false to indicate there are no new profiles
            profile = null;
            return false;
        }

        protected override TrackingChannel GetTrackingChannel(TrackingParameters parameters)
        {
            return new SimpleTrackingChannel(parameters);
        }

        #region Tracking Profile Creation

        // Reads a file containing an XML representation of a Tracking Profile
        private static TrackingProfile GetProfile()
        {
            FileStream fileStream = null;
            try
            {
                string trackingProfileFile = Environment.CurrentDirectory + "\\profile.xml";
                Console.WriteLine("trackingProfileFile is {0}", trackingProfileFile);
                if (File.Exists(trackingProfileFile))
                {
                    Console.WriteLine("Reading trackingProfile from {0}", trackingProfileFile);
                    fileStream = File.OpenRead(trackingProfileFile);
                    if (null == fileStream)
                    {
                        Console.WriteLine("fileStream is null");
                        return null;
                    }
                    StreamReader reader = new StreamReader(fileStream);
                    TrackingProfile profile;
                    TrackingProfileSerializer trackingProfileSerializer = new TrackingProfileSerializer();
                    profile = trackingProfileSerializer.Deserialize(reader);

                    return profile;
                }
                else
                {
                    Console.WriteLine("trackingProfileFile {0} doesn't exist", trackingProfileFile);
                    return null;
                }
            }
            catch (TrackingProfileDeserializationException tpex)
            { 
                Console.WriteLine("Encountered a deserialization exception.");
                foreach (ValidationEventArgs validationError in tpex.ValidationEventArgs)
                {
                    Console.WriteLine("Exception Message: {0}", validationError.Message);
                }
                return null;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1}", ex.Source, ex.Message);
                return null;
            }
            finally
            {
                if (fileStream != null)
                    fileStream.Close();
            }
        }

        #endregion
    }
}
