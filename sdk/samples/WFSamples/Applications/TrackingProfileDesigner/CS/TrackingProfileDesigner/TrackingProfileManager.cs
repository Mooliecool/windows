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
using System.Text;
using System.Workflow.Runtime.Tracking;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Diagnostics;
using System.IO;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner
{
    /// <summary>
    /// A facade over the tracking profile APIs that allow us 
    /// to easily map from the designer surface to the tracking profile
    /// </summary>
    class TrackingProfileManager
    {
        private TrackingProfile trackingProfileValue;

        /// <summary>
        /// Constructor
        /// </summary>
        public TrackingProfileManager()
        {
            trackingProfileValue = new TrackingProfile();
            //New profiles default to v1.0.0
            trackingProfileValue.Version = new Version("1.0.0");
        }

        /// <summary>
        /// Constructor that takes a profile
        /// </summary>
        /// <param name="profile"></param>
        public TrackingProfileManager(TrackingProfile profile)
        {
            this.TrackingProfile = profile;
        }

        /// <summary>
        /// Gets the Workflow TrackPoint if one has been created, otherwise returns null.
        /// </summary>
        public WorkflowTrackPoint WorkflowTrackPoint
        {
            get
            {
                if (TrackingProfile.WorkflowTrackPoints.Count > 0)
                    return TrackingProfile.WorkflowTrackPoints[0];
                else return null;
            }
        }

        /// <summary>
        /// Gets or sets the Tracking Profile
        /// </summary>
        public TrackingProfile TrackingProfile
        {
            get
            {
                return trackingProfileValue;
            }
            set
            {
                trackingProfileValue = value;
            }
        }

        /// <summary>
        /// Finds the associated ActivityTrackPoint for a given activity.
        /// Matches by activity type
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        public ActivityTrackPoint GetTrackPointForActivity(Activity activity)
        {
            if (activity != null)
            {
                foreach (ActivityTrackPoint trackPoint in TrackingProfile.ActivityTrackPoints)
                {
                    if (trackPoint.MatchingLocations[0].ActivityType == activity.GetType())
                    {
                        return trackPoint;
                    }
                }
            }
            return null;
        }

        /// <summary>
        /// Returns true if the activity has an associated track point
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        public bool IsTracked(Activity activity)
        {
            return (GetTrackPointForActivity(activity) != null);
        }

        /// <summary>
        /// Returns true if there exists a track point that matches the specified activity because the activity is derived from the track point's activity
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        public bool IsMatchedByDerivedTrackPoint(Activity activity)
        {
            foreach (ActivityTrackPoint trackPoint in TrackingProfile.ActivityTrackPoints)
            {
                if (activity.GetType().IsSubclassOf(trackPoint.MatchingLocations[0].ActivityType) && trackPoint.MatchingLocations[0].MatchDerivedTypes)
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Toggles (removes or adds) a trackpoint for a given activity
        /// </summary>
        /// <param name="activity"></param>
        public void ToggleActivityTrackPoint(Activity activity)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            //If a trackpoint is tracking this activity type, remove it
            if (trackPoint != null)
            {
                TrackingProfile.ActivityTrackPoints.Remove(trackPoint);
            }
            //Otherwise, add a new one
            else
            {
                trackPoint = new ActivityTrackPoint();
                trackPoint.MatchingLocations.Add(new ActivityTrackingLocation(activity.GetType()));
                TrackingProfile.ActivityTrackPoints.Add(trackPoint);
            }
        }

        #region Annotation
        /// <summary>
        /// Gets the annotation for an activity.  If no annotation is present, null is returned.
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        public string GetAnnotation(Activity activity)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null && trackPoint.Annotations.Count > 0)
            {
                return trackPoint.Annotations[0];
            }
            else return null;
        }

        /// <summary>
        /// Sets the annotaiton for an activity or workflow trackpoint
        /// </summary>
        /// <param name="activity"></param>
        /// <param name="annotation"></param>
        public void SetAnnotation(Activity activity, string annotation)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null && trackPoint.Annotations.Count > 0)
            {
                trackPoint.Annotations[0] = annotation;
            }
            else
            {
                trackPoint.Annotations.Add(annotation);
            }
        }
        #endregion

        /// <summary>
        /// Toggles whether the specified member is extracted
        /// </summary>
        /// <param name="activity"></param>
        /// <param name="activityExtract">True if this is an activity extract, false if this is a workflow extract</param>
        /// <param name="member"></param>
        internal void ToggleExtract(Activity activity, bool activityExtract, string member)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null)
            {
                bool removed = false;
                for (int i = trackPoint.Extracts.Count - 1; i > 0; i--)
                {
                    if (trackPoint.Extracts[i].Member == member)
                    {
                        trackPoint.Extracts.Remove(trackPoint.Extracts[i]);
                        removed = true;
                    }
                }
                if (!removed)
                {
                    if (activityExtract)
                    {
                        trackPoint.Extracts.Add(new ActivityDataTrackingExtract(member));
                    }
                    else
                    {
                        trackPoint.Extracts.Add(new WorkflowDataTrackingExtract(member));
                    }
                }
            }
        }

        /// <summary>
        /// Returns true if the associated trackpoint for the activity matches derived types
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        public bool MatchesDerivedTypes(Activity activity)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            return (trackPoint != null && trackPoint.MatchingLocations[0].MatchDerivedTypes);
        }

        /// <summary>
        /// Toggles whether the specified activity status is tracked
        /// </summary>
        /// <param name="activity"></param>
        /// <param name="status"></param>
        internal void ToggleEventStatus(Activity activity, ActivityExecutionStatus status)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null)
            {
                if (trackPoint.MatchingLocations[0].ExecutionStatusEvents.Contains(status))
                {
                    trackPoint.MatchingLocations[0].ExecutionStatusEvents.Remove(status);
                }
                else
                {
                    trackPoint.MatchingLocations[0].ExecutionStatusEvents.Add(status);
                }
            }
        }

        /// <summary>
        /// Returns an error message if the specified activity's track point is not correctly configured
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        internal bool IsActivityValid(Activity activity, out string errorMessage)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null)
            {
                if (trackPoint.MatchingLocations.Count == 0 ||
                    trackPoint.MatchingLocations[0].ExecutionStatusEvents.Count == 0)
                {
                    errorMessage = "Need at least one status event";
                    return true;
                }

            }
            errorMessage = string.Empty;
            return false;
        }

        /// <summary>
        /// Toggles a tracked workflow event for the workflow trackpoint
        /// </summary>
        /// <param name="workflowEvent"></param>
        internal void ToggleEventStatus(TrackingWorkflowEvent workflowEvent)
        {
            if (WorkflowTrackPoint != null && WorkflowTrackPoint.MatchingLocation.Events.Contains(workflowEvent))
            {
                WorkflowTrackPoint.MatchingLocation.Events.Remove(workflowEvent);
                if (TrackingProfile.WorkflowTrackPoints[0].MatchingLocation.Events.Count == 0)
                {
                    TrackingProfile.WorkflowTrackPoints.Clear();
                }
            }
            else
            {
                if (WorkflowTrackPoint == null)
                {
                    TrackingProfile.WorkflowTrackPoints.Add(new WorkflowTrackPoint());
                }
                WorkflowTrackPoint.MatchingLocation.Events.Add(workflowEvent);
            }
        }

        /// <summary>
        /// Deletes a tracking condition for an activity
        /// </summary>
        /// <param name="activity"></param>
        /// <param name="condition"></param>
        internal void DeleteTrackingCondition(Activity activity, ActivityTrackingCondition condition)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null)
            {
                trackPoint.MatchingLocations[0].Conditions.Remove(condition);
            }
        }

        /// <summary>
        /// Saves a tracking condition for an activity
        /// </summary>
        /// <param name="activity"></param>
        /// <param name="key"></param>
        /// <param name="member"></param>
        /// <param name="op"></param>
        /// <param name="value"></param>
        internal void SaveTrackingCondition(Activity activity, ref ActivityTrackingCondition key, string member, ComparisonOperator op, string value)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null)
            {
                if (key == null)
                {
                    key = new ActivityTrackingCondition();
                    trackPoint.MatchingLocations[0].Conditions.Add(key);
                }
                key.Member = member;
                key.Value = value;
                key.Operator = op;
            }
        }

        /// <summary>
        /// Gets the conditions for an activity
        /// </summary>
        /// <param name="activity"></param>
        /// <returns></returns>
        internal TrackingConditionCollection GetTrackingConditions(Activity activity)
        {
            ActivityTrackPoint trackPoint = GetTrackPointForActivity(activity);
            if (trackPoint != null)
            {
                return trackPoint.MatchingLocations[0].Conditions;
            }
            else
            {
                return null;
            }
        }

        #region Serialization
        /// <summary>
        /// Read profile from path
        /// </summary>
        /// <param name="path"></param>
        public void ReadProfile(string path)
        {
            using (StreamReader reader = new StreamReader(path))
            {
                TrackingProfileSerializer serializer = new TrackingProfileSerializer();
                TrackingProfile = serializer.Deserialize(reader);
            }
        }

        /// <summary>
        /// Serialize a profile to a string
        /// </summary>
        /// <returns></returns>
        public string SerializeProfile()
        {
            StringBuilder profileDoc = new StringBuilder();
            using (StringWriter writer = new StringWriter(profileDoc))
            {
                SerializeProfile(writer);
                return writer.GetStringBuilder().ToString();
            }
        }

        /// <summary>
        /// Serialize the profile to a text writer
        /// </summary>
        /// <param name="writer"></param>
        public void SerializeProfile(TextWriter writer)
        {
            TrackingProfileSerializer serializer = new TrackingProfileSerializer();
            serializer.Serialize(writer, TrackingProfile);
        }
        #endregion

    }
}
