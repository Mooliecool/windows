//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Workflow.Activities;

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

