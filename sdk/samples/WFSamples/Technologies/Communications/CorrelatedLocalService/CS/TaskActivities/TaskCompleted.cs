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

namespace Microsoft.Samples.Workflow.CorrelatedLocalService
{
    [ToolboxItemAttribute(typeof(ActivityToolboxItem))]
    public partial class TaskCompleted : System.Workflow.Activities.HandleExternalEventActivity
    {
        // properties
        public static DependencyProperty SenderProperty = System.Workflow.ComponentModel.DependencyProperty.Register("Sender", typeof(Object), typeof(TaskCompleted));
        public static DependencyProperty EProperty = System.Workflow.ComponentModel.DependencyProperty.Register("E", typeof(TaskEventArgs), typeof(TaskCompleted));

        public TaskCompleted()
        {
            this.EventName = "TaskCompleted";
            this.InterfaceType = typeof(Microsoft.Samples.Workflow.CorrelatedLocalService.ITaskService);
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


        public static DependencyProperty EventArgsProperty = System.Workflow.ComponentModel.DependencyProperty.Register("EventArgs", typeof(TaskEventArgs), typeof(TaskCompleted));


        [DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)]
        [BrowsableAttribute(true)]
        public Microsoft.Samples.Workflow.CorrelatedLocalService.TaskEventArgs EventArgs
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
