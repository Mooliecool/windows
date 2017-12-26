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

namespace Microsoft.Samples.Workflow.FileWatcher
{
    public class FileWatcherService
    {
        private Dictionary<string, FileWatcherSubscription> subscriptions;
        private WorkflowRuntime runtime;

        public FileWatcherService(WorkflowRuntime runtime)
        {
            this.runtime = runtime;
            this.subscriptions = new Dictionary<string, FileWatcherSubscription>();
        }

        public Guid RegisterListener(IComparable queueName, string path, string filter, NotifyFilters notifyFilter, bool includeSubdirectories)
        {
            FileSystemWatcher fileSystemWatcher = new FileSystemWatcher();

            try
            {
                fileSystemWatcher.Path = path;
            }
            catch (ArgumentException)
            {
                Console.WriteLine("Path \"{0}\" not found.  Please see documentation for setup steps.", path);
                return Guid.Empty;
            }
            fileSystemWatcher.Filter = filter;
            fileSystemWatcher.NotifyFilter = notifyFilter;
            fileSystemWatcher.IncludeSubdirectories = includeSubdirectories;

            fileSystemWatcher.Changed += new FileSystemEventHandler(FileSystemWatcher_Handler);
            fileSystemWatcher.Created += new FileSystemEventHandler(FileSystemWatcher_Handler);
            fileSystemWatcher.Deleted += new FileSystemEventHandler(FileSystemWatcher_Handler);
            fileSystemWatcher.Error += new ErrorEventHandler(FileSystemWatcher_Error);
            
            FileWatcherSubscription subscription =
                new FileWatcherSubscription(fileSystemWatcher, WorkflowEnvironment.WorkflowInstanceId, queueName);

            Guid subscriptionId = Guid.NewGuid();

            lock (this.subscriptions)
            {
                this.subscriptions.Add(subscriptionId.ToString(), subscription);
            }

            // Turn the file system watcher on
            fileSystemWatcher.EnableRaisingEvents = true;

            Console.WriteLine("FileWatcherService subscription '" + subscriptionId.ToString() + "' created");

            return subscriptionId;
        }

        public void UnregisterListener(Guid subscriptionId)
        {
            string key = subscriptionId.ToString();
            lock (this.subscriptions)
            {
                if (this.subscriptions.ContainsKey(key))
                {
                    FileSystemWatcher watcher = this.subscriptions[key].FileSystemWatcher;

                    // Turn the file system watcher off
                    watcher.EnableRaisingEvents = false;
                    watcher.Dispose();

                    this.subscriptions.Remove(key);
                    Console.WriteLine("FileWatcherService subscription '" + key + "' removed");
                }
                else
                {
                    Console.WriteLine("FileWatcherService subscription '" + key + "' not found");
                }
            }
        }

        void FileSystemWatcher_Error(object sender, ErrorEventArgs e)
        {
            
        }

        void FileSystemWatcher_Handler(object sender, FileSystemEventArgs e)
        {
            // Sender is the FileSystemWatcher that raised this event
            FileSystemWatcher fileSystemWatcher = (FileSystemWatcher) sender;
            lock (this.subscriptions)
            {
                foreach (FileWatcherSubscription subscription in this.subscriptions.Values)
                {
                    if (subscription.FileSystemWatcher.Equals(fileSystemWatcher))
                    {
                        this.DeliverToWorkflow(subscription, e);
                        return;
                    }
                }
            }
        }

        private void DeliverToWorkflow(FileWatcherSubscription subscription, FileSystemEventArgs fileSystemEventArgs)
        {
            try
            {
                // We can't just use the FileSystemEventArgs because it's not serializable
                FileWatcherEventArgs eventArgs = new FileWatcherEventArgs(fileSystemEventArgs);

                WorkflowInstance workflowInstance = this.runtime.GetWorkflow(subscription.WorkflowInstanceId);
                workflowInstance.EnqueueItem(subscription.QueueName, eventArgs, null, null);
            }
            catch (Exception e)
            {
                // Write the exception out to the Debug console and throw the exception
                System.Diagnostics.Debug.WriteLine(e);
                throw e;
            }
        }
    }
}
