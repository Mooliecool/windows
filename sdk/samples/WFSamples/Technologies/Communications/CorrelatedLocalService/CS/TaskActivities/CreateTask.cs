//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.ComponentModel.Compiler;

namespace Microsoft.Samples.Workflow.CorrelatedLocalService
{
    [ToolboxItemAttribute(typeof(ActivityToolboxItem))]
    public partial class CreateTask : System.Workflow.Activities.CallExternalMethodActivity
    {
        // Properties on the task
        public static DependencyProperty AssigneeProperty = DependencyProperty.Register("Assignee", typeof(System.String), typeof(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask));
        public static DependencyProperty TaskIdProperty = DependencyProperty.Register("TaskId", typeof(System.String), typeof(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask));
        public static DependencyProperty TextProperty = DependencyProperty.Register("Text", typeof(System.String), typeof(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask));

        private void InitializeComponent()
        {

        }
    
        public CreateTask()
        {
            this.InterfaceType = typeof(Microsoft.Samples.Workflow.CorrelatedLocalService.ITaskService);
            this.MethodName = "CreateTask";
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string Assignee
        {
            get
            {
                return ((string)(base.GetValue(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask.AssigneeProperty)));
            }
            set
            {
                base.SetValue(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask.AssigneeProperty, value);
            }
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string TaskId
        {
            get
            {
                return ((string)(base.GetValue(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask.TaskIdProperty)));
            }
            set
            {
                base.SetValue(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask.TaskIdProperty, value);
            }
        }

        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public string Text
        {
            get
            {
                return ((string)(base.GetValue(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask.TextProperty)));
            }
            set
            {
                base.SetValue(Microsoft.Samples.Workflow.CorrelatedLocalService.CreateTask.TextProperty, value);
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
