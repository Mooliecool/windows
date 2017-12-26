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
using System.CodeDom;
using System.ComponentModel;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.Workflow.ChangingRules
{
    public sealed partial class DynamicRulesWorkflow: SequentialWorkflowActivity
    {
        int i = 0;

        private int amountValue;
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

        public DynamicRulesWorkflow()
        {
            InitializeComponent();
        }

        void OnInitAmount(object sender, EventArgs e)
        {
            Console.WriteLine("Loop " + i);
        }

        private void OnManagerApproval(object sender, EventArgs e)
        {
            Console.WriteLine("  Get Manager Approval");
        }

        private void OnVPApproval(object sender, EventArgs e)
        {
            Console.WriteLine("  Get VP Approval");
        }

        private void Rerun(object sender, ConditionalEventArgs e)
        {
            e.Result = (i < 2);
            i++;
        }
    }
}
