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

namespace Microsoft.Samples.Workflow.DynamicUpdateFromHost
{
    public partial class DynamicUpdateWorkflow : System.Workflow.Activities.SequentialWorkflowActivity
    {
        private int amount;

        public int Amount
        {
            get
            {
                return this.amount;
            }

            set
            {
                this.amount = value;
            }
        }

        public DynamicUpdateWorkflow()
        {
            this.InitializeComponent();
        }

        private void OnPORequest(object sender, EventArgs e)
        {
            Console.WriteLine("  PO Request Amount of {0:c}", this.Amount);
        }

        private void OnPOCreated(object sender, EventArgs e)
        {
            Console.WriteLine("  Processing PO");
        }

        private void OnCreditCheckDelay(object sender, EventArgs e)
        {
            Console.WriteLine("  Background check");
        }
    }
}