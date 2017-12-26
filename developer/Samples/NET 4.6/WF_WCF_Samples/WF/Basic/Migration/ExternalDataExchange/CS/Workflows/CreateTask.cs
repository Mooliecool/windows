//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.InteropDemo
{

    public class CreateTask : CallExternalMethodActivity
    {
        public static DependencyProperty AssigneeProperty = 
            DependencyProperty.Register("Assignee", typeof(System.String), typeof(CreateTask));
        public static DependencyProperty TaskIdProperty = 
            DependencyProperty.Register("TaskId", typeof(System.String), typeof(CreateTask));
        public static DependencyProperty TextProperty = 
            DependencyProperty.Register("Text", typeof(System.String), typeof(CreateTask));

        public CreateTask()
        {
            this.InterfaceType = typeof(ITaskService);
            this.MethodName = "CreateTask";
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string Assignee
        {
            get
            {
                return ((string)(base.GetValue(CreateTask.AssigneeProperty)));
            }
            set
            {
                base.SetValue(CreateTask.AssigneeProperty, value);
            }
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string TaskId
        {
            get
            {
                return ((string)(base.GetValue(CreateTask.TaskIdProperty)));
            }
            set
            {
                base.SetValue(CreateTask.TaskIdProperty, value);
            }
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string Text
        {
            get
            {
                return ((string)(base.GetValue(CreateTask.TextProperty)));
            }
            set
            {
                base.SetValue(CreateTask.TextProperty, value);
            }
        }

        protected override void OnMethodInvoking(EventArgs e)
        {
            this.ParameterBindings["taskId"].Value = this.TaskId;
            this.ParameterBindings["assignee"].Value = this.Assignee;
            this.ParameterBindings["text"].Value = this.Text;
        }
    }
}
