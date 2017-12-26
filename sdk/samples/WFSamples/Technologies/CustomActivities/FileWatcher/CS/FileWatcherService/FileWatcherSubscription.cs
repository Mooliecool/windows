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

namespace Microsoft.Samples.Workflow.FileWatcher
{
    internal class FileWatcherSubscription
    {
        private FileSystemWatcher fileSystemWatcherValue;
        private Guid workflowInstanceIdValue;
        private IComparable queueNameValue;

        internal FileWatcherSubscription(FileSystemWatcher fileSystemWatcher,
                    Guid workflowInstanceId, IComparable queueName)
        {
            this.fileSystemWatcherValue = fileSystemWatcher;
            this.workflowInstanceIdValue = workflowInstanceId;
            this.queueNameValue = queueName;
        }

        internal FileSystemWatcher FileSystemWatcher
        {
            get { return fileSystemWatcherValue; }
        }

        internal Guid WorkflowInstanceId
        {
            get { return workflowInstanceIdValue; }
        }

        internal IComparable QueueName
        {
            get { return queueNameValue; }
        }
    }
}
