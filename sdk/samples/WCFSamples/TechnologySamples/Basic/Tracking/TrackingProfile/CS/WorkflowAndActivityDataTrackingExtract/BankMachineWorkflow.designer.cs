//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.Workflow.Activities;

namespace Microsoft.Workflow.Samples
{
	partial class BankMachineWorkflow
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
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            System.Workflow.Activities.CodeCondition codecondition2 = new System.Workflow.Activities.CodeCondition();
            System.Workflow.Activities.CodeCondition codecondition3 = new System.Workflow.Activities.CodeCondition();
            this.withdrawalActivity = new System.Workflow.Activities.CodeActivity();
            this.depositActivity = new System.Workflow.Activities.CodeActivity();
            this.withdrawalIfElseBranchActivity2 = new System.Workflow.Activities.IfElseBranchActivity();
            this.depositIfElseBranchActivity = new System.Workflow.Activities.IfElseBranchActivity();
            this.depositOrWithdrawalIfElseActivity = new System.Workflow.Activities.IfElseActivity();
            this.userInputActivity = new System.Workflow.Activities.CodeActivity();
            this.bankMachineSequenceActivity = new System.Workflow.Activities.SequenceActivity();
            this.serviceChargeActivity = new Microsoft.Samples.Workflow.ServiceChargeActivityLibrary.ServiceCharge();
            this.whileNotExitActivity = new System.Workflow.Activities.WhileActivity();
            // 
            // withdrawalActivity
            // 
            this.withdrawalActivity.Name = "withdrawalActivity";
            this.withdrawalActivity.ExecuteCode += new System.EventHandler(this.WithdrawalExecuteCode);
            // 
            // depositActivity
            // 
            this.depositActivity.Name = "depositActivity";
            this.depositActivity.ExecuteCode += new System.EventHandler(this.DepositExecuteCode);
            // 
            // withdrawalIfElseBranchActivity2
            // 
            this.withdrawalIfElseBranchActivity2.Activities.Add(this.withdrawalActivity);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.CheckWithdrawal);
            this.withdrawalIfElseBranchActivity2.Condition = codecondition1;
            this.withdrawalIfElseBranchActivity2.Name = "withdrawalIfElseBranchActivity2";
            // 
            // depositIfElseBranchActivity
            // 
            this.depositIfElseBranchActivity.Activities.Add(this.depositActivity);
            codecondition2.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.CheckDeposit);
            this.depositIfElseBranchActivity.Condition = codecondition2;
            this.depositIfElseBranchActivity.Name = "depositIfElseBranchActivity";
            // 
            // depositOrWithdrawalIfElseActivity
            // 
            this.depositOrWithdrawalIfElseActivity.Activities.Add(this.depositIfElseBranchActivity);
            this.depositOrWithdrawalIfElseActivity.Activities.Add(this.withdrawalIfElseBranchActivity2);
            this.depositOrWithdrawalIfElseActivity.Name = "depositOrWithdrawalIfElseActivity";
            // 
            // userInputActivity
            // 
            this.userInputActivity.Name = "userInputActivity";
            this.userInputActivity.ExecuteCode += new System.EventHandler(this.UserInputExecuteCode);
            // 
            // bankMachineSequenceActivity
            // 
            this.bankMachineSequenceActivity.Activities.Add(this.userInputActivity);
            this.bankMachineSequenceActivity.Activities.Add(this.depositOrWithdrawalIfElseActivity);
            this.bankMachineSequenceActivity.Name = "bankMachineSequenceActivity";
            // 
            // serviceChargeActivity
            // 
            this.serviceChargeActivity.Fee = 2.41;
            this.serviceChargeActivity.Name = "serviceChargeActivity";
            // 
            // whileNotExitActivity
            // 
            this.whileNotExitActivity.Activities.Add(this.bankMachineSequenceActivity);
            codecondition3.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.CheckOption);
            this.whileNotExitActivity.Condition = codecondition3;
            this.whileNotExitActivity.Name = "whileNotExitActivity";
            // 
            // BankMachineWorkflow
            // 
            this.Activities.Add(this.whileNotExitActivity);
            this.Activities.Add(this.serviceChargeActivity);
            this.Name = "BankMachineWorkflow";
            this.CanModifyActivities = false;

		}

		#endregion

        private Microsoft.Samples.Workflow.ServiceChargeActivityLibrary.ServiceCharge serviceChargeActivity;
        private SequenceActivity bankMachineSequenceActivity;
        private CodeActivity withdrawalActivity;
        private CodeActivity depositActivity;
        private IfElseBranchActivity withdrawalIfElseBranchActivity2;
        private IfElseBranchActivity depositIfElseBranchActivity;
        private IfElseActivity depositOrWithdrawalIfElseActivity;
        private WhileActivity whileNotExitActivity;
        private CodeActivity userInputActivity;


















    }
}
