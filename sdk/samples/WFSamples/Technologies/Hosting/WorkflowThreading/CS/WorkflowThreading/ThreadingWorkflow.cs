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
using System.Threading;

namespace Microsoft.Samples.Workflow.WorkflowThreading
{
    public sealed partial class ThreadingWorkflow : SequentialWorkflowActivity
    {
        private string branchFlag;

        public ThreadingWorkflow()
        {
            InitializeComponent();

            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "ThreadingWorkflow: Processed Constructor");
        }

        public string BranchFlag
        {
            get
            {
                return this.branchFlag;
            }
            set
            {
                this.branchFlag = value;
            }
        }

        private void OnCodeActivity1ExecuteCode(object sender, EventArgs e)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "CodeActivity1: Processed ExecuteCode Event");
        }

        private void OnCodeActivity2ExecuteCode(object sender, EventArgs e)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "CodeActivity2: Processed ExecuteCode Event");
        }

        private void OnCodeActivity3ExecuteCode(object sender, EventArgs e)
        {
            ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "CodeActivity3: Processed ExecuteCode Event");
        }

        private void IfElseBranchActivityCodeCondition(object sender, ConditionalEventArgs e)
        {
            e.Result = !this.BranchFlag.Equals("Delay", StringComparison.OrdinalIgnoreCase);
        }
    }
}
