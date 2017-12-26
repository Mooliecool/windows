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
    [Serializable]
    public class FileWatcherEventArgs : EventArgs
    {
        private WatcherChangeTypes changeType;
        private string fullPath;
        private string name;

        internal FileWatcherEventArgs(FileSystemEventArgs fileSystemEventArgs)
        {
            this.changeType = fileSystemEventArgs.ChangeType;
            this.fullPath = fileSystemEventArgs.FullPath;
            this.name = fileSystemEventArgs.Name;
        }

        public WatcherChangeTypes ChangeType
        {
            get { return changeType; }
        }

        public string FullPath
        {
            get { return fullPath; }
        }

        public string Name
        {
            get { return name; }
        }
    }
}