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
