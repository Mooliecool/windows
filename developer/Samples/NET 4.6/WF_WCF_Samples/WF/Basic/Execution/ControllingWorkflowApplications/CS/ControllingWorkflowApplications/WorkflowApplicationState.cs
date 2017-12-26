//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Hosting;
using System.Collections.ObjectModel;
using System.IO;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    //Wraps a WorkflowApplication, used by WorkflowApplicationManager
    class WorkflowApplicationState
    {
        IWorkflowApplicationHandler applicationHandler;
        WorkflowApplication instance;
        ReadOnlyCollection<BookmarkInfo> availableBookmarks;
        TextWriter instanceWriter;
        bool isLoaded;

        public WorkflowApplicationState(WorkflowApplication instance, IWorkflowApplicationHandler applicationHandler, TextWriter instanceWriter)
        {
            this.instance = instance;
            this.applicationHandler = applicationHandler;
            this.isLoaded = true;

            this.instanceWriter = instanceWriter;

            try
            {
                this.availableBookmarks = this.instance.GetBookmarks();
            }
            catch (WorkflowApplicationException)
            {
                //it is possible the instance completed through some other stimulus (ex. load);
            }

            instance.Aborted = new Action<WorkflowApplicationAbortedEventArgs>(applicationHandler.OnAborted);
            instance.Completed = new Action<WorkflowApplicationCompletedEventArgs>(applicationHandler.OnCompleted);
            instance.Idle = new Action<WorkflowApplicationIdleEventArgs>(OnIdle);
            instance.OnUnhandledException = new Func<WorkflowApplicationUnhandledExceptionEventArgs, UnhandledExceptionAction>(applicationHandler.OnUnhandledException);
            instance.Unloaded = new Action<WorkflowApplicationEventArgs>(OnUnloaded);
        }

        public WorkflowApplication WorkflowApplication
        {
            get
            {
                return this.instance;
            }
        }

        public TextWriter InstanceWriter
        {
            get
            {
                return this.instanceWriter;
            }
        }

        public ReadOnlyCollection<BookmarkInfo> AvailableBookmarks
        {
            get
            {
                return this.availableBookmarks;
            }
        }

        public bool CanResumeBookmarks { get; set; }

        public bool IsLoaded
        {
            get
            {
                return this.isLoaded;
            }
        }

        public WorkflowApplicationInfo AsWorkflowApplicationInfo()
        {
            return new WorkflowApplicationInfo
                {
                    Id = this.WorkflowApplication.Id,
                    AvailableBookmarks = this.AvailableBookmarks,
                    CanResumeBookmarks = this.CanResumeBookmarks,
                    IsLoaded = this.IsLoaded,
                    InstanceWriter = this.InstanceWriter
                };
        }

        public void Close()
        {
            this.availableBookmarks = null;
            this.CanResumeBookmarks = false;
            this.instance.Aborted = null;
            this.instance.Completed = null;
            this.instance.Idle = null;
            this.instance.OnUnhandledException = null;
            this.instance.Unloaded = null;
            this.applicationHandler = null;
            this.isLoaded = false;
        }

        void OnIdle(WorkflowApplicationIdleEventArgs e)
        {
            this.CanResumeBookmarks = true;
            this.availableBookmarks = e.Bookmarks;
            this.applicationHandler.OnIdle(e);
        }

        void OnUnloaded(WorkflowApplicationEventArgs e)
        {
            this.isLoaded = false;
        }
    }
}
