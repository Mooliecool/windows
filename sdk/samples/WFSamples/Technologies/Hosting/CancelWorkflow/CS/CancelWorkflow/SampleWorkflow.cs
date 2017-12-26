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
    public partial class SampleWorkflow: SequentialWorkflowActivity
    {
        public SampleWorkflow()
        {
            InitializeComponent();
        }

        void submitExpense_MethodInvoking(object sender, EventArgs e)
        {
            Console.WriteLine("Workflow: submits expense report");
        }
        
        void expenseApproval_Executing(object sender, ActivityExecutionStatusChangedEventArgs e)
        {
            Console.WriteLine("Workflow: waiting for approval");
        }
    }
}
