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
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;
using System.Text;
using System.IO;
using System.Workflow.ComponentModel;
using System.Globalization;

namespace Microsoft.Samples.Workflow.TrackingProfileObjectModel
{
    class Program
    {
        static void Main()
        {
            WriteTitle("Extract Name off all events of activityName except compensating event and annotate them");
            SerializeTrackingProfileAndWriteToConsole(GetProfileWithWorkflowDataExtract());

            WriteTitle("Extract all user track points");
            SerializeTrackingProfileAndWriteToConsole(GetProfileWithUserTrackPoint());

            WriteTitle("Extract all workflow events");
            SerializeTrackingProfileAndWriteToConsole(GetProfileAllWorkflowEvents());

            WriteTitle("Deserialize TrackingProfile String");
            DeserializeTrackingProfileString();
        }

        static TrackingProfile GetProfileWithWorkflowDataExtract()
        {
            //Create Tracking Profile
            TrackingProfile trackingProfile = new TrackingProfile();
            trackingProfile.Version = new Version("1.0.0");

            //Create Activity Track Point
            ActivityTrackPoint activityTrackPoint = new ActivityTrackPoint();

            //Create matchingActivityTrackingLocation 
            ActivityTrackingLocation matchingActivityTrackingLocation = new ActivityTrackingLocation();
            //Set ActivityName = "activityName"
            matchingActivityTrackingLocation.ActivityTypeName = "activityName";
            //Add all possible ActivityExecutionStatus to the matchingActivityTrackingLocation Events
            IEnumerable<ActivityExecutionStatus> statuses = Enum.GetValues(typeof(ActivityExecutionStatus)) as IEnumerable<ActivityExecutionStatus>;
            foreach (ActivityExecutionStatus status in statuses)
            {
                matchingActivityTrackingLocation.ExecutionStatusEvents.Add(status);
            }

            //Create matchingActivityTrackingCondition where memberName = memberValue
            ActivityTrackingCondition matchingActivityTrackingCondition = new ActivityTrackingCondition("memberName", "memberValue");
            matchingActivityTrackingCondition.Operator = ComparisonOperator.Equals;

            //Add matchingActivityTrackingCondition to activityTrackingLocation
            matchingActivityTrackingLocation.Conditions.Add(matchingActivityTrackingCondition);

            //Add matchingActivityTrackingCondition to the matching locations for activityTrackPoint
            activityTrackPoint.MatchingLocations.Add(matchingActivityTrackingLocation);

            //Create excludedActivityTrackingLocation 
            ActivityTrackingLocation excludedActivityTrackingLocation = new ActivityTrackingLocation();
            //Set ActivityName = "activityName"
            excludedActivityTrackingLocation.ActivityTypeName = "activityName";
            //Add Compensating ActivityExecutionStatus to the excludedActivityTrackingLocation Events
            excludedActivityTrackingLocation.ExecutionStatusEvents.Add(ActivityExecutionStatus.Compensating);

            //Add excludedActivityTrackingCondition to the excluded locations for activityTrackPoint
            activityTrackPoint.ExcludedLocations.Add(excludedActivityTrackingLocation);

            //Create workflowDataTrackingExtract to extract Workflow data "Name"
            WorkflowDataTrackingExtract workflowDataTrackingExtract = new WorkflowDataTrackingExtract();
            workflowDataTrackingExtract.Member = "Name";

            //Add workflowDataTrackingExtract to activityTrackPoint
            activityTrackPoint.Extracts.Add(workflowDataTrackingExtract);

            //Annotate activityTrackPoint with 
            activityTrackPoint.Annotations.Add("Track Point Annotations");

            //Add ActivityTrackPoints to trackingProfile 
            trackingProfile.ActivityTrackPoints.Add(activityTrackPoint);

            return trackingProfile;
        }

        static TrackingProfile GetProfileWithUserTrackPoint()
        {
            TrackingProfile trackingProfile = new TrackingProfile();
            trackingProfile.Version = new Version("1.0.0");

            // Add a TrackPoint to cover all user track points
            UserTrackPoint userTrackPoint = new UserTrackPoint();
            UserTrackingLocation userLocation = new UserTrackingLocation();
            userLocation.ActivityType = typeof(Activity);
            userLocation.MatchDerivedActivityTypes = true;
            userLocation.ArgumentType = typeof(object);
            userLocation.MatchDerivedArgumentTypes = true;
            userTrackPoint.MatchingLocations.Add(userLocation);
            trackingProfile.UserTrackPoints.Add(userTrackPoint);

            return trackingProfile;
        }

        static TrackingProfile GetProfileAllWorkflowEvents()
        {
            TrackingProfile trackingProfile = new TrackingProfile();
            trackingProfile.Version = new Version("1.0.0");

            // Add a TrackPoint to cover all user track points
            WorkflowTrackPoint workflowTrackPoint = new WorkflowTrackPoint();
            IEnumerable<TrackingWorkflowEvent> statuses = Enum.GetValues(typeof(TrackingWorkflowEvent)) as IEnumerable<TrackingWorkflowEvent>;
            foreach (TrackingWorkflowEvent status in statuses)
            {
                workflowTrackPoint.MatchingLocation.Events.Add(status);
            }
            trackingProfile.WorkflowTrackPoints.Add(workflowTrackPoint);

            return trackingProfile;
        }


        static void SerializeTrackingProfileAndWriteToConsole(TrackingProfile trackingProfile)
        {
            TrackingProfileSerializer trackingProfileSerializer = new TrackingProfileSerializer();
            StringBuilder trackingProfileString = new StringBuilder();
            using (StringWriter writer = new StringWriter(trackingProfileString, CultureInfo.InvariantCulture))
            {
                trackingProfileSerializer.Serialize(writer, trackingProfile);
                Console.WriteLine(writer.ToString());
            }
        }

        static void DeserializeTrackingProfileString()
        {
            string workflowTerminatedTrackingProfile = @"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<TrackingProfile xmlns=""http://schemas.microsoft.com/winfx/2006/workflow/trackingprofile"" version=""2.0.0"">
    <TrackPoints>
        <WorkflowTrackPoint>
            <MatchingLocation>
                <WorkflowTrackingLocation>
                    <TrackingWorkflowEvents>
                        <TrackingWorkflowEvent>Terminated</TrackingWorkflowEvent>
                    </TrackingWorkflowEvents>
                </WorkflowTrackingLocation>
            </MatchingLocation>
        </WorkflowTrackPoint>
    </TrackPoints>
</TrackingProfile>";
            TrackingProfileSerializer trackingProfileSerializer = new TrackingProfileSerializer();
            using (StringReader stringReader = new StringReader(workflowTerminatedTrackingProfile))
            {
                TrackingProfile trackingProfile = trackingProfileSerializer.Deserialize(stringReader);
                Console.WriteLine("Tracking Profile Version " + trackingProfile.Version);
            }
        }

        static void WriteTitle(string title)
        {
            Console.WriteLine("\n*************************************************************************");
            Console.WriteLine(title);
            Console.WriteLine("*************************************************************************");
        }
    }
}
