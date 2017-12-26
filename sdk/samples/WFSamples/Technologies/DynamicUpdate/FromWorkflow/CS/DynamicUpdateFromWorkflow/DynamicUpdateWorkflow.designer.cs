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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.DynamicUpdateFromWorkflow
{
    public partial class  DynamicUpdateWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        public void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.PORequest = new System.Workflow.Activities.CodeActivity();
            this.CreditCheckDelay = new System.Workflow.Activities.DelayActivity();
            this.CheckApproval = new System.Workflow.Activities.IfElseActivity();
            this.POCreated = new System.Workflow.Activities.CodeActivity();
            this.Approved = new System.Workflow.Activities.IfElseBranchActivity();
            this.AddApprovalStep = new System.Workflow.Activities.CodeActivity();
            // 
            // PORequest
            // 
            this.PORequest.Name = "PORequest";
            this.PORequest.ExecuteCode += new System.EventHandler(this.OnPORequest);
            // 
            // CreditCheckDelay
            // 
            this.CreditCheckDelay.Name = "CreditCheckDelay";
            this.CreditCheckDelay.TimeoutDuration = System.TimeSpan.Parse("00:00:02");
            this.CreditCheckDelay.InitializeTimeoutDuration += new System.EventHandler(this.OnCreditCheckDelay);
            // 
            // CheckApproval
            // 
            this.CheckApproval.Activities.Add(this.Approved);
            this.CheckApproval.Name = "CheckApproval";
            // 
            // POCreated
            // 
            this.POCreated.Name = "POCreated";
            this.POCreated.ExecuteCode += new System.EventHandler(this.OnPOCreated);
            // 
            // Approved
            // 
            this.Approved.Activities.Add(this.AddApprovalStep);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.AddApprovalStepCondition);
            this.Approved.Condition = codecondition1;
            this.Approved.Name = "Approved";
            // 
            // AddApprovalStep
            // 
            this.AddApprovalStep.Name = "AddApprovalStep";
            this.AddApprovalStep.ExecuteCode += new System.EventHandler(this.OnAddApprovalStep);
            // 
            // DynamicUpdateWorkflow
            // 
            this.Activities.Add(this.PORequest);
            this.Activities.Add(this.CreditCheckDelay);
            this.Activities.Add(this.CheckApproval);
            this.Activities.Add(this.POCreated);
            this.Name = "DynamicUpdateWorkflow";
            this.CanModifyActivities = false;
        }

        private CodeActivity PORequest;
        private CodeActivity POCreated;
        private CodeActivity AddApprovalStep;
        private DelayActivity CreditCheckDelay;
        private IfElseActivity CheckApproval;
        private IfElseBranchActivity Approved;
    }
}
