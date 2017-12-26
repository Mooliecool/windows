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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleInMemorySample
{
    public partial class SequentialWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.ifElseActivity = new System.Workflow.Activities.IfElseActivity();
            this.approveIfElseBranch = new System.Workflow.Activities.IfElseBranchActivity();
            this.rejecteIfElseBranch = new System.Workflow.Activities.IfElseBranchActivity();
            this.Approve = new System.Workflow.Activities.CodeActivity();
            this.Reject = new System.Workflow.Activities.CodeActivity();
            // 
            // ifElseActivity
            // 
            this.ifElseActivity.Activities.Add(this.approveIfElseBranch);
            this.ifElseActivity.Activities.Add(this.rejecteIfElseBranch);
            this.ifElseActivity.Name = "ifElseActivity";
            // 
            // approveIfElseBranch
            // 
            this.approveIfElseBranch.Activities.Add(this.Approve);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.IsApproved);
            this.approveIfElseBranch.Condition = codecondition1;
            this.approveIfElseBranch.Name = "approveIfElseBranch";
            // 
            // rejecteIfElseBranch
            // 
            this.rejecteIfElseBranch.Activities.Add(this.Reject);
            this.rejecteIfElseBranch.Name = "rejecteIfElseBranch";
            // 
            // Approve
            // 
            this.Approve.Name = "Approve";
            this.Approve.ExecuteCode += new System.EventHandler(this.OnApproved);
            // 
            // Reject
            // 
            this.Reject.Name = "Reject";
            this.Reject.ExecuteCode += new System.EventHandler(this.OnRejected);
            // 
            // SequentialWorkflow
            // 
            this.Activities.Add(this.ifElseActivity);
            this.Name = "SequentialWorkflow";
            this.CanModifyActivities = false;
        }

        private IfElseActivity ifElseActivity;
        private IfElseBranchActivity approveIfElseBranch;
        private IfElseBranchActivity rejecteIfElseBranch;
        private CodeActivity Approve;
        private CodeActivity Reject;
        private int amountValue;
        private StatusType statusValue;
    }
}
