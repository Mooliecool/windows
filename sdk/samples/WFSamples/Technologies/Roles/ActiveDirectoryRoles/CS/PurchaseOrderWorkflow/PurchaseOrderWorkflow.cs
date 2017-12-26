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

namespace Microsoft.Samples.Workflow.ActiveDirectoryRoles
{
    public sealed partial class PurchaseOrderWorkflow : SequentialWorkflowActivity
    {
        private WorkflowRoleCollection poInitiatorsValue = new WorkflowRoleCollection();

        public WorkflowRoleCollection POInitiators
        {
            get { return poInitiatorsValue; }
        }
	
        
        public PurchaseOrderWorkflow()
        {
            InitializeComponent();
        }
        
        private void OnPOInitiated(object sender, EventArgs e)
        {
            Console.WriteLine("PO Initiated successfully");
        }

        private void OnSetupRoles(object sender, EventArgs e)
        {
            ActiveDirectoryRole poInitiatorsRole = ActiveDirectoryRoleFactory.CreateFromAlias("ADGroup");

            // Add the role to the WorkflowRoleCollection representing the POInitiators
            POInitiators.Add(poInitiatorsRole);
        }
    }
}
