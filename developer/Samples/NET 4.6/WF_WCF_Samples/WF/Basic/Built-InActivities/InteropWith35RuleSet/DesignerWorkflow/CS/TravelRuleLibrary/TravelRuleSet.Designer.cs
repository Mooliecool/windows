//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Reflection;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.TravelRuleLibrary
{
    public partial class TravelRuleSet
    {
        #region Activity Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.Rules.RuleSetReference rulesetreference1 = new System.Workflow.Activities.Rules.RuleSetReference();
            this.DiscountPolicy = new System.Workflow.Activities.PolicyActivity();
            // 
            // DiscountPolicy
            // 
            this.DiscountPolicy.Name = "DiscountPolicy";
            rulesetreference1.RuleSetName = "Rule Set1";
            this.DiscountPolicy.RuleSetReference = rulesetreference1;
            // 
            // TravelRuleSet
            // 
            this.Activities.Add(this.DiscountPolicy);
            this.Name = "TravelRuleSet";
            this.CanModifyActivities = false;

        }

        #endregion

        private PolicyActivity DiscountPolicy;









    }
}
