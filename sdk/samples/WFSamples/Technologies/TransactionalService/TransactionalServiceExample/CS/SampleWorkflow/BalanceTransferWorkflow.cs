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

using System.Collections.Generic;

namespace Microsoft.Samples.Workflow.TransactionalServiceSample
{
    public sealed partial class BalanceTransferWorkflow : SequentialWorkflowActivity
    {
        private int transferAmountValue;

        public int TransferAmount
        {
            get { return transferAmountValue; }
            set { transferAmountValue = value; }
        }

        private IList<Int32> accountBalancesValue;

        public IList<Int32> AccountBalances
        {
            get { return accountBalancesValue; }
            set { accountBalancesValue = value; }
        }

        private int accountNumberValue = 1;

        public int AccountNumber
        {
            get { return accountNumberValue; }
            set { accountNumberValue = value; }
        }

        private Exception workflowExceptionValue = new Exception();

        public Exception WorkflowException
        {
            get { return workflowExceptionValue; }
            set { workflowExceptionValue = value; }
        }
	

        public BalanceTransferWorkflow()
        {
            AccountBalances = new Int32[] { 0, 0 };
            this.CanModifyActivities = true;
            InitializeComponent();
            this.CanModifyActivities = false;
        }
        private void onBefore_CreditAmount(object sender, EventArgs e)
        {
            Console.WriteLine("Crediting the Checking amount in account number {0}",AccountNumber);
            this.creditAmount1.Amount = TransferAmount;
        }
        private void onBefore_DebitAmount(object sender, EventArgs e)
        {
             Console.WriteLine("Debiting the Savings amount in account number {0}",AccountNumber);
             this.debitAmount1.Amount = TransferAmount;
        }
        private void onBeforeTransactionInquiry(object sender, EventArgs e)
        {
             Console.WriteLine("Querying account number {0} to find the balance before the transaction",AccountNumber);
        }

        private void onAfterTransactionInquiry(object sender, EventArgs e)
        {
             Console.WriteLine("Querying account number {0} to find the balance after the transaction",AccountNumber);
        }

        private void onDisplayAccountBalances(object sender, EventArgs e)
        {
            Console.WriteLine("The account balances for account number {0} are:  Checking : {1:c} , Savings : {2:c}",
                 AccountNumber, AccountBalances[0], AccountBalances[1]);
        }

        private void onCodeNotify_ExecuteCode(object sender, EventArgs e)
        {
            WorkflowException = faultHandlerActivity1.Fault;
             Console.WriteLine("The transaction failed due to an exception aborting the transaction.");
             Console.WriteLine(WorkflowException.Message);
        }
    }

}
