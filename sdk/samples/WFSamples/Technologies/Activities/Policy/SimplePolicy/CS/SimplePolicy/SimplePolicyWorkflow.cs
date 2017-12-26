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
using System.CodeDom;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.Workflow.SimplePolicy
{
    public sealed partial class SimplePolicyWorkflow : SequentialWorkflowActivity
    {
        // Set orderValue and customerType to change the discount calculated by the policy
        private double orderValue = 600.00;
        private CustomerType customerType = CustomerType.Residential;
        private double discount = 0;

        public SimplePolicyWorkflow()
        {
            InitializeComponent();
        }
        
        private void WorkflowCompleted(object sender, EventArgs e)
        {
            Console.WriteLine("Order value = {0:c}", orderValue);
            Console.WriteLine("Customer type = {0}", customerType);
            Console.WriteLine("Calculated discount = {0}%", discount);
        }

        public enum CustomerType
        {
            Empty = 0,
            Residential = 1,
            Business = 2
        }
    }
}
