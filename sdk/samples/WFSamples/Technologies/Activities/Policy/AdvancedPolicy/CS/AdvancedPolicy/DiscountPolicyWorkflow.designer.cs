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
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.Workflow.AdvancedPolicy
{
    public sealed partial class DiscountPolicyWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.Rules.RuleSetReference rulesetreference1 = new System.Workflow.Activities.Rules.RuleSetReference();
            this.advancedDiscountPolicy = new System.Workflow.Activities.PolicyActivity();
            // 
            // advancedDiscountPolicy
            // 
            this.advancedDiscountPolicy.Name = "advancedDiscountPolicy";
            rulesetreference1.RuleSetName = "DiscountRuleSet";
            this.advancedDiscountPolicy.RuleSetReference = rulesetreference1;
            // 
            // DiscountPolicyWorkflow
            // 
            this.Activities.Add(this.advancedDiscountPolicy);
            this.Name = "DiscountPolicyWorkflow";
            this.Completed += new System.EventHandler(this.WorkflowCompleted);
            this.CanModifyActivities = false;

        }

        private PolicyActivity advancedDiscountPolicy;
    }
}