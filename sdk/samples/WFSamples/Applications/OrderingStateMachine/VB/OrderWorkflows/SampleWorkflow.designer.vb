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

Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class SampleWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding1 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Me.terminateWorkflow = New System.Workflow.ComponentModel.TerminateActivity
        Me.handleOrderCanceled = New System.Workflow.Activities.HandleExternalEventActivity
        Me.setOrderCompletedState = New System.Workflow.Activities.SetStateActivity
        Me.handleOrderShipped = New System.Workflow.Activities.HandleExternalEventActivity
        Me.setOrderOpenState3 = New System.Workflow.Activities.SetStateActivity
        Me.handleOrderUpdated2 = New System.Workflow.Activities.HandleExternalEventActivity
        Me.setOrderProcessedState = New System.Workflow.Activities.SetStateActivity
        Me.handleOrderProcessed = New System.Workflow.Activities.HandleExternalEventActivity
        Me.setOrderOpenState2 = New System.Workflow.Activities.SetStateActivity
        Me.handleOrderUpdated = New System.Workflow.Activities.HandleExternalEventActivity
        Me.setOrderOpenState = New System.Workflow.Activities.SetStateActivity
        Me.handleOrderCreated = New System.Workflow.Activities.HandleExternalEventActivity
        Me.OnOrderCanceled = New System.Workflow.Activities.EventDrivenActivity
        Me.OnOrderShipped = New System.Workflow.Activities.EventDrivenActivity
        Me.OnOrderUpdated2 = New System.Workflow.Activities.EventDrivenActivity
        Me.OnOrderProcessed = New System.Workflow.Activities.EventDrivenActivity
        Me.OnOrderUpdated = New System.Workflow.Activities.EventDrivenActivity
        Me.OnOrderCreated = New System.Workflow.Activities.EventDrivenActivity
        Me.OrderProcessedState = New System.Workflow.Activities.StateActivity
        Me.OrderOpenState = New System.Workflow.Activities.StateActivity
        Me.OrderCompletedState = New System.Workflow.Activities.StateActivity
        Me.WaitingForOrderState = New System.Workflow.Activities.StateActivity
        '
        'terminateWorkflow
        '
        Me.terminateWorkflow.Name = "terminateWorkflow"
        '
        'handleOrderCanceled
        '
        Me.handleOrderCanceled.EventName = "OrderCanceled"
        Me.handleOrderCanceled.InterfaceType = GetType(IOrderService)
        Me.handleOrderCanceled.Name = "handleOrderCanceled"
        '
        'setOrderCompletedState
        '
        Me.setOrderCompletedState.Name = "setOrderCompletedState"
        Me.setOrderCompletedState.TargetStateName = "OrderCompletedState"
        '
        'handleOrderShipped
        '
        Me.handleOrderShipped.EventName = "OrderShipped"
        Me.handleOrderShipped.InterfaceType = GetType(IOrderService)
        Me.handleOrderShipped.Name = "handleOrderShipped"
        '
        'setOrderOpenState3
        '
        Me.setOrderOpenState3.Name = "setOrderOpenState3"
        Me.setOrderOpenState3.TargetStateName = "OrderOpenState"
        '
        'handleOrderUpdated2
        '
        Me.handleOrderUpdated2.EventName = "OrderUpdated"
        Me.handleOrderUpdated2.InterfaceType = GetType(IOrderService)
        Me.handleOrderUpdated2.Name = "handleOrderUpdated2"
        '
        'setOrderProcessedState
        '
        Me.setOrderProcessedState.Name = "setOrderProcessedState"
        Me.setOrderProcessedState.TargetStateName = "OrderProcessedState"
        '
        'handleOrderProcessed
        '
        Me.handleOrderProcessed.EventName = "OrderProcessed"
        Me.handleOrderProcessed.InterfaceType = GetType(IOrderService)
        Me.handleOrderProcessed.Name = "handleOrderProcessed"
        '
        'setOrderOpenState2
        '
        Me.setOrderOpenState2.Name = "setOrderOpenState2"
        Me.setOrderOpenState2.TargetStateName = "OrderOpenState"
        '
        'handleOrderUpdated
        '
        Me.handleOrderUpdated.EventName = "OrderUpdated"
        Me.handleOrderUpdated.InterfaceType = GetType(IOrderService)
        Me.handleOrderUpdated.Name = "handleOrderUpdated"
        '
        'setOrderOpenState
        '
        Me.setOrderOpenState.Name = "setOrderOpenState"
        Me.setOrderOpenState.TargetStateName = "OrderOpenState"
        '
        'handleOrderCreated
        '
        Me.handleOrderCreated.EventName = "OrderCreated"
        Me.handleOrderCreated.InterfaceType = GetType(IOrderService)
        Me.handleOrderCreated.Name = "handleOrderCreated"
        activitybind1.Name = "SampleWorkflow"
        activitybind1.Path = "OrderSender"
        workflowparameterbinding1.ParameterName = "sender"
        workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        Me.handleOrderCreated.ParameterBindings.Add(workflowparameterbinding1)
        '
        'OnOrderCanceled
        '
        Me.OnOrderCanceled.Activities.Add(Me.handleOrderCanceled)
        Me.OnOrderCanceled.Activities.Add(Me.terminateWorkflow)
        Me.OnOrderCanceled.Name = "OnOrderCanceled"
        '
        'OnOrderShipped
        '
        Me.OnOrderShipped.Activities.Add(Me.handleOrderShipped)
        Me.OnOrderShipped.Activities.Add(Me.setOrderCompletedState)
        Me.OnOrderShipped.Name = "OnOrderShipped"
        '
        'OnOrderUpdated2
        '
        Me.OnOrderUpdated2.Activities.Add(Me.handleOrderUpdated2)
        Me.OnOrderUpdated2.Activities.Add(Me.setOrderOpenState3)
        Me.OnOrderUpdated2.Name = "OnOrderUpdated2"
        '
        'OnOrderProcessed
        '
        Me.OnOrderProcessed.Activities.Add(Me.handleOrderProcessed)
        Me.OnOrderProcessed.Activities.Add(Me.setOrderProcessedState)
        Me.OnOrderProcessed.Name = "OnOrderProcessed"
        '
        'OnOrderUpdated
        '
        Me.OnOrderUpdated.Activities.Add(Me.handleOrderUpdated)
        Me.OnOrderUpdated.Activities.Add(Me.setOrderOpenState2)
        Me.OnOrderUpdated.Name = "OnOrderUpdated"
        '
        'OnOrderCreated
        '
        Me.OnOrderCreated.Activities.Add(Me.handleOrderCreated)
        Me.OnOrderCreated.Activities.Add(Me.setOrderOpenState)
        Me.OnOrderCreated.Name = "OnOrderCreated"
        '
        'OrderProcessedState
        '
        Me.OrderProcessedState.Activities.Add(Me.OnOrderUpdated2)
        Me.OrderProcessedState.Activities.Add(Me.OnOrderShipped)
        Me.OrderProcessedState.Activities.Add(Me.OnOrderCanceled)
        Me.OrderProcessedState.Name = "OrderProcessedState"
        '
        'OrderOpenState
        '
        Me.OrderOpenState.Activities.Add(Me.OnOrderUpdated)
        Me.OrderOpenState.Activities.Add(Me.OnOrderProcessed)
        Me.OrderOpenState.Name = "OrderOpenState"
        '
        'OrderCompletedState
        '
        Me.OrderCompletedState.Name = "OrderCompletedState"
        '
        'WaitingForOrderState
        '
        Me.WaitingForOrderState.Activities.Add(Me.OnOrderCreated)
        Me.WaitingForOrderState.Name = "WaitingForOrderState"
        '
        'SampleWorkflow
        '
        Me.Activities.Add(Me.WaitingForOrderState)
        Me.Activities.Add(Me.OrderCompletedState)
        Me.Activities.Add(Me.OrderOpenState)
        Me.Activities.Add(Me.OrderProcessedState)
        Me.CompletedStateName = "OrderCompletedState"
        Me.DynamicUpdateCondition = Nothing
        Me.InitialStateName = "WaitingForOrderState"
        Me.Name = "SampleWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private OrderCompletedState As System.Workflow.Activities.StateActivity
    Private OrderOpenState As System.Workflow.Activities.StateActivity
    Private OrderProcessedState As System.Workflow.Activities.StateActivity
    Private OnOrderCreated As System.Workflow.Activities.EventDrivenActivity
    Private OnOrderUpdated As System.Workflow.Activities.EventDrivenActivity
    Private OnOrderProcessed As System.Workflow.Activities.EventDrivenActivity
    Private OnOrderUpdated2 As System.Workflow.Activities.EventDrivenActivity
    Private OnOrderShipped As System.Workflow.Activities.EventDrivenActivity
    Private OnOrderCanceled As System.Workflow.Activities.EventDrivenActivity
    Private handleOrderCreated As System.Workflow.Activities.HandleExternalEventActivity
    Private setOrderOpenState As System.Workflow.Activities.SetStateActivity
    Private handleOrderUpdated As System.Workflow.Activities.HandleExternalEventActivity
    Private setOrderOpenState2 As System.Workflow.Activities.SetStateActivity
    Private handleOrderProcessed As System.Workflow.Activities.HandleExternalEventActivity
    Private setOrderProcessedState As System.Workflow.Activities.SetStateActivity
    Private handleOrderUpdated2 As System.Workflow.Activities.HandleExternalEventActivity
    Private setOrderOpenState3 As System.Workflow.Activities.SetStateActivity
    Private handleOrderShipped As System.Workflow.Activities.HandleExternalEventActivity
    Private setOrderCompletedState As System.Workflow.Activities.SetStateActivity
    Private handleOrderCanceled As System.Workflow.Activities.HandleExternalEventActivity
    Private terminateWorkflow As System.Workflow.ComponentModel.TerminateActivity
    Private WaitingForOrderState As System.Workflow.Activities.StateActivity


End Class
