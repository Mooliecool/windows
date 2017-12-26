//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.InteropDemo
{

    public class TaskCompleted : HandleExternalEventActivity
    {
        public static DependencyProperty SenderProperty = 
            DependencyProperty.Register("Sender", typeof(Object), typeof(TaskCompleted));

        public static DependencyProperty EventArgsProperty = 
            DependencyProperty.Register("EventArgs", typeof(TaskEventArgs), typeof(TaskCompleted));

        public TaskCompleted()
        {
            this.EventName = "TaskCompleted";
            this.InterfaceType = typeof(ITaskService);
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public object Sender
        {
            get
            {
                return ((Object)(base.GetValue(TaskCompleted.SenderProperty)));
            }
            set
            {
                base.SetValue(TaskCompleted.SenderProperty, value);
            }
        }


        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public TaskEventArgs EventArgs
        {
            get
            {
                return ((TaskEventArgs)(base.GetValue(TaskCompleted.EventArgsProperty)));
            }
            set
            {
                base.SetValue(TaskCompleted.EventArgsProperty, value);
            }
        }

        protected override void OnInvoked(EventArgs e)
        {
            EventArgs = e as TaskEventArgs;
        }
    }
}
