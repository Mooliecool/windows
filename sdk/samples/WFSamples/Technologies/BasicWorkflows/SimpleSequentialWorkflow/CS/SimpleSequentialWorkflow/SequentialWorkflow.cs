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
using System.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SimpleSequentialWorkflow
{
    //This sample demonstrates a canonical purchase order workflow that
    // contains a single IfElse activity with two branches.  Each branch
    // contains a single code activity.  Since the condition always
    // evaluates to true the left branch is always taken and the order
    // is approved.
    public sealed partial class SequentialWorkflow : SequentialWorkflowActivity
    {
        public SequentialWorkflow()
        {
            InitializeComponent();
        }

        // The event handler that executes on ExecuteCode event of the ApprovePO activity
        private void OnApproved(object sender, EventArgs e)
        {
            Console.WriteLine("Purchase Order Approved.");
        }

        // The event handler that executes on ExecuteCode event of the RejectPO activity
        private void OnRejected(object sender, EventArgs e)
        {
            Console.WriteLine("Purchase Order Rejected.");
        }

        // Code condition to evaluate whether to take the first branch, YesIfElseBranch
        // Since it always returns true, the first branch is always taken.
        private void IsUnderLimit(object sender, ConditionalEventArgs e)
        {
            e.Result = true;
        }
    }
}
