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

namespace Microsoft.Samples.Workflow.SequentialWorkflowWithParameters
{
    public sealed partial class SequentialWorkflow : SequentialWorkflowActivity
    {
        private int amountValue;
        private StatusType statusValue;

        // Code condition to evaluate whether to take the first branch of IfElseBranch Activity
        private void IsApproved(object sender, ConditionalEventArgs e)
        {
            e.Result = (Amount < 500);
        }

        // When the order is < 500 it sets the Status parameter value to "Approved"
        private void OnApproved(object sender, EventArgs e)
        {
            Status = StatusType.Approved;
        }

        // When the order is >= 500 it sets the Status parameter value to "Rejected"
        private void OnRejected(object sender, EventArgs e)
        {
            Status = StatusType.Rejected;
        }

        public int Amount
        {
            get
            {
                return this.amountValue;
            }
            set
            {
                this.amountValue = value;
            }
        }

        public StatusType Status
        {
            get
            {
                return this.statusValue;
            }
            set
            {
                this.statusValue = value;
            }
        }

        public SequentialWorkflow()
        {
            InitializeComponent();
        }

        
    }

    public enum StatusType
    { 
        None,
        Approved,
        Rejected
    }
}