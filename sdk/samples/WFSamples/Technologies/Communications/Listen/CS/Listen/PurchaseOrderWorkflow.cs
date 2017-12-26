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

namespace Microsoft.Samples.Workflow.Listen
{
    public sealed partial class PurchaseOrderWorkflow : SequentialWorkflowActivity
    {
        public PurchaseOrderWorkflow()
        {
            InitializeComponent();
        }

        // The event handler that executes on Invoked event of the ApprovePO activity
        private void OnApprovePO(object sender, ExternalDataEventArgs e)
        {
            Console.WriteLine("\nPurchase Order Approved");
        }

        // The event handler that executes on MethodInvoking event of the CreatePO activity
        private void OnBeforeCreateOrder(object sender, EventArgs e)
        {
            Console.WriteLine("\nPO Workflow created");
        }

        // The event handler that executes on ExecuteCode event of the Timeout activity
        private void OnTimeout(object sender, EventArgs e)
        {
            Console.WriteLine("\nPurchase Order Workflow timed out");
        }

        // The event handler that executes on Invoked event of the RejectPO activity
        private void OnRejectPO(object sender, ExternalDataEventArgs e)
        {
            Console.WriteLine("\nPurchase Order Rejected");
        }
    }
}
