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
using System.Collections.Specialized;
using System.Text;
using System.Diagnostics;
using System.Workflow.Runtime.Tracking;

namespace Microsoft.Samples.Workflow.TerminationTrackingServiceSample
{
    /// <summary>
    /// Terminate 
    /// </summary>
    public sealed class TerminationTrackingService : TrackingService
    {
        #region Private Data Members

        private volatile static TrackingProfile profile = null;
        private string source = "TerminationTrackingService";
        private bool sourceExists = false;

        #endregion

        #region Constructors

        public TerminationTrackingService()
        {
        }

        public TerminationTrackingService(NameValueCollection parameters)
        {
            if (parameters == null)
                throw new ArgumentNullException("parameters");

            if (parameters.Count > 0)
            {
                foreach (string key in parameters.Keys)
                {
                    if (0 == string.Compare("EventSource", key, StringComparison.OrdinalIgnoreCase))
                    {
                        source = parameters[key];
                        if (null == source)
                            throw new ArgumentNullException("EventSource cannot be null or empty.", "parameters");

                        if (0 == source.Length)
                            throw new ArgumentException("EventSource cannot be null or empty.", "parameters");
                    }
                }
            }
        }

        #endregion

        #region Public Properties

        /// <summary>
        /// The value that will appear in the Event Viewer's Source column. 
        /// </summary>
        public string EventSource
        {
            get { return source; }
            set
            {
                if (null == value)
                    throw new ArgumentNullException("value", "EventSource cannot be null or empty.");

                if (0 == value.Length)
                    throw new ArgumentException("EventSource cannot be null or empty.", "value");
                sourceExists = false;
                source = value;
                //
                // This will throw if we are invalid to inform the host immediately
                ValidateEventLogSource(value);
            }
        }

        #endregion

        #region Public Methods

        protected override void Start()
        {
            base.Start();
            //
            // This will throw if we are invalid to inform the host immediately
            ValidateEventLogSource(source);
        }

        #endregion Public Methods

        #region GetProfile

        /// <summary>
        /// Returns a tracking channel that will receive instnce terminated events.
        /// </summary>
        protected override TrackingChannel GetTrackingChannel(TrackingParameters parameters)
        {
            return new TerminationTrackingChannel(parameters, source);
        }

        /// <summary>
        /// Returns a static tracking profile that only tracks instance terminated events.
        /// </summary>
        protected override bool TryGetProfile(Type workflowType, out TrackingProfile profile)
        {
            profile = GetProfile();
            return true;
        }

        /// <summary>
        /// Returns a static tracking profile that only tracks instance terminated events.
        /// </summary>
        protected override TrackingProfile GetProfile(Type workflowType, Version version)
        {
            return GetProfile();
        }

        /// <summary>
        /// Returns a static tracking profile that only tracks instance terminated events.
        /// </summary>
        protected override TrackingProfile GetProfile(Guid workflowInstanceId)
        {
            return GetProfile();
        }

        private TrackingProfile GetProfile()
        {
            //
            // We shouldn't hit this point without the host ignoring an earlier exception.
            // However if we're here and the source doesn't exist we can't function.
            // Throwing an exception from here will block instance creation
            // but that is better than failing silently on termination 
            // and having the admin think everything is OK because the event log is clear.
            if (!sourceExists)
                throw new InvalidOperationException(string.Format(System.Globalization.CultureInfo.InvariantCulture, "EventLog Source with the name '{0}' does not exist", source));

            //
            // The profile for this instance will never change
            if (null == profile)
            {
                lock (typeof(TerminationTrackingService))
                {
                    if (null == profile)
                    {
                        profile = new TrackingProfile();
                        profile.Version = new Version("3.0.0.0");
                        WorkflowTrackPoint point = new WorkflowTrackPoint();
                        point.MatchingLocation = new WorkflowTrackingLocation();
                        point.MatchingLocation.Events.Add(TrackingWorkflowEvent.Terminated);
                        profile.WorkflowTrackPoints.Add(point);
                    }
                }
            }
            return profile;
        }
        /// <summary>
        /// Always returns false; this tracking service has no need to reload its tracking profile for a running instance.
        /// </summary>
        /// <param name="workflowType"></param>
        /// <param name="workflowInstanceId"></param>
        /// <param name="profile"></param>
        /// <returns></returns>
        protected override bool TryReloadProfile(Type workflowType, Guid workflowInstanceId, out TrackingProfile profile)
        {
            //
            // There is no reason for this service to ever reload a profile
            profile = null;
            return false;
        }
        #endregion

        #region Private Members

        private void ValidateEventLogSource(string source)
        {
            sourceExists = false;
            if (!EventLog.SourceExists(source))
                throw new ArgumentException(string.Format(System.Globalization.CultureInfo.InvariantCulture, "EventLog Source with the name '{0}' does not exist", source));

            sourceExists = true;
        }

        #endregion Private Members

        #region Channel

        /// <summary>
        /// Receives tracking events for a specific instance.
        /// </summary>
        private class TerminationTrackingChannel : TrackingChannel
        {
            private TrackingParameters parametersValue = null;
            private string sourceValue = null;

            protected TerminationTrackingChannel()
            {
            }

            public TerminationTrackingChannel(TrackingParameters parameters, string source)
            {
                if (null == parameters)
                    throw new ArgumentNullException("parameters");

                if (null == source)
                    throw new ArgumentNullException("source");

                if (0 == source.Length)
                    throw new ArgumentException("EventSource cannot be null or empty.", "source");

                sourceValue = source;
                parametersValue = parameters;
            }

            /// <summary>
            /// Receives tracking events.  Instance terminated events are written to the event log.
            /// </summary>
            protected override void Send(TrackingRecord record)
            {
                WorkflowTrackingRecord instanceTrackingRecord = record as WorkflowTrackingRecord;

                if ((null == instanceTrackingRecord) || (TrackingWorkflowEvent.Terminated != instanceTrackingRecord.TrackingWorkflowEvent))
                    return;

                // Create an EventLog instance and assign its source.
                EventLog log = new EventLog();
                log.Source = sourceValue;

                // Write an informational entry to the event log.  
                TrackingWorkflowTerminatedEventArgs terminatedEventArgs = instanceTrackingRecord.EventArgs as TrackingWorkflowTerminatedEventArgs;

                StringBuilder message = new StringBuilder(512);
                message.AppendLine(string.Format(System.Globalization.CultureInfo.InvariantCulture, "Workflow instance {0} has been terminated.", parametersValue.InstanceId.ToString()));
                message.AppendLine();

                if (null != terminatedEventArgs.Exception)
                    message.AppendLine(terminatedEventArgs.Exception.ToString());

                log.WriteEntry(message.ToString(), EventLogEntryType.Warning);
            }

            protected override void InstanceCompletedOrTerminated()
            {
            }
        }
        #endregion
    }
}
