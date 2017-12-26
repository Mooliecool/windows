//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Workflow.Activities;

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
