'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class BalanceTransferWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding1 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Dim activitybind2 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding2 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Dim activitybind5 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding3 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Dim activitybind6 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding4 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Dim activitybind3 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim activitybind4 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim activitybind7 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding5 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Dim activitybind8 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding6 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Me.InvokeBeforeTransaction = New System.Workflow.Activities.CallExternalMethodActivity
        Me.CodeDisplayAccountBalances1 = New System.Workflow.Activities.CodeActivity
        Me.transactionScope1 = New System.Workflow.ComponentModel.TransactionScopeActivity
        Me.InvokeAfterTransaction = New System.Workflow.Activities.CallExternalMethodActivity
        Me.CodeDisplayAccountBalances2 = New System.Workflow.Activities.CodeActivity
        Me.exceptionHandlers1 = New System.Workflow.ComponentModel.FaultHandlersActivity
        Me.CreditAmount1 = New Microsoft.Samples.Workflow.TransactionalService.CreditAmount
        Me.DebitAmount1 = New Microsoft.Samples.Workflow.TransactionalService.DebitAmount
        Me.exceptionHandler1 = New System.Workflow.ComponentModel.FaultHandlerActivity
        Me.CodeNotify1 = New System.Workflow.Activities.CodeActivity
        Me.InvokeQueryAccount = New System.Workflow.Activities.CallExternalMethodActivity
        Me.CodeDisplayAccountBalances = New System.Workflow.Activities.CodeActivity
        '
        'InvokeBeforeTransaction
        '
        Me.InvokeBeforeTransaction.InterfaceType = GetType(Microsoft.Samples.Workflow.TransactionalService.IQueryAccountService)
        Me.InvokeBeforeTransaction.MethodName = "QueryAccount"
        Me.InvokeBeforeTransaction.Name = "InvokeBeforeTransaction"
        activitybind1.Name = "BalanceTransferWorkflow"
        activitybind1.Path = "AccountBalances"
        workflowparameterbinding1.ParameterName = "(ReturnValue)"
        workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        activitybind2.Name = "BalanceTransferWorkflow"
        activitybind2.Path = "AccountNumber"
        workflowparameterbinding2.ParameterName = "accountNumber"
        workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind2, System.Workflow.ComponentModel.ActivityBind))
        Me.InvokeBeforeTransaction.ParameterBindings.Add(workflowparameterbinding1)
        Me.InvokeBeforeTransaction.ParameterBindings.Add(workflowparameterbinding2)
        AddHandler Me.InvokeBeforeTransaction.MethodInvoking, AddressOf Me.onBeforeTransactionInquiry
        '
        'CodeDisplayAccountBalances1
        '
        Me.CodeDisplayAccountBalances1.Name = "CodeDisplayAccountBalances1"
        AddHandler Me.CodeDisplayAccountBalances1.ExecuteCode, AddressOf Me.onDisplayAccountBalances
        '
        'transactionScope1
        '
        Me.transactionScope1.Activities.Add(Me.CreditAmount1)
        Me.transactionScope1.Activities.Add(Me.DebitAmount1)
        Me.transactionScope1.Name = "transactionScope1"
        Me.transactionScope1.TransactionOptions.IsolationLevel = System.Transactions.IsolationLevel.Serializable
        '
        'InvokeAfterTransaction
        '
        Me.InvokeAfterTransaction.InterfaceType = GetType(Microsoft.Samples.Workflow.TransactionalService.IQueryAccountService)
        Me.InvokeAfterTransaction.MethodName = "QueryAccount"
        Me.InvokeAfterTransaction.Name = "InvokeAfterTransaction"
        activitybind5.Name = "BalanceTransferWorkflow"
        activitybind5.Path = "AccountBalances"
        workflowparameterbinding3.ParameterName = "(ReturnValue)"
        workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind5, System.Workflow.ComponentModel.ActivityBind))
        activitybind6.Name = "BalanceTransferWorkflow"
        activitybind6.Path = "AccountNumber"
        workflowparameterbinding4.ParameterName = "accountNumber"
        workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind6, System.Workflow.ComponentModel.ActivityBind))
        Me.InvokeAfterTransaction.ParameterBindings.Add(workflowparameterbinding3)
        Me.InvokeAfterTransaction.ParameterBindings.Add(workflowparameterbinding4)
        AddHandler Me.InvokeAfterTransaction.MethodInvoking, AddressOf Me.onAfterTransactionInquiry
        '
        'CodeDisplayAccountBalances2
        '
        Me.CodeDisplayAccountBalances2.Name = "CodeDisplayAccountBalances2"
        AddHandler Me.CodeDisplayAccountBalances2.ExecuteCode, AddressOf Me.onDisplayAccountBalances
        '
        'exceptionHandlers1
        '
        Me.exceptionHandlers1.Activities.Add(Me.exceptionHandler1)
        Me.exceptionHandlers1.Name = "exceptionHandlers1"
        '
        'CreditAmount1
        '
        activitybind3.Name = "BalanceTransferWorkflow"
        activitybind3.Path = "TransferAmount"
        Me.CreditAmount1.Name = "CreditAmount1"
        AddHandler Me.CreditAmount1.OnBeforeInvoke, AddressOf Me.onBeforeCreditAmount
        Me.CreditAmount1.SetBinding(Microsoft.Samples.Workflow.TransactionalService.CreditAmount.AmountProperty, CType(activitybind3, System.Workflow.ComponentModel.ActivityBind))
        '
        'DebitAmount1
        '
        activitybind4.Name = "BalanceTransferWorkflow"
        activitybind4.Path = "TransferAmount"
        Me.DebitAmount1.Name = "DebitAmount1"
        AddHandler Me.DebitAmount1.OnBeforeInvoke, AddressOf Me.onBeforeDebitAmount
        Me.DebitAmount1.SetBinding(Microsoft.Samples.Workflow.TransactionalService.DebitAmount.AmountProperty, CType(activitybind4, System.Workflow.ComponentModel.ActivityBind))
        '
        'exceptionHandler1
        '
        Me.exceptionHandler1.Activities.Add(Me.CodeNotify1)
        Me.exceptionHandler1.Activities.Add(Me.InvokeQueryAccount)
        Me.exceptionHandler1.Activities.Add(Me.CodeDisplayAccountBalances)
        Me.exceptionHandler1.FaultType = GetType(System.Exception)
        Me.exceptionHandler1.Name = "exceptionHandler1"
        '
        'CodeNotify1
        '
        Me.CodeNotify1.Name = "CodeNotify1"
        AddHandler Me.CodeNotify1.ExecuteCode, AddressOf Me.onExecuteCode
        '
        'InvokeQueryAccount
        '
        Me.InvokeQueryAccount.InterfaceType = GetType(Microsoft.Samples.Workflow.TransactionalService.IQueryAccountService)
        Me.InvokeQueryAccount.MethodName = "QueryAccount"
        Me.InvokeQueryAccount.Name = "InvokeQueryAccount"
        activitybind7.Name = "BalanceTransferWorkflow"
        activitybind7.Path = "AccountBalances"
        workflowparameterbinding5.ParameterName = "(ReturnValue)"
        workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind7, System.Workflow.ComponentModel.ActivityBind))
        activitybind8.Name = "BalanceTransferWorkflow"
        activitybind8.Path = "AccountNumber"
        workflowparameterbinding6.ParameterName = "accountNumber"
        workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind8, System.Workflow.ComponentModel.ActivityBind))
        Me.InvokeQueryAccount.ParameterBindings.Add(workflowparameterbinding5)
        Me.InvokeQueryAccount.ParameterBindings.Add(workflowparameterbinding6)
        '
        'CodeDisplayAccountBalances
        '
        Me.CodeDisplayAccountBalances.Name = "CodeDisplayAccountBalances"
        AddHandler Me.CodeDisplayAccountBalances.ExecuteCode, AddressOf Me.onDisplayAccountBalances
        '
        'BalanceTransferWorkflow
        '
        Me.Activities.Add(Me.InvokeBeforeTransaction)
        Me.Activities.Add(Me.CodeDisplayAccountBalances1)
        Me.Activities.Add(Me.transactionScope1)
        Me.Activities.Add(Me.InvokeAfterTransaction)
        Me.Activities.Add(Me.CodeDisplayAccountBalances2)
        Me.Activities.Add(Me.exceptionHandlers1)
        Me.Name = "BalanceTransferWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private TransactionComposite As System.Workflow.ComponentModel.TransactionScopeActivity
    Private CreditAmount1 As Microsoft.Samples.Workflow.TransactionalService.CreditAmount
    Private DebitAmount1 As Microsoft.Samples.Workflow.TransactionalService.DebitAmount
    Private transactionScope1 As System.Workflow.ComponentModel.TransactionScopeActivity
    Private CodeNotify1 As System.Workflow.Activities.CodeActivity
    Private InvokeQueryAccount As System.Workflow.Activities.CallExternalMethodActivity
    Private CodeDisplayAccountBalances As System.Workflow.Activities.CodeActivity
    Private exceptionHandlers1 As System.Workflow.ComponentModel.FaultHandlersActivity
    Private exceptionHandler1 As System.Workflow.ComponentModel.FaultHandlerActivity
    Private InvokeAfterTransaction As System.Workflow.Activities.CallExternalMethodActivity
    Private CodeDisplayAccountBalances2 As System.Workflow.Activities.CodeActivity
    Private CodeDisplayAccountBalances1 As System.Workflow.Activities.CodeActivity
    Private InvokeBeforeTransaction As System.Workflow.Activities.CallExternalMethodActivity

End Class
