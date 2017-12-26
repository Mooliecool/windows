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
using System.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.FileWatcher
{
    public sealed partial class FileWatcherWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.Event1 = new Microsoft.Samples.Workflow.FileWatcher.FileSystemEvent();
            this.listen1 = new System.Workflow.Activities.ListenActivity();
            this.Event3 = new Microsoft.Samples.Workflow.FileWatcher.FileSystemEvent();
            this.eventDriven1 = new System.Workflow.Activities.EventDrivenActivity();
            this.eventDriven2 = new System.Workflow.Activities.EventDrivenActivity();
            this.Event2 = new Microsoft.Samples.Workflow.FileWatcher.FileSystemEvent();
            this.Delay1 = new System.Workflow.Activities.DelayActivity();
            // 
            // Event1
            // 
            this.Event1.IncludeSubdirectories = false;
            this.Event1.Name = "Event1";
            this.Event1.Path = "C:\\temp";
            this.Event1.FileWatcherBefore += new System.EventHandler<System.EventArgs>(this.BeforeFileSystemEvent);
            this.Event1.FileWatcherEventHandler += new System.EventHandler<Microsoft.Samples.Workflow.FileWatcher.FileWatcherEventArgs>(this.AfterFileSystemEvent);
            // 
            // listen1
            // 
            this.listen1.Activities.Add(this.eventDriven1);
            this.listen1.Activities.Add(this.eventDriven2);
            this.listen1.Name = "listen1";
            // 
            // Event3
            // 
            this.Event3.Name = "Event3";
            this.Event3.Path = "C:\\temp";
            this.Event3.FileWatcherBefore += new System.EventHandler<System.EventArgs>(this.BeforeFileSystemEvent);
            this.Event3.FileWatcherEventHandler += new System.EventHandler<Microsoft.Samples.Workflow.FileWatcher.FileWatcherEventArgs>(this.AfterFileSystemEvent);
            // 
            // eventDriven1
            // 
            this.eventDriven1.Activities.Add(this.Event2);
            this.eventDriven1.Name = "eventDriven1";
            // 
            // eventDriven2
            // 
            this.eventDriven2.Activities.Add(this.Delay1);
            this.eventDriven2.Name = "eventDriven2";
            // 
            // Event2
            // 
            this.Event2.Name = "Event2";
            this.Event2.Path = "C:\\temp2";
            this.Event2.FileWatcherBefore += new System.EventHandler<System.EventArgs>(this.BeforeFileSystemEvent);
            this.Event2.FileWatcherEventHandler += new System.EventHandler<Microsoft.Samples.Workflow.FileWatcher.FileWatcherEventArgs>(this.AfterFileSystemEvent);
            // 
            // Delay1
            // 
            this.Delay1.Name = "Delay1";
            this.Delay1.TimeoutDuration = System.TimeSpan.Parse("00:00:20");
            // 
            // FileWatcherWorkflow
            // 
            this.Activities.Add(this.Event1);
            this.Activities.Add(this.listen1);
            this.Activities.Add(this.Event3);
            this.Name = "FileWatcherWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private ListenActivity listen1;
        private EventDrivenActivity eventDriven1;
        private FileSystemEvent Event2;
        private EventDrivenActivity eventDriven2;
        private DelayActivity Delay1;
        private FileSystemEvent Event3;
        private FileSystemEvent Event1;



    }
}
