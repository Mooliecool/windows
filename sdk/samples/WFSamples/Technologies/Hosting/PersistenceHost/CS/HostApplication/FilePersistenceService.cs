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
using System.IO;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Threading;

namespace Microsoft.Samples.Workflow.PersistenceHost
{
    public class FilePersistenceService : WorkflowPersistenceService
    {
        private bool unloadOnIdle = false;

        public FilePersistenceService(bool unloadOnIdle)
        {
            this.unloadOnIdle = unloadOnIdle;
        }

        // Save the workflow instance state at the point of persistence with option of locking the instance state if it is shared
        // across multiple runtimes or multiple phase instance updates
        protected override void SaveWorkflowInstanceState(Activity rootActivity, bool unlock)
        {
            // Save the workflow
            Guid contextGuid = (Guid)rootActivity.GetValue(Activity.ActivityContextGuidProperty);
            Console.WriteLine("Saving instance: {0}\n", contextGuid);
            SerializeToFile(
                WorkflowPersistenceService.GetDefaultSerializedForm(rootActivity), contextGuid);

            // See when the next timer (Delay activity) for this workflow will expire
            TimerEventSubscriptionCollection timers = (TimerEventSubscriptionCollection)rootActivity.GetValue(TimerEventSubscriptionCollection.TimerCollectionProperty);
            TimerEventSubscription subscription = timers.Peek();
            if (subscription != null)
            {
                // Set a system timer to automatically reload this workflow when its next timer expires
                TimerCallback callback = new TimerCallback(ReloadWorkflow);
                TimeSpan timeDifference = subscription.ExpiresAt - DateTime.UtcNow;
                Timer timer = new System.Threading.Timer(
                    callback,
                    subscription.WorkflowInstanceId,
                    timeDifference < TimeSpan.Zero ? TimeSpan.Zero : timeDifference,
                    new TimeSpan(-1));
            }
        }

        private void ReloadWorkflow(object id)
        {
            // Reload the workflow so that it will continue processing
            this.Runtime.GetWorkflow((Guid)id);
        }

        // Load workflow instance state.
        protected override Activity LoadWorkflowInstanceState(Guid instanceId)
        {
            Console.WriteLine("Loading instance: {0}\n", instanceId);
            byte[] workflowBytes = DeserializeFromFile(instanceId);
            return WorkflowPersistenceService.RestoreFromDefaultSerializedForm(workflowBytes, null);
        }

        // Unlock the workflow instance state.  
        // Instance state locking is necessary when multiple runtimes share instance persistence store
        protected override void UnlockWorkflowInstanceState(Activity state)
        {
            //File locking is not supported in this sample
        }

        // Save the completed activity state.
        protected override void SaveCompletedContextActivity(Activity activity)
        {
            Guid contextGuid = (Guid)activity.GetValue(Activity.ActivityContextGuidProperty);
            Console.WriteLine("Saving completed activity context: {0}", contextGuid);
            SerializeToFile(
                WorkflowPersistenceService.GetDefaultSerializedForm(activity), contextGuid);
        }

        // Load the completed activity state.
        protected override Activity LoadCompletedContextActivity(Guid activityId, Activity outerActivity)
        {
            Console.WriteLine("Loading completed activity context: {0}", activityId);
            byte[] workflowBytes = DeserializeFromFile(activityId);
            Activity deserializedActivities = WorkflowPersistenceService.RestoreFromDefaultSerializedForm(workflowBytes, outerActivity);
            return deserializedActivities;

        }

        protected override bool UnloadOnIdle(Activity activity)
        {
            return unloadOnIdle;
        }
        // Serialize the activity instance state to file
        private void SerializeToFile(byte[] workflowBytes, Guid id)
        {
            String filename = id.ToString();
            FileStream fileStream = null;
            try
            {
                if (File.Exists(filename))
                    File.Delete(filename);

                fileStream = new FileStream(filename, FileMode.CreateNew, FileAccess.Write, FileShare.None);

                // Get the serialized form
                fileStream.Write(workflowBytes, 0, workflowBytes.Length);
            }
            finally
            {
                if (fileStream != null)
                    fileStream.Close();
            }
        }
        // Deserialize the instance state from the file given the instance id 
        private byte[] DeserializeFromFile(Guid id)
        {
            String filename = id.ToString();
            FileStream fileStream = null;
            try
            {
                // File opened for shared reads but no writes by anyone
                fileStream = new FileStream(filename, FileMode.Open, FileAccess.Read, FileShare.Read);
                fileStream.Seek(0, SeekOrigin.Begin);
                byte[] workflowBytes = new byte[fileStream.Length];

                // Get the serialized form
                fileStream.Read(workflowBytes, 0, workflowBytes.Length);

                return workflowBytes;
            }
            finally
            {
                fileStream.Close();
            }
        }

    }
}