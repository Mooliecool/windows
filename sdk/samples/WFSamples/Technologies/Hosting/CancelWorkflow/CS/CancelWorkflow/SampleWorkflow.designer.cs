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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.CancelWorkflow
{
    public partial class SampleWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            CanModifyActivities = true;
            this.submitExpense = new CallExternalMethodActivity();
            this.expenseApproval = new HandleExternalEventActivity();

            this.submitExpense.Name = "submitExpense";
            this.submitExpense.InterfaceType = typeof(IExpenseReportService);
            this.submitExpense.MethodName = "SubmitExpense";
            this.submitExpense.MethodInvoking += new EventHandler(submitExpense_MethodInvoking);

            this.expenseApproval.Name = "expenseApproval";
            this.expenseApproval.InterfaceType = typeof(IExpenseReportService);
            this.expenseApproval.EventName = "ExpenseApproval";
            this.expenseApproval.Executing += new EventHandler<ActivityExecutionStatusChangedEventArgs>(expenseApproval_Executing);

            this.Activities.Add(this.submitExpense);
            this.Activities.Add(this.expenseApproval);
            this.Name = "SampleWorkflow";
            CanModifyActivities = false;
        }
        private CallExternalMethodActivity submitExpense;
        private HandleExternalEventActivity expenseApproval;
    }
}
