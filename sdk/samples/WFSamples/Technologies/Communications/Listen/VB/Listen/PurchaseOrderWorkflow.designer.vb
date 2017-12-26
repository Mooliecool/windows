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
Imports System.ComponentModel
Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class PurchaseOrderWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.CreatePO = New System.Workflow.Activities.CallExternalMethodActivity
        Me.POStatusListen = New System.Workflow.Activities.ListenActivity
        Me.OnOrderApprovedEventDriven = New System.Workflow.Activities.EventDrivenActivity
        Me.OnOrderRejectedEventDriven = New System.Workflow.Activities.EventDrivenActivity
        Me.OnTimeoutEventDriven = New System.Workflow.Activities.EventDrivenActivity
        Me.ApprovePO = New System.Workflow.Activities.HandleExternalEventActivity
        Me.RejectPO = New System.Workflow.Activities.HandleExternalEventActivity
        Me.Delay = New System.Workflow.Activities.DelayActivity
        Me.Timeout = New System.Workflow.Activities.CodeActivity
        '
        'CreatePO
        '
        Me.CreatePO.InterfaceType = GetType(IOrderService)
        Me.CreatePO.MethodName = "CreateOrder"
        Me.CreatePO.Name = "CreatePO"
        AddHandler Me.CreatePO.MethodInvoking, AddressOf Me.OnBeforeCreateOrder
        '
        'POStatusListen
        '
        Me.POStatusListen.Activities.Add(Me.OnOrderApprovedEventDriven)
        Me.POStatusListen.Activities.Add(Me.OnOrderRejectedEventDriven)
        Me.POStatusListen.Activities.Add(Me.OnTimeoutEventDriven)
        Me.POStatusListen.Name = "POStatusListen"
        '
        'OnOrderApprovedEventDriven
        '
        Me.OnOrderApprovedEventDriven.Activities.Add(Me.ApprovePO)
        Me.OnOrderApprovedEventDriven.Name = "OnOrderApprovedEventDriven"
        '
        'OnOrderRejectedEventDriven
        '
        Me.OnOrderRejectedEventDriven.Activities.Add(Me.RejectPO)
        Me.OnOrderRejectedEventDriven.Name = "OnOrderRejectedEventDriven"
        '
        'OnTimeoutEventDriven
        '
        Me.OnTimeoutEventDriven.Activities.Add(Me.Delay)
        Me.OnTimeoutEventDriven.Activities.Add(Me.Timeout)
        Me.OnTimeoutEventDriven.Name = "OnTimeoutEventDriven"
        '
        'ApprovePO
        '
        Me.ApprovePO.EventName = "OrderApproved"
        Me.ApprovePO.InterfaceType = GetType(IOrderService)
        Me.ApprovePO.Name = "ApprovePO"
        AddHandler Me.ApprovePO.Invoked, AddressOf Me.OnApprovePO
        '
        'RejectPO
        '
        Me.RejectPO.EventName = "OrderRejected"
        Me.RejectPO.InterfaceType = GetType(IOrderService)
        Me.RejectPO.Name = "RejectPO"
        AddHandler Me.RejectPO.Invoked, AddressOf Me.OnRejectPO
        '
        'Delay
        '
        Me.Delay.Name = "Delay"
        Me.Delay.TimeoutDuration = System.TimeSpan.Parse("00:00:05")
        '
        'Timeout
        '
        Me.Timeout.Name = "Timeout"
        AddHandler Me.Timeout.ExecuteCode, AddressOf Me.OnTimeout
        '
        'PurchaseOrderWorkflow
        '
        Me.Activities.Add(Me.CreatePO)
        Me.Activities.Add(Me.POStatusListen)
        Me.Name = "PurchaseOrderWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private POStatusListen As System.Workflow.Activities.ListenActivity
    Private OnOrderApprovedEventDriven As System.Workflow.Activities.EventDrivenActivity
    Private OnOrderRejectedEventDriven As System.Workflow.Activities.EventDrivenActivity
    Private ApprovePO As System.Workflow.Activities.HandleExternalEventActivity
    Private RejectPO As System.Workflow.Activities.HandleExternalEventActivity
    Private OnTimeoutEventDriven As System.Workflow.Activities.EventDrivenActivity
    Private Delay As System.Workflow.Activities.DelayActivity
    Private Timeout As System.Workflow.Activities.CodeActivity
    Private CreatePO As System.Workflow.Activities.CallExternalMethodActivity

End Class
