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

namespace Microsoft.Samples.Workflow.DynamicUpdateFromWorkflow
{
    public partial class DynamicUpdateWorkflow : SequentialWorkflowActivity
    {
        
        private int purchaseOrderAmount;
        
        private Int16 approvedPurchaseOrderAmount = 1000;

        public int Amount
        {
            get
            {
                return this.purchaseOrderAmount;
            }
            set
            {
                this.purchaseOrderAmount = value;
            }
        }

        public DynamicUpdateWorkflow()
        {
            this.InitializeComponent();
        }

        

        private void OnPORequest(object sender, EventArgs e)
        {
            Console.WriteLine(string.Format("  PO Request Amount of {0:c}", Amount));
        }

        private void OnPOCreated(object sender, EventArgs e)
        {
            Console.WriteLine("  PO Created");
        }

        private void OnCreditCheckDelay(object sender, EventArgs e)
        {
            Console.WriteLine("  Background check delay");
        }

        private void AddApprovalStepCondition(object sender, ConditionalEventArgs e)
        {
            e.Result = (Amount >= this.approvedPurchaseOrderAmount);
        }

        private void OnAddApprovalStep(object sender, EventArgs e)
        {
            InvokeWorkflowActivity invokeApprovalStepWorkflow = new InvokeWorkflowActivity();

            //
            // use WorkflowChanges class to author dynamic change
            //
            WorkflowChanges changes = new WorkflowChanges(this);
            //
            // setup to invoke ApprovalStepWorkflow type
            Type type = typeof(ApprovalStepWorkflow);
            invokeApprovalStepWorkflow.Name = "AddApprovalStepWorkflow";
            invokeApprovalStepWorkflow.TargetWorkflow = type;
            //
            // insert invokeApprovalStepWorkflow in ifElseApproval transient activity collection
            //
            CompositeActivity checkApproval = changes.TransientWorkflow.Activities["CheckApproval"] as CompositeActivity;
            CompositeActivity approvedBranch = checkApproval.Activities["Approved"] as CompositeActivity;
            approvedBranch.Activities.Add(invokeApprovalStepWorkflow);
            //
            // apply transient changes to instance
            //
            this.ApplyWorkflowChanges(changes);
            Console.WriteLine("  Added an InvokeWorkflow activity within the workflow to approve the PO");
         }
    }
}