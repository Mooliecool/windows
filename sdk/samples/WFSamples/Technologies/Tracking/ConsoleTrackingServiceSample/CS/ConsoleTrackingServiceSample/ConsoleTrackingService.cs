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
using System.Collections.Generic;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime.Hosting;

namespace Microsoft.Samples.Workflow.ConsoleTrackingServiceSample
{
    public class TrackingChannelSample : TrackingChannel
    {
        private TrackingParameters parameters = null;

        protected TrackingChannelSample()
        {
        }

        public TrackingChannelSample(TrackingParameters parameters)
        {
            this.parameters = parameters;
        }

        // Send() is called by Tracking runtime to send various tracking records
        protected override void Send(TrackingRecord record)
        {
            // filter on record type
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

        private static void WriteTitle (string title)
        {
            Console.WriteLine("\n***********************************************************");
            Console.WriteLine("\t" + title);
            Console.WriteLine("***********************************************************");
        }

        private static void WriteWorkflowTrackingRecord(WorkflowTrackingRecord workflowTrackingRecord)
        {
            WriteTitle("Workflow Tracking Record");
            Console.WriteLine("EventDateTime: " + workflowTrackingRecord.EventDateTime.ToString());
            Console.WriteLine("Status: " + workflowTrackingRecord.TrackingWorkflowEvent.ToString());        
        }

        private static void WriteActivityTrackingRecord(ActivityTrackingRecord activityTrackingRecord)
        {
            WriteTitle("Activity Tracking Record");
            Console.WriteLine("EventDateTime: " + activityTrackingRecord.EventDateTime.ToString());
            Console.WriteLine("QualifiedName: " + activityTrackingRecord.QualifiedName.ToString());
            Console.WriteLine("Type: " + activityTrackingRecord.ActivityType);
            Console.WriteLine("Status: " + activityTrackingRecord.ExecutionStatus.ToString());
        }

        private static void WriteUserTrackingRecord(UserTrackingRecord userTrackingRecord)
        {
            WriteTitle("User Activity Record"); 
            Console.WriteLine("EventDateTime: " + userTrackingRecord.EventDateTime.ToString());
            Console.WriteLine("QualifiedName: " + userTrackingRecord.QualifiedName.ToString());
            Console.WriteLine("ActivityType: " + userTrackingRecord.ActivityType.FullName.ToString());
            Console.WriteLine("Args: " + userTrackingRecord.UserData.ToString());
        }
    }

    public class ConsoleTrackingService : TrackingService
    {
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
            // Returning false to indicate there is no new profiles
            profile = null;
            return false;
        }

        protected override TrackingChannel GetTrackingChannel(TrackingParameters parameters)
        {
            //return a tracking channel to receive runtime events
            return new TrackingChannelSample(parameters);
        }

        // Profile creation
        private static TrackingProfile GetProfile()
        {
            // Create a Tracking Profile
            TrackingProfile profile = new TrackingProfile();
            profile.Version = new Version("3.0.0");

            // Add a TrackPoint to cover all activity status events
            ActivityTrackPoint activityTrackPoint = new ActivityTrackPoint();
            ActivityTrackingLocation activityLocation = new ActivityTrackingLocation(typeof(Activity));
            activityLocation.MatchDerivedTypes = true;
            WorkflowTrackingLocation wLocation = new WorkflowTrackingLocation();

            IEnumerable<ActivityExecutionStatus> statuses = Enum.GetValues(typeof(ActivityExecutionStatus)) as IEnumerable<ActivityExecutionStatus>;
            foreach (ActivityExecutionStatus status in statuses)
            {
                activityLocation.ExecutionStatusEvents.Add(status);
            }

            activityTrackPoint.MatchingLocations.Add(activityLocation);
            profile.ActivityTrackPoints.Add(activityTrackPoint);

            // Add a TrackPoint to cover all workflow status events
            WorkflowTrackPoint workflowTrackPoint = new WorkflowTrackPoint();
            workflowTrackPoint.MatchingLocation = new WorkflowTrackingLocation();
            foreach (TrackingWorkflowEvent workflowEvent in Enum.GetValues(typeof(TrackingWorkflowEvent)))
            {
                workflowTrackPoint.MatchingLocation.Events.Add(workflowEvent);
            }
            profile.WorkflowTrackPoints.Add(workflowTrackPoint);
            
            // Add a TrackPoint to cover all user track points
            UserTrackPoint userTrackPoint = new UserTrackPoint();
            UserTrackingLocation userLocation = new UserTrackingLocation();
            userLocation.ActivityType = typeof(Activity);
            userLocation.MatchDerivedActivityTypes = true;
            userLocation.ArgumentType = typeof(object);
            userLocation.MatchDerivedArgumentTypes = true;
            userTrackPoint.MatchingLocations.Add(userLocation);
            profile.UserTrackPoints.Add(userTrackPoint);

            return profile;
        }
    }
}
