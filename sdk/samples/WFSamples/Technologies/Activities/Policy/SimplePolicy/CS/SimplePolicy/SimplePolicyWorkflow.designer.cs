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

namespace Microsoft.Samples.Workflow.SimplePolicy
{
    public sealed partial class SimplePolicyWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.Rules.RuleSetReference rulesetreference1 = new System.Workflow.Activities.Rules.RuleSetReference();
            this.simpleDiscountPolicy = new System.Workflow.Activities.PolicyActivity();
            // 
            // simpleDiscountPolicy
            // 
            this.simpleDiscountPolicy.Name = "simpleDiscountPolicy";
            rulesetreference1.RuleSetName = "DiscountRuleSet";
            this.simpleDiscountPolicy.RuleSetReference = rulesetreference1;
            // 
            // SimplePolicyWorkflow
            // 
            this.Activities.Add(this.simpleDiscountPolicy);
            this.Name = "SimplePolicyWorkflow";
            this.Completed += new System.EventHandler(this.WorkflowCompleted);
            this.CanModifyActivities = false;

        }

        private PolicyActivity simpleDiscountPolicy;
    }
}