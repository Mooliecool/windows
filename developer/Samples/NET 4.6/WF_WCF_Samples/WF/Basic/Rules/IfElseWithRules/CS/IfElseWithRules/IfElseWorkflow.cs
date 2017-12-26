//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.IfElseActivityWithRules
{
    public sealed partial class IfElseWorkflow : SequentialWorkflowActivity
    {
        // This field is populated by the incoming workflow parameter and is used by the condition
        private int orderValue;

        public IfElseWorkflow()
        {
            InitializeComponent();
        }

        private void ManagerApprovalHandler(object sender, EventArgs e)
        {
            Console.WriteLine("Get Manager Approval");
        }

        private void VPApprovalHandler(object sender, EventArgs e)
        {
            Console.WriteLine("Get VP Approval");
        }

        public int OrderValue
        {
            get
            {
                return this.orderValue;
            }
            set
            {
                this.orderValue = value;
            }
        }
    }
}

