//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Workflow.Activities;

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
