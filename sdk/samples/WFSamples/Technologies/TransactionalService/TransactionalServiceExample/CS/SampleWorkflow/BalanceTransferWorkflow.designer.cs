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
using System.Workflow.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.TransactionalServiceSample
{
    public sealed partial class BalanceTransferWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.ActivityBind activitybind3 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind4 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind5 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding3 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind6 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding4 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            this.CodeNotify = new System.Workflow.Activities.CodeActivity();
            this.faultHandlerActivity1 = new System.Workflow.ComponentModel.FaultHandlerActivity();

            this.debitAmount1 = new Microsoft.Samples.Workflow.TransactionalServiceSample.DebitAmount();
            this.creditAmount1 = new Microsoft.Samples.Workflow.TransactionalServiceSample.CreditAmount();
            this.faultHandlersActivity1 = new System.Workflow.ComponentModel.FaultHandlersActivity();
            this.CodeDisplayAccountBalances2 = new System.Workflow.Activities.CodeActivity();
            this.CallAfterTransaction = new System.Workflow.Activities.CallExternalMethodActivity();
            this.transactionScope1 = new System.Workflow.ComponentModel.CompensatableTransactionScopeActivity();
            this.CodeDisplayAccountBalances1 = new System.Workflow.Activities.CodeActivity();
            this.CallBeforeTransaction = new System.Workflow.Activities.CallExternalMethodActivity();
            // 
            // CodeNotify
            // 
            this.CodeNotify.Name = "CodeNotify";
            this.CodeNotify.ExecuteCode += new System.EventHandler(this.onCodeNotify_ExecuteCode);
            // 
            // faultHandlerActivity1
            // 
            this.faultHandlerActivity1.Activities.Add(this.CodeNotify);
            this.faultHandlerActivity1.FaultType = typeof(System.Exception);
            this.faultHandlerActivity1.Name = "faultHandlerActivity1";
            // 
            // debitAmount1
            // 
            activitybind1.Name = "BalanceTransferWorkflow";
            activitybind1.Path = "TransferAmount";
            this.debitAmount1.Name = "debitAmount1";
            this.debitAmount1.OnBeforeInvoke += new System.EventHandler(this.onBefore_DebitAmount);
            this.debitAmount1.SetBinding(Microsoft.Samples.Workflow.TransactionalServiceSample.DebitAmount.AmountProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            // 
            // creditAmount1
            // 
            activitybind2.Name = "BalanceTransferWorkflow";
            activitybind2.Path = "TransferAmount";
            this.creditAmount1.Name = "creditAmount1";
            this.creditAmount1.OnBeforeInvoke += new System.EventHandler(this.onBefore_CreditAmount);
            this.creditAmount1.SetBinding(Microsoft.Samples.Workflow.TransactionalServiceSample.CreditAmount.AmountProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            // 
            // faultHandlersActivity1
            // 
            this.faultHandlersActivity1.Activities.Add(this.faultHandlerActivity1);
            this.faultHandlersActivity1.Name = "faultHandlersActivity1";
            // 
            // CodeDisplayAccountBalances2
            // 
            this.CodeDisplayAccountBalances2.Name = "CodeDisplayAccountBalances2";
            this.CodeDisplayAccountBalances2.ExecuteCode += new System.EventHandler(this.onDisplayAccountBalances);
            // 
            // CallAfterTransaction
            // 
            this.CallAfterTransaction.InterfaceType = typeof(Microsoft.Samples.Workflow.TransactionalServiceSample.IQueryAccountService);
            this.CallAfterTransaction.MethodName = "QueryAccount";
            this.CallAfterTransaction.Name = "CallAfterTransaction";
            activitybind3.Name = "BalanceTransferWorkflow";
            activitybind3.Path = "AccountBalances";
            workflowparameterbinding1.ParameterName = "(ReturnValue)";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind3)));
            activitybind4.Name = "BalanceTransferWorkflow";
            activitybind4.Path = "AccountNumber";
            workflowparameterbinding2.ParameterName = "accountNumber";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind4)));
            this.CallAfterTransaction.ParameterBindings.Add(workflowparameterbinding1);
            this.CallAfterTransaction.ParameterBindings.Add(workflowparameterbinding2);
            this.CallAfterTransaction.MethodInvoking += new System.EventHandler(this.onAfterTransactionInquiry);
            // 
            // transactionScope1
            // 
            this.transactionScope1.Activities.Add(this.creditAmount1);
            this.transactionScope1.Activities.Add(this.debitAmount1);
            this.transactionScope1.Name = "transactionScope1";
            this.transactionScope1.TransactionOptions.IsolationLevel = System.Transactions.IsolationLevel.Serializable;
            // 
            // CodeDisplayAccountBalances1
            // 
            this.CodeDisplayAccountBalances1.Name = "CodeDisplayAccountBalances1";
            this.CodeDisplayAccountBalances1.ExecuteCode += new System.EventHandler(this.onDisplayAccountBalances);
            // 
            // CallBeforeTransaction
            // 
            this.CallBeforeTransaction.InterfaceType = typeof(Microsoft.Samples.Workflow.TransactionalServiceSample.IQueryAccountService);
            this.CallBeforeTransaction.MethodName = "QueryAccount";
            this.CallBeforeTransaction.Name = "CallBeforeTransaction";
            activitybind5.Name = "BalanceTransferWorkflow";
            activitybind5.Path = "AccountBalances";
            workflowparameterbinding3.ParameterName = "(ReturnValue)";
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind5)));
            activitybind6.Name = "BalanceTransferWorkflow";
            activitybind6.Path = "AccountNumber";
            workflowparameterbinding4.ParameterName = "accountNumber";
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind6)));
            this.CallBeforeTransaction.ParameterBindings.Add(workflowparameterbinding3);
            this.CallBeforeTransaction.ParameterBindings.Add(workflowparameterbinding4);
            this.CallBeforeTransaction.MethodInvoking += new System.EventHandler(this.onBeforeTransactionInquiry);
            // 
            // BalanceTransferWorkflow
            // 
            this.Activities.Add(this.CallBeforeTransaction);
            this.Activities.Add(this.CodeDisplayAccountBalances1);
            this.Activities.Add(this.transactionScope1);
            this.Activities.Add(this.CallAfterTransaction);
            this.Activities.Add(this.CodeDisplayAccountBalances2);
            this.Activities.Add(this.faultHandlersActivity1);
            this.Name = "BalanceTransferWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private CompensatableTransactionScopeActivity transactionScope1;
        private CreditAmount creditAmount1;
        private DebitAmount debitAmount1;
        private CodeActivity CodeDisplayAccountBalances1;
        private CallExternalMethodActivity CallAfterTransaction;
        private CodeActivity CodeDisplayAccountBalances2;
        private FaultHandlersActivity faultHandlersActivity1;
        private FaultHandlerActivity faultHandlerActivity1;
        private CodeActivity CodeNotify;
        private CallExternalMethodActivity CallBeforeTransaction;



















    }
}
