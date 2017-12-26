//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Workflow.Activities;

namespace Microsoft.Samples.InteropDemo
{

    public sealed partial class TaskWorkflow : SequentialWorkflowActivity
    {
        public TaskWorkflow()
        {
            InitializeComponent();
        }

        private void OnTaskCompleted(object sender, EventArgs e)
        {
            TaskEventArgs eventArgs = e as TaskEventArgs;
            Console.WriteLine("task {0} is done", eventArgs.Id);
        }
    }
}
