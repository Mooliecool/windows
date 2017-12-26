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

namespace Microsoft.Samples.Workflow.ChangingRules
{
    public sealed partial class DynamicRulesWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.Rules.RuleConditionReference ruleconditionreference1 = new System.Workflow.Activities.Rules.RuleConditionReference();
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.getVPApproval = new System.Workflow.Activities.CodeActivity();
            this.getManagerApproval = new System.Workflow.Activities.CodeActivity();
            this.elseBranch = new System.Workflow.Activities.IfElseBranchActivity();
            this.ifBranch = new System.Workflow.Activities.IfElseBranchActivity();
            this.delayActivity = new System.Workflow.Activities.DelayActivity();
            this.conditionActivity = new System.Workflow.Activities.IfElseActivity();
            this.initAmount = new System.Workflow.Activities.CodeActivity();
            this.mainSequence = new System.Workflow.Activities.SequenceActivity();
            this.whileLoopActivity = new System.Workflow.Activities.WhileActivity();
            // 
            // getVPApproval
            // 
            this.getVPApproval.Name = "getVPApproval";
            this.getVPApproval.ExecuteCode += new System.EventHandler(this.OnVPApproval);
            // 
            // getManagerApproval
            // 
            this.getManagerApproval.Name = "getManagerApproval";
            this.getManagerApproval.ExecuteCode += new System.EventHandler(this.OnManagerApproval);
            // 
            // ifElseBranch2
            // 
            this.elseBranch.Activities.Add(this.getVPApproval);
            this.elseBranch.Name = "ifElseBranch2";
            // 
            // ifElseBranch1
            // 
            this.ifBranch.Activities.Add(this.getManagerApproval);
            ruleconditionreference1.ConditionName = "Check";
            this.ifBranch.Condition = ruleconditionreference1;
            this.ifBranch.Name = "ifElseBranch1";
            // 
            // delay1
            // 
            this.delayActivity.Name = "delay1";
            this.delayActivity.TimeoutDuration = System.TimeSpan.Parse("00:00:02");
            // 
            // ifElse1
            // 
            this.conditionActivity.Activities.Add(this.ifBranch);
            this.conditionActivity.Activities.Add(this.elseBranch);
            this.conditionActivity.Name = "ifElse1";
            // 
            // initAmount
            // 
            this.initAmount.Name = "initAmount";
            this.initAmount.ExecuteCode += new System.EventHandler(this.OnInitAmount);
            // 
            // sequence1
            // 
            this.mainSequence.Activities.Add(this.initAmount);
            this.mainSequence.Activities.Add(this.conditionActivity);
            this.mainSequence.Activities.Add(this.delayActivity);
            this.mainSequence.Name = "sequence1";
            // 
            // while1
            // 
            this.whileLoopActivity.Activities.Add(this.mainSequence);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.Rerun);
            this.whileLoopActivity.Condition = codecondition1;
            this.whileLoopActivity.Name = "while1";
            // 
            // DynamicRulesWorkflow
            // 
            this.Activities.Add(this.whileLoopActivity);
            this.Name = "DynamicRulesWorkflow";
            this.CanModifyActivities = false;

        }

        private DelayActivity delayActivity;
        private IfElseActivity conditionActivity;
        private IfElseBranchActivity ifBranch;
        private CodeActivity getManagerApproval;
        private IfElseBranchActivity elseBranch;
        private CodeActivity getVPApproval;
        private WhileActivity whileLoopActivity;
        private SequenceActivity mainSequence;
        private CodeActivity initAmount;
    }
}
