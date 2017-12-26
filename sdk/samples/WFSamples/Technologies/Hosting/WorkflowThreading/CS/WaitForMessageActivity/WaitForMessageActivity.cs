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
using System.Threading;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;

namespace Microsoft.Samples.Workflow.WorkflowThreading
{
    [ToolboxItem(typeof(ActivityToolboxItem))]
    public partial class WaitForMessageActivity: Activity
    {
        WorkflowQueue workflowQueue;

        public WaitForMessageActivity()
        {
            InitializeComponent();
        }

        protected override void Initialize(IServiceProvider provider)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "WaitForMessageActivity", "WaitForMessageActivity: Processed Initialization");

            WorkflowQueuingService queuingService = (WorkflowQueuingService)provider.GetService(typeof(WorkflowQueuingService));
            this.workflowQueue = queuingService.CreateWorkflowQueue("WaitForMessageActivityQueue", false);
            this.workflowQueue.QueueItemAvailable += this.HandleExternalEvent;
        }

        private void HandleExternalEvent(Object sender, QueueEventArgs args)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "WaitForMessageActivity", "WaitForMessageActivity: Processed External Event");

            object data = this.workflowQueue.Dequeue();

            ActivityExecutionContext context = sender as ActivityExecutionContext;
            context.CloseActivity();
        }

        protected override ActivityExecutionStatus Execute(ActivityExecutionContext context)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "WaitForMessageActivity", "WaitForMessageActivity: Processed Waiting For External Event");

            return ActivityExecutionStatus.Executing;
        }
    }
}
