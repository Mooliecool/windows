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
Partial Public Class DynamicUpdateWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.PORequest = New System.Workflow.Activities.CodeActivity
        Me.CreditCheckDelay = New System.Workflow.Activities.DelayActivity
        Me.CheckApproval = New System.Workflow.Activities.IfElseActivity
        Me.POCreated = New System.Workflow.Activities.CodeActivity
        Me.Approved = New System.Workflow.Activities.IfElseBranchActivity
        Me.AddApprovalStep = New System.Workflow.Activities.CodeActivity
        '
        'PORequest
        '
        Me.PORequest.Name = "PORequest"
        AddHandler Me.PORequest.ExecuteCode, AddressOf Me.OnPORequest
        '
        'CreditCheckDelay
        '
        Me.CreditCheckDelay.Name = "CreditCheckDelay"
        Me.CreditCheckDelay.TimeoutDuration = System.TimeSpan.Parse("00:00:02")
        AddHandler Me.CreditCheckDelay.InitializeTimeoutDuration, AddressOf Me.OnCreditCheckDelay
        '
        'CheckApproval
        '
        Me.CheckApproval.Activities.Add(Me.Approved)
        Me.CheckApproval.Name = "CheckApproval"
        '
        'POCreated
        '
        Me.POCreated.Name = "POCreated"
        AddHandler Me.POCreated.ExecuteCode, AddressOf Me.OnPOCreated
        '
        'Approved
        '
        Me.Approved.Activities.Add(Me.AddApprovalStep)
        AddHandler codecondition1.Condition, AddressOf Me.AddApprovalStepCondition
        Me.Approved.Condition = codecondition1
        Me.Approved.Name = "Approved"
        '
        'AddNewStep
        '
        Me.AddApprovalStep.Name = "AddApprovalStep"
        AddHandler Me.AddApprovalStep.ExecuteCode, AddressOf Me.OnAddApprovalStep
        '
        'DynamicUpdateWorkflow
        '
        Me.Activities.Add(Me.PORequest)
        Me.Activities.Add(Me.CreditCheckDelay)
        Me.Activities.Add(Me.CheckApproval)
        Me.Activities.Add(Me.POCreated)
        Me.Name = "DynamicUpdateWorkflow"
        Me.CanModifyActivities = False
    End Sub
    Private WithEvents CreditCheckDelay As System.Workflow.Activities.DelayActivity
    Private WithEvents CheckApproval As System.Workflow.Activities.IfElseActivity
    Private WithEvents Approved As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents POCreated As System.Workflow.Activities.CodeActivity
    Private WithEvents AddApprovalStep As System.Workflow.Activities.CodeActivity
    Private WithEvents PORequest As System.Workflow.Activities.CodeActivity

End Class
