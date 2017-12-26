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
using System.Workflow.Activities.Rules;
using System.CodeDom;

namespace Microsoft.Samples.Workflow.AdvancedPolicy
{
    public sealed partial class DiscountPolicyWorkflow : SequentialWorkflowActivity
    {
        // set orderValue and customerType to change the discount calculated by the policy
        private double orderValue = 60000.00;
        private CustomerType customerType = CustomerType.Residential;
        private double discount = 0;
        private double calculatedTotal;
        private string error = string.Empty;

        public DiscountPolicyWorkflow()
        {
            InitializeComponent();
        }

        private void WorkflowCompleted(object sender, EventArgs e)
        {
            Console.WriteLine("\r\nOrder value = {0:c}", orderValue);
            Console.WriteLine("Customer type = {0}", customerType);
            Console.WriteLine("Calculated discount = {0}%", discount);
            Console.WriteLine("Calculated total = {0:c}", calculatedTotal);
            if (error.Length > 0)
            {
                Console.WriteLine("Discount policy error: {0}", error);
            }
        }

        #region Helper method to set the total field
        // The RuleWrite attribute indicates that this method updates the calculatedTotal field.
        // The engine uses this information to reevaluate any conditions that use calculatedTotal
        // whenever this method is called
        [RuleWrite("calculatedTotal")]
        public void CalculateTotal(double currentOrderValue, double currentDiscount)
        {
            this.calculatedTotal = currentOrderValue * (1.0 - currentDiscount / 100.0);
        }
        #endregion

        public enum CustomerType
        {
            Empty = 0,
            Residential = 1,
            Business = 2
        }
    }

    
}
