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
using System.Workflow.Runtime;
using System.Workflow.Runtime.Tracking;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime.Hosting;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.Workflow.RuleActionTrackingEventSample
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
            //filter on record type
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

        private static void WriteUserTrackingRecord(UserTrackingRecord userTrackingRecord)
        {
            WriteTitle("User Activity Record");
            Console.WriteLine("EventDataTime: " + userTrackingRecord.EventDateTime.ToString());
            Console.WriteLine("QualifiedId: " + userTrackingRecord.QualifiedName.ToString());
            Console.WriteLine("ActivityType: " + userTrackingRecord.ActivityType.FullName.ToString());
            if (userTrackingRecord.UserData is RuleActionTrackingEvent)
            {
                WriteRuleActionTrackingEvent((RuleActionTrackingEvent)userTrackingRecord.UserData);
            }
        }

        private static void WriteRuleActionTrackingEvent(RuleActionTrackingEvent ruleActionTrackingEvent)
        {
            Console.WriteLine("RuleActionTrackingEvent");
            Console.WriteLine("***********************");
            Console.WriteLine("RuleName: " + ruleActionTrackingEvent.RuleName.ToString());
            Console.WriteLine("ConditionResult: " + ruleActionTrackingEvent.ConditionResult.ToString());
        }
    }

    public class ConsoleTrackingService : TrackingService
    {

        public ConsoleTrackingService()
        {
        }

        protected override bool TryGetProfile(Type workflowType, out TrackingProfile profile)
        {
            // Retrieves the tracking profile for the specified workflow type if one is available.
            // Depending on the workflowType, service can return different tracking profiles.
            // In this sample the same profile for all workflows is returned.
            profile = GetProfile();
            return true;
        }

        protected override TrackingProfile GetProfile(Guid workflowInstanceId)
        {
            // Returns the tracking profile for the specified workflow instance. 
            // This sample does not support reloading/instance profiles.
            throw new NotImplementedException("The method or operation is not implemented.");
        }

        protected override TrackingProfile GetProfile(Type workflowType, Version profileVersionId)
        {
            // Returns the tracking profile for the specified workflow type, qualified by version.
            // In this sample profiles are not modified.
            return GetProfile();
        }

        protected override bool TryReloadProfile(Type workflowType, Guid workflowInstanceId, out TrackingProfile profile)
        {
            // Retrieves a new tracking profile for the specified workflow instance 
            // if the tracking profile has changed since it was last loaded. 
            // Returning false to indicate there are no new profiles.
            profile = null;
            return false;
        }

        protected override TrackingChannel GetTrackingChannel(TrackingParameters parameters)
        {
            // Returns the channel that the runtime tracking infrastructure uses 
            // to send tracking records to the tracking service.
            // For this sample a channel is returned that dumps the tracking record
            // to the command window using Console.WriteLine().
            return new TrackingChannelSample(parameters);
        }

        // Profile creation
        private static TrackingProfile GetProfile()
        {

            // Create a Tracking Profile that covers all user track points.
            TrackingProfile profile = new TrackingProfile();
            profile.Version = new Version("1.0.0");
            
            // Add a TrackPoint to cover all user track points.
            // We want to receive user events generated by any Activity, with any argument type.
            // We could restrict the ActivityType to be PolicyActivity and 
            // ArgumentType to be RuleActionTrackingEvent if we wanted to only get 
            // tracking events from policy execution.
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
