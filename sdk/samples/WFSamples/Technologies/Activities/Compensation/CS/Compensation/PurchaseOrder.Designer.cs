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

namespace Microsoft.Samples.Workflow.Compensation
{
    public sealed partial class PurchaseOrder : SequentialWorkflowActivity
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.CompensateRefund = new System.Workflow.ComponentModel.CompensateActivity();
            this.DiscontinuedProduct = new System.Workflow.Activities.CodeActivity();
            this.NoProductFault = new System.Workflow.ComponentModel.ThrowActivity();
            this.Ship = new System.Workflow.Activities.CodeActivity();
            this.Refund = new System.Workflow.Activities.CodeActivity();
            this.NoProductFaultHandler = new System.Workflow.ComponentModel.FaultHandlerActivity();
            this.Discontinued = new System.Workflow.Activities.IfElseBranchActivity();
            this.Available = new System.Workflow.Activities.IfElseBranchActivity();
            this.CompensateOrder = new System.Workflow.ComponentModel.CompensationHandlerActivity();
            this.WithdrawFunds = new System.Workflow.Activities.CodeActivity();
            this.FaultsHandling = new System.Workflow.ComponentModel.FaultHandlersActivity();
            this.ProductCheck = new System.Workflow.Activities.IfElseActivity();
            this.OrderScope = new System.Workflow.ComponentModel.CompensatableTransactionScopeActivity();
            this.ReceiverOrder = new System.Workflow.Activities.CodeActivity();
            // 
            // CompensateRefund
            // 
            this.CompensateRefund.Name = "CompensateRefund";
            this.CompensateRefund.TargetActivityName = "OrderScope";
            // 
            // DiscontinuedProduct
            // 
            this.DiscontinuedProduct.Name = "DiscontinuedProduct";
            this.DiscontinuedProduct.ExecuteCode += new System.EventHandler(this.DiscontinuedProduct_ExecuteCode);
            activitybind1.Name = "PurchaseOrder";
            activitybind1.Path = "discontinuedProductException1";
            // 
            // NoProductFault
            // 
            this.NoProductFault.FaultType = typeof(Microsoft.Samples.Workflow.Compensation.DiscontinuedProductException);
            this.NoProductFault.Name = "NoProductFault";
            this.NoProductFault.SetBinding(System.Workflow.ComponentModel.ThrowActivity.FaultProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            // 
            // Ship
            // 
            this.Ship.Name = "Ship";
            this.Ship.ExecuteCode += new System.EventHandler(this.ShipHandler);
            // 
            // Refund
            // 
            this.Refund.Name = "Refund";
            this.Refund.ExecuteCode += new System.EventHandler(this.RefundHandler);
            // 
            // NoProductFaultHandler
            // 
            this.NoProductFaultHandler.Activities.Add(this.DiscontinuedProduct);
            this.NoProductFaultHandler.Activities.Add(this.CompensateRefund);
            this.NoProductFaultHandler.FaultType = typeof(Microsoft.Samples.Workflow.Compensation.DiscontinuedProductException);
            this.NoProductFaultHandler.Name = "NoProductFaultHandler";
            // 
            // Discontinued
            // 
            this.Discontinued.Activities.Add(this.NoProductFault);
            this.Discontinued.Name = "Discontinued";
            // 
            // Available
            // 
            this.Available.Activities.Add(this.Ship);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.ProductCheckHandler);
            this.Available.Condition = codecondition1;
            this.Available.Name = "Available";
            // 
            // CompensateOrder
            // 
            this.CompensateOrder.Activities.Add(this.Refund);
            this.CompensateOrder.Name = "CompensateOrder";
            // 
            // WithdrawFunds
            // 
            this.WithdrawFunds.Name = "WithdrawFunds";
            this.WithdrawFunds.ExecuteCode += new System.EventHandler(this.WithdrawFundsHandler);
            // 
            // FaultsHandling
            // 
            this.FaultsHandling.Activities.Add(this.NoProductFaultHandler);
            this.FaultsHandling.Name = "FaultsHandling";
            // 
            // ProductCheck
            // 
            this.ProductCheck.Activities.Add(this.Available);
            this.ProductCheck.Activities.Add(this.Discontinued);
            this.ProductCheck.Name = "ProductCheck";
            // 
            // OrderScope
            // 
            this.OrderScope.Activities.Add(this.WithdrawFunds);
            this.OrderScope.Activities.Add(this.CompensateOrder);
            this.OrderScope.Name = "OrderScope";
            this.OrderScope.TransactionOptions.IsolationLevel = System.Transactions.IsolationLevel.Serializable;
            // 
            // ReceiverOrder
            // 
            this.ReceiverOrder.Name = "ReceiverOrder";
            this.ReceiverOrder.ExecuteCode += new System.EventHandler(this.ReceiveOrderHandler);
            // 
            // PurchaseOrder
            // 
            this.Activities.Add(this.ReceiverOrder);
            this.Activities.Add(this.OrderScope);
            this.Activities.Add(this.ProductCheck);
            this.Activities.Add(this.FaultsHandling);
            this.Name = "PurchaseOrder";
            this.CanModifyActivities = false;

        }
        #endregion

        private CompensatableTransactionScopeActivity OrderScope;
        private CompensationHandlerActivity CompensateOrder;
        private CodeActivity Refund;
        private IfElseActivity ProductCheck;
        private IfElseBranchActivity Available;
        private IfElseBranchActivity Discontinued;
        private CodeActivity Ship;
        private CodeActivity WithdrawFunds;
        private CodeActivity ReceiverOrder;
        private FaultHandlerActivity NoProductFaultHandler;
        private ThrowActivity NoProductFault;
        private FaultHandlersActivity FaultsHandling;
        private CompensateActivity CompensateRefund;
        private CodeActivity DiscontinuedProduct;
        
    }
}
