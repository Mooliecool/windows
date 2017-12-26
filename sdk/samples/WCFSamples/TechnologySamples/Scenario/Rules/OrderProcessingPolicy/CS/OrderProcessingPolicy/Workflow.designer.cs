//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.Workflow.Activities;

namespace Microsoft.Rules.Samples
{
	partial class Workflow
	{
		#region Designer generated code
		
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
		private void InitializeComponent()
		{
            this.CanModifyActivities = true;
            System.Workflow.Activities.Rules.RuleSetReference rulesetreference1 = new System.Workflow.Activities.Rules.RuleSetReference();
            this.policyActivity1 = new System.Workflow.Activities.PolicyActivity();
            // 
            // policyActivity1
            // 
            this.policyActivity1.Name = "policyActivity1";
            rulesetreference1.RuleSetName = "ValidateOrders";
            this.policyActivity1.RuleSetReference = rulesetreference1;
            // 
            // Workflow
            // 
            this.Activities.Add(this.policyActivity1);
            this.Name = "Workflow";
            this.CanModifyActivities = false;

		}

		#endregion

        private PolicyActivity policyActivity1;






    }
}
