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
using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.FileWatcher
{
    [ToolboxItemAttribute(typeof(ActivityToolboxItem))]
    [DefaultProperty("Path")]
    public class FileSystemEvent : Activity, IEventActivity, IActivityEventListener<QueueEventArgs>
    {
        #region Default values

        public const string FilterDefaultValue = "*.*";
        
        public const NotifyFilters NotifyFilterDefaultValue =
            System.IO.NotifyFilters.LastWrite |
            System.IO.NotifyFilters.FileName |
            System.IO.NotifyFilters.DirectoryName;
        
        public const bool IncludeSubdirectoriesDefaultValue = true;
        
        #endregion

        #region Private data

        private Guid subscriptionId = Guid.Empty;
        private IComparable queueName;

        #endregion
        
        #region Dependency Properties

        public static DependencyProperty PathProperty = DependencyProperty.Register("Path", typeof(string), typeof(FileSystemEvent));
        public static DependencyProperty FilterProperty = DependencyProperty.Register("Filter", typeof(string), typeof(FileSystemEvent), new PropertyMetadata(FileSystemEvent.FilterDefaultValue));
        public static DependencyProperty NotifyFilterProperty = DependencyProperty.Register("NotifyFilter", typeof(System.IO.NotifyFilters), typeof(FileSystemEvent), new PropertyMetadata(FileSystemEvent.NotifyFilterDefaultValue));
        public static DependencyProperty IncludeSubdirectoriesProperty = DependencyProperty.Register("IncludeSubdirectories", typeof(bool), typeof(FileSystemEvent), new PropertyMetadata(FileSystemEvent.IncludeSubdirectoriesDefaultValue));

        public static DependencyProperty FileWatcherBeforeEvent = DependencyProperty.Register("FileWatcherBefore", typeof(EventHandler<EventArgs>), typeof(FileSystemEvent));
        public static DependencyProperty FileWatcherEventHandlerEvent = DependencyProperty.Register("FileWatcherEventHandler", typeof(EventHandler<FileWatcherEventArgs>), typeof(FileSystemEvent));
        
        #endregion

        #region Activity Properties

        // Required by IEventActivity
        [Browsable(false)]
        public IComparable QueueName
        {
            get { return this.queueName; }
        }
        
        [Category("File System Watcher")]
        [DefaultValue(FileSystemEvent.FilterDefaultValue)]
        public string Filter
        {
            get
            {
                return ((string)(base.GetValue(FileSystemEvent.FilterProperty)));
            }
            set
            {
                base.SetValue(FileSystemEvent.FilterProperty, value);
            }
        }

        [Category("File System Watcher")]
        [TypeConverterAttribute("System.Diagnostics.Design.StringValueConverter, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a")]
        [EditorAttribute("System.Diagnostics.Design.FSWPathEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", "System.Drawing.Design.UITypeEditor, System.Drawing, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a")]
        public string Path
        {
            get
            {
                return ((string)(base.GetValue(FileSystemEvent.PathProperty)));
            }
            set
            {
                base.SetValue(FileSystemEvent.PathProperty, value);
            }
        }

        [Category("File System Watcher")]
        [DefaultValue(FileSystemEvent.IncludeSubdirectoriesDefaultValue)]
        public bool IncludeSubdirectories
        {
            get
            {
                return ((bool)(base.GetValue(FileSystemEvent.IncludeSubdirectoriesProperty)));
            }
            set
            {
                base.SetValue(FileSystemEvent.IncludeSubdirectoriesProperty, value);
            }
        }

        [Category("File System Watcher")]
        [DefaultValue(FileSystemEvent.NotifyFilterDefaultValue)]
        public System.IO.NotifyFilters NotifyFilter
        {
            get
            {
                return ((System.IO.NotifyFilters)(base.GetValue(FileSystemEvent.NotifyFilterProperty)));
            }
            set
            {
                base.SetValue(FileSystemEvent.NotifyFilterProperty, value);
            }
        }

        [Description("This event is raised when the FileSystemEvent activity begins executing.")]
        [Browsable(true)]
        [Category("Handlers")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public event EventHandler<EventArgs> FileWatcherBefore
        {
            add
            {
                base.AddHandler(FileSystemEvent.FileWatcherBeforeEvent, value);
            }
            remove
            {
                base.RemoveHandler(FileSystemEvent.FileWatcherBeforeEvent, value);
            }
        }
        
        [Description("This event is raised when a file system event has occurred.")]
        [Browsable(true)]
        [Category("Handlers")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public event EventHandler<FileWatcherEventArgs> FileWatcherEventHandler
        {
            add
            {
                base.AddHandler(FileSystemEvent.FileWatcherEventHandlerEvent, value);
            }
            remove
            {
                base.RemoveHandler(FileSystemEvent.FileWatcherEventHandlerEvent, value);
            }
        }
        #endregion

        #region Activity Execution Logic

        protected override void Initialize(IServiceProvider provider)
        {
            Console.WriteLine("Initialize");

            this.queueName = this.Name + Guid.NewGuid().ToString();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            Console.WriteLine("Execute");

            base.RaiseGenericEvent<EventArgs>(FileSystemEvent.FileWatcherBeforeEvent, this, EventArgs.Empty);
            
            if (this.ProcessQueueItem(context))
            {
                return ActivityExecutionStatus.Closed;
            }

            this.DoSubscribe(context, this);
            return ActivityExecutionStatus.Executing;
        }

        void IEventActivity.Subscribe(ActivityExecutionContext parentContext, IActivityEventListener<QueueEventArgs> parentEventHandler)
        {
            Console.WriteLine("Subscribe");
            DoSubscribe(parentContext, parentEventHandler);
        }

        protected override ActivityExecutionStatus Cancel(ActivityExecutionContext context)
        {
            Console.WriteLine("Cancel");

            DoUnsubscribe(context, this);
            DeleteQueue(context);
            return ActivityExecutionStatus.Closed;
        }

        void IEventActivity.Unsubscribe(ActivityExecutionContext parentContext, IActivityEventListener<QueueEventArgs> parentEventHandler)
        {
            Console.WriteLine("Unsubscribe");
            DoUnsubscribe(parentContext, parentEventHandler);
        }

        private WorkflowQueue CreateQueue(ActivityExecutionContext context)
        {
            Console.WriteLine("CreateQueue");
            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>();

            if (!qService.Exists(this.QueueName))
            {
                qService.CreateWorkflowQueue(this.QueueName, true);
            }

            return qService.GetWorkflowQueue(this.QueueName);
        }

        private void DeleteQueue(ActivityExecutionContext context)
        {
            Console.WriteLine("DeleteQueue");
            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>();
            qService.DeleteWorkflowQueue(this.QueueName);
        }

        private Boolean DoSubscribe(ActivityExecutionContext context, IActivityEventListener<QueueEventArgs> listener)
        {
            WorkflowQueue queue = CreateQueue(context);
            queue.RegisterForQueueItemAvailable(listener);

            FileWatcherService fileService = context.GetService<FileWatcherService>();
            this.subscriptionId = fileService.RegisterListener(this.QueueName, this.Path, this.Filter, this.NotifyFilter, this.IncludeSubdirectories);
            return (subscriptionId != Guid.Empty);
        }
  
        private void DoUnsubscribe(ActivityExecutionContext context, IActivityEventListener<QueueEventArgs> listener)
        {
            if (!this.subscriptionId.Equals(Guid.Empty))
            {
                FileWatcherService fileService = context.GetService<FileWatcherService>();
                fileService.UnregisterListener(this.subscriptionId);
                this.subscriptionId = Guid.Empty;
            }

            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>();
            WorkflowQueue queue = qService.GetWorkflowQueue(this.QueueName);

            queue.UnregisterForQueueItemAvailable(listener);
        }

        void IActivityEventListener<QueueEventArgs>.OnEvent(object sender, QueueEventArgs e)
        {
            Console.WriteLine("OnQueueItemAvailable #1");

            // If activity is not scheduled for execution, do nothing
            if (this.ExecutionStatus == ActivityExecutionStatus.Executing)
            {
                Console.WriteLine("OnQueueItemAvailable #2");

                ActivityExecutionContext context = sender as ActivityExecutionContext;
                if (this.ProcessQueueItem(context))
                {
                    context.CloseActivity();
                }
            }
        }

        private bool ProcessQueueItem(ActivityExecutionContext context)
        {
            WorkflowQueuingService qService = context.GetService<WorkflowQueuingService>();
            if (!qService.Exists(this.QueueName))
            {
                return false;
            }

            WorkflowQueue queue = qService.GetWorkflowQueue(this.QueueName);

            // If the queue has messages, then process the first one
            if (queue.Count == 0)
            {
                return false;
            }

            FileWatcherEventArgs e = (FileWatcherEventArgs) queue.Dequeue();

            // Raise the FileSystemEvent
            base.RaiseGenericEvent<FileWatcherEventArgs>(FileSystemEvent.FileWatcherEventHandlerEvent, this, e);

            DoUnsubscribe(context, this);
            DeleteQueue(context);
            return true;
        }

        #endregion
    }
}