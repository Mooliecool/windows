'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------
Namespace Microsoft.Samples.Workflow.Compensation
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Public Class PurchaseOrder

        'NOTE: The following procedure is required by the Workflow Designer
        'It can be modified using the Workflow Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
            Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
            Me.CompensateRefund = New System.Workflow.ComponentModel.CompensateActivity
            Me.DiscontinuedProduct = New System.Workflow.Activities.CodeActivity
            Me.NoProductFault = New System.Workflow.ComponentModel.ThrowActivity
            Me.Ship = New System.Workflow.Activities.CodeActivity
            Me.Refund = New System.Workflow.Activities.CodeActivity
            Me.NoProductFaultHandler = New System.Workflow.ComponentModel.FaultHandlerActivity
            Me.Discontinued = New System.Workflow.Activities.IfElseBranchActivity
            Me.Available = New System.Workflow.Activities.IfElseBranchActivity
            Me.CompensateOrder = New System.Workflow.ComponentModel.CompensationHandlerActivity
            Me.WithdrawFunds = New System.Workflow.Activities.CodeActivity
            Me.faultHandlersActivity1 = New System.Workflow.ComponentModel.FaultHandlersActivity
            Me.ProductCheck = New System.Workflow.Activities.IfElseActivity
            Me.OrderTransaction = New System.Workflow.ComponentModel.CompensatableTransactionScopeActivity
            Me.ReceiveOrder = New System.Workflow.Activities.CodeActivity
            '
            'CompensateRefund
            '
            Me.CompensateRefund.Name = "CompensateRefund"
            Me.CompensateRefund.TargetActivityName = "OrderTransaction"
            '
            'DiscontinuedProduct
            '
            Me.DiscontinuedProduct.Name = "DiscontinuedProduct"
            AddHandler Me.DiscontinuedProduct.ExecuteCode, AddressOf Me.DiscontinuedProduct_ExecuteCode
            activitybind1.Name = "PurchaseOrder"
            activitybind1.Path = "discontinuedProductException1"
            '
            'NoProductFault
            '
            Me.NoProductFault.FaultType = GetType(Microsoft.Samples.Workflow.Compensation.DiscontinuedProductException)
            Me.NoProductFault.Name = "NoProductFault"
            Me.NoProductFault.SetBinding(System.Workflow.ComponentModel.ThrowActivity.FaultProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
            '
            'Ship
            '
            Me.Ship.Name = "Ship"
            AddHandler Me.Ship.ExecuteCode, AddressOf Me.ShipHandler
            '
            'Refund
            '
            Me.Refund.Name = "Refund"
            AddHandler Me.Refund.ExecuteCode, AddressOf Me.RefundHandler
            '
            'NoProductFaultHandler
            '
            Me.NoProductFaultHandler.Activities.Add(Me.DiscontinuedProduct)
            Me.NoProductFaultHandler.Activities.Add(Me.CompensateRefund)
            Me.NoProductFaultHandler.FaultType = GetType(Microsoft.Samples.Workflow.Compensation.DiscontinuedProductException)
            Me.NoProductFaultHandler.Name = "NoProductFaultHandler"
            '
            'Discontinued
            '
            Me.Discontinued.Activities.Add(Me.NoProductFault)
            Me.Discontinued.Name = "Discontinued"
            '
            'Available
            '
            Me.Available.Activities.Add(Me.Ship)
            AddHandler codecondition1.Condition, AddressOf Me.ProductCheckHandler
            Me.Available.Condition = codecondition1
            Me.Available.Name = "Available"
            '
            'CompensateOrder
            '
            Me.CompensateOrder.Activities.Add(Me.Refund)
            Me.CompensateOrder.Name = "CompensateOrder"
            '
            'WithdrawFunds
            '
            Me.WithdrawFunds.Name = "WithdrawFunds"
            AddHandler Me.WithdrawFunds.ExecuteCode, AddressOf Me.WithdrawFundsHandler
            '
            'faultHandlersActivity1
            '
            Me.faultHandlersActivity1.Activities.Add(Me.NoProductFaultHandler)
            Me.faultHandlersActivity1.Name = "faultHandlersActivity1"
            '
            'ProductCheck
            '
            Me.ProductCheck.Activities.Add(Me.Available)
            Me.ProductCheck.Activities.Add(Me.Discontinued)
            Me.ProductCheck.Name = "ProductCheck"
            '
            'OrderTransaction
            '
            Me.OrderTransaction.Activities.Add(Me.WithdrawFunds)
            Me.OrderTransaction.Activities.Add(Me.CompensateOrder)
            Me.OrderTransaction.Name = "OrderTransaction"
            Me.OrderTransaction.TransactionOptions.IsolationLevel = System.Transactions.IsolationLevel.Serializable
            '
            'ReceiveOrder
            '
            Me.ReceiveOrder.Name = "ReceiveOrder"
            AddHandler Me.ReceiveOrder.ExecuteCode, AddressOf Me.ReceiveOrderHandler
            '
            'PurchaseOrder
            '
            Me.Activities.Add(Me.ReceiveOrder)
            Me.Activities.Add(Me.OrderTransaction)
            Me.Activities.Add(Me.ProductCheck)
            Me.Activities.Add(Me.faultHandlersActivity1)
            Me.Name = "PurchaseOrder"
            Me.CanModifyActivities = False

        End Sub
        Private CanShip As System.Workflow.Activities.CodeCondition
        Private OrderTransaction As System.Workflow.ComponentModel.CompensatableTransactionScopeActivity
        Private WithdrawFunds As System.Workflow.Activities.CodeActivity
        Private Discontinued As System.Workflow.Activities.IfElseBranchActivity
        Private Available As System.Workflow.Activities.IfElseBranchActivity
        Private ProductCheck As System.Workflow.Activities.IfElseActivity
        Private Ship As System.Workflow.Activities.CodeActivity
        Private NoProductFault As System.Workflow.ComponentModel.ThrowActivity
        Private faultHandlersActivity1 As System.Workflow.ComponentModel.FaultHandlersActivity
        Private NoProductFaultHandler As System.Workflow.ComponentModel.FaultHandlerActivity
        Private DiscontinuedProduct As System.Workflow.Activities.CodeActivity
        Private CompensateRefund As System.Workflow.ComponentModel.CompensateActivity
        Private CompensateOrder As System.Workflow.ComponentModel.CompensationHandlerActivity
        Private Refund As System.Workflow.Activities.CodeActivity
        Private ReceiveOrder As System.Workflow.Activities.CodeActivity

    End Class
End Namespace